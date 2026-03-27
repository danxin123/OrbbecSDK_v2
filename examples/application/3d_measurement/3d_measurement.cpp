// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// 3D Measurement — Click two points on an aligned RGBD image to measure real-world distance.
// Demonstrates: Align (D2C), mouse-based 2D→3D projection, Euclidean distance calculation.

#include <libobsensor/ObSensor.hpp>
#include "libobsensor/hpp/Utils.hpp"

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>
#include <cmath>
#include <mutex>
#include <vector>

// Measurement state shared between mouse callback and main loop.
struct MeasureState {
    std::mutex             mtx;
    std::vector<cv::Point> clickedPoints;  // 0, 1, or 2 points
    OBPoint3f              point3D[2];     // 3D coordinates of the two points (mm)
    float                  distance;       // Euclidean distance in mm
    bool                   valid;          // true if distance is valid
};

static MeasureState g_state;

// Mouse callback: record click positions (left button down)
static void onMouse(int event, int x, int y, int /*flags*/, void * /*userdata*/) {
    if(event != cv::EVENT_LBUTTONDOWN)
        return;

    std::lock_guard<std::mutex> lock(g_state.mtx);
    if(g_state.clickedPoints.size() >= 2) {
        // Reset for a new measurement
        g_state.clickedPoints.clear();
        g_state.valid = false;
    }
    g_state.clickedPoints.push_back(cv::Point(x, y));
}

// Project a 2D pixel + depth to 3D using aligned depth intrinsics.
// Returns false if depth is 0 or projection fails.
static bool project2Dto3D(int px, int py, const std::shared_ptr<ob::DepthFrame> &depthFrame, OBPoint3f &out) {
    uint32_t w = depthFrame->getWidth();
    uint32_t h = depthFrame->getHeight();
    if(px < 0 || py < 0 || (uint32_t)px >= w || (uint32_t)py >= h)
        return false;

    const uint16_t *depthData = reinterpret_cast<const uint16_t *>(depthFrame->getData());
    float           depthMm   = static_cast<float>(depthData[py * w + px]) * depthFrame->getValueScale();
    if(depthMm < 1.0f)
        return false;  // No valid depth at this pixel

    // After D2C alignment, depth frame carries the color camera's intrinsics.
    auto depthProfile = depthFrame->getStreamProfile()->as<ob::VideoStreamProfile>();
    auto intrinsic    = depthProfile->getIntrinsic();

    // Identity extrinsic (same coordinate system after alignment)
    OBExtrinsic identity = {};
    identity.rot[0]      = 1.0f;
    identity.rot[4]      = 1.0f;
    identity.rot[8]      = 1.0f;

    OBPoint2f src = { static_cast<float>(px), static_cast<float>(py) };
    return ob::CoordinateTransformHelper::transformation2dto3d(src, depthMm, intrinsic, identity, &out);
}

int main(void) try {
    // Configure Pipeline with Color + Depth
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->enableVideoStream(OB_STREAM_COLOR, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    auto pipe = std::make_shared<ob::Pipeline>();
    pipe->start(config);

    // Software Align: depth → color coordinate space
    auto alignFilter = std::make_shared<ob::Align>(OB_STREAM_COLOR);

    // Format converter for color display
    auto converter = std::make_shared<ob::FormatConvertFilter>();

    const std::string winName = "3D Measurement (click 2 points)";
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(winName, onMouse, nullptr);

    std::cout << "=== 3D Measurement ===" << std::endl;
    std::cout << "Left-click two points on the image to measure real-world distance." << std::endl;
    std::cout << "Press 'R' to reset, 'Esc' to exit." << std::endl;

    while(true) {
        auto frameSet = pipe->waitForFrameset(100);
        if(!frameSet)
            continue;

        // Align depth to color
        auto aligned = alignFilter->process(frameSet);
        if(!aligned)
            continue;
        auto alignedSet = aligned->as<ob::FrameSet>();
        if(!alignedSet)
            continue;

        auto depthFrame = alignedSet->getFrame(OB_FRAME_DEPTH)->as<ob::DepthFrame>();
        auto colorFrame = alignedSet->getFrame(OB_FRAME_COLOR);
        if(!depthFrame || !colorFrame)
            continue;

        // Convert color to BGR for OpenCV display
        std::shared_ptr<ob::Frame> bgrFrame = colorFrame;
        auto                       fmt      = colorFrame->as<ob::VideoFrame>()->getFormat();
        if(fmt == OB_FORMAT_MJPG) {
            converter->setFormatConvertType(FORMAT_MJPG_TO_BGR);
            bgrFrame = converter->process(colorFrame);
        }
        else if(fmt == OB_FORMAT_RGB) {
            converter->setFormatConvertType(FORMAT_RGB_TO_BGR);
            bgrFrame = converter->process(colorFrame);
        }
        else if(fmt == OB_FORMAT_UYVY) {
            converter->setFormatConvertType(FORMAT_UYVY_TO_RGB);
            bgrFrame = converter->process(colorFrame);
            converter->setFormatConvertType(FORMAT_RGB_TO_BGR);
            bgrFrame = converter->process(bgrFrame);
        }
        else if(fmt == OB_FORMAT_YUYV) {
            converter->setFormatConvertType(FORMAT_YUYV_TO_RGB);
            bgrFrame = converter->process(colorFrame);
            converter->setFormatConvertType(FORMAT_RGB_TO_BGR);
            bgrFrame = converter->process(bgrFrame);
        }

        auto    videoFrame = bgrFrame->as<ob::VideoFrame>();
        cv::Mat displayMat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData());

        // Process measurement under lock
        {
            std::lock_guard<std::mutex> lock(g_state.mtx);

            // If we have two clicked points, try to compute 3D distance
            if(g_state.clickedPoints.size() == 2 && !g_state.valid) {
                bool ok1 = project2Dto3D(g_state.clickedPoints[0].x, g_state.clickedPoints[0].y, depthFrame, g_state.point3D[0]);
                bool ok2 = project2Dto3D(g_state.clickedPoints[1].x, g_state.clickedPoints[1].y, depthFrame, g_state.point3D[1]);
                if(ok1 && ok2) {
                    float dx         = g_state.point3D[1].x - g_state.point3D[0].x;
                    float dy         = g_state.point3D[1].y - g_state.point3D[0].y;
                    float dz         = g_state.point3D[1].z - g_state.point3D[0].z;
                    g_state.distance = std::sqrt(dx * dx + dy * dy + dz * dz);
                    g_state.valid    = true;

                    std::cout << "Point A: (" << g_state.point3D[0].x << ", " << g_state.point3D[0].y << ", " << g_state.point3D[0].z << ") mm" << std::endl;
                    std::cout << "Point B: (" << g_state.point3D[1].x << ", " << g_state.point3D[1].y << ", " << g_state.point3D[1].z << ") mm" << std::endl;
                    std::cout << "Distance: " << g_state.distance << " mm" << std::endl;
                }
                else {
                    // Depth invalid at one or both points
                    g_state.clickedPoints.clear();
                    std::cout << "No valid depth at clicked point(s). Try again." << std::endl;
                }
            }

            // Draw click markers
            for(size_t i = 0; i < g_state.clickedPoints.size(); i++) {
                cv::Scalar color = (i == 0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
                cv::circle(displayMat, g_state.clickedPoints[i], 6, color, 2);
                cv::circle(displayMat, g_state.clickedPoints[i], 2, color, -1);

                // Show individual point label
                std::string label = (i == 0) ? "A" : "B";
                cv::putText(displayMat, label, g_state.clickedPoints[i] + cv::Point(10, -10), cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
            }

            // Draw measurement result
            if(g_state.valid && g_state.clickedPoints.size() == 2) {
                cv::line(displayMat, g_state.clickedPoints[0], g_state.clickedPoints[1], cv::Scalar(255, 255, 0), 2);

                // Show distance at the midpoint of the line
                cv::Point mid = (g_state.clickedPoints[0] + g_state.clickedPoints[1]) / 2;
                char      buf[64];
                snprintf(buf, sizeof(buf), "%.1f mm", g_state.distance);
                cv::putText(displayMat, buf, mid + cv::Point(0, -15), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);

                // Show 3D coordinates
                char bufA[128], bufB[128];
                snprintf(bufA, sizeof(bufA), "A(%.0f,%.0f,%.0f)", g_state.point3D[0].x, g_state.point3D[0].y, g_state.point3D[0].z);
                snprintf(bufB, sizeof(bufB), "B(%.0f,%.0f,%.0f)", g_state.point3D[1].x, g_state.point3D[1].y, g_state.point3D[1].z);
                cv::putText(displayMat, bufA, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
                cv::putText(displayMat, bufB, cv::Point(10, 55), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
            }
        }

        // Show help text
        cv::putText(displayMat, "Click 2 points to measure | R:Reset | Esc:Exit", cv::Point(10, displayMat.rows - 15), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(200, 200, 200), 1);

        cv::imshow(winName, displayMat);

        int key = cv::waitKey(1);
        if(key == 27)  // Esc
            break;
        if(key == 'r' || key == 'R') {
            std::lock_guard<std::mutex> lock(g_state.mtx);
            g_state.clickedPoints.clear();
            g_state.valid = false;
            std::cout << "Measurement reset." << std::endl;
        }
    }

    pipe->stop();
    cv::destroyAllWindows();
    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
