// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Depth Viewer — View depth stream with 3D rendering, colormap, and PNG save.
// Demonstrates: Pipeline depth config, renderDepth3D(), cv::Mat from DepthFrame, single-window rendering.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

static cv::Mat depthFrameToColorized(const std::shared_ptr<ob::DepthFrame> &depthFrame, int colormapId) {
    if(!depthFrame || depthFrame->getFormat() != OB_FORMAT_Y16) {
        return cv::Mat();
    }

    uint32_t        width  = depthFrame->getWidth();
    uint32_t        height = depthFrame->getHeight();
    const uint16_t *data   = reinterpret_cast<const uint16_t *>(depthFrame->getData());

    cv::Mat raw(height, width, CV_16UC1, (void *)data);
    cv::Mat normalized;
    raw.convertTo(normalized, CV_8UC1, 255.0 / 8000.0);  // Normalize to 0-8 meters range
    cv::Mat colored;
    cv::applyColorMap(normalized, colored, colormapId);
    return colored;
}

int main(void) try {
    // Create a pipeline with default device.
    ob::Pipeline pipe;

    // Enable the depth stream.
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH);

    // Start the pipeline.
    pipe.start(config);

    bool     render3D   = true;  // Default: 3D rendering ON
    int      colormapId = cv::COLORMAP_JET;
    uint32_t saveIndex  = 0;

    std::shared_ptr<ob::DepthFrame> latestDepth;
    bool                            saveRequested = false;

    std::cout << "Depth Viewer Controls:\n"
              << "  M/m: Toggle 3D rendering (currently ON)\n"
              << "  C/c: Cycle colormap (used in 2D mode)\n"
              << "  S/s: Save depth PNG\n"
              << "  Esc: Exit\n";

    while(true) {
        auto frameSet = pipe.waitForFrameset(100);
        if(frameSet == nullptr)
            continue;

        auto depthFrameRaw = frameSet->getFrame(OB_FRAME_DEPTH);
        if(!depthFrameRaw)
            continue;

        auto depthFrame = depthFrameRaw->as<ob::DepthFrame>();
        latestDepth     = depthFrame;

        // Print center distance every 30 frames
        if(depthFrame->getIndex() % 30 == 0 && depthFrame->getFormat() == OB_FORMAT_Y16) {
            uint32_t        width          = depthFrame->getWidth();
            uint32_t        height         = depthFrame->getHeight();
            float           scale          = depthFrame->getValueScale();
            const uint16_t *data           = reinterpret_cast<const uint16_t *>(depthFrame->getData());
            float           centerDistance = data[width * height / 2 + width / 2] * scale;
            std::cout << "Center distance: " << centerDistance << " mm\n";
        }

        // Render depth in main window
        cv::Mat displayMat;
        if(render3D) {
            displayMat = ob_smpl::renderDepth3D(depthFrame, colormapId);
        }
        else {
            displayMat = depthFrameToColorized(depthFrame, colormapId);
        }

        if(!displayMat.empty()) {
            // Draw operation prompt
            cv::putText(displayMat, "M:3D/2D  C:Colormap  S:Save  Esc:Exit", cv::Point(12, displayMat.rows - 14), cv::FONT_HERSHEY_DUPLEX, 0.55,
                        cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
            // Draw mode indicator
            std::string mode = render3D ? "3D Rendering" : "2D Colormap";
            cv::putText(displayMat, mode, cv::Point(12, 24), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
            cv::imshow("Depth Viewer", displayMat);
        }

        // Key handling
        int key = cv::waitKey(1);
        if(key == 27) {  // ESC
            break;
        }
        else if(key == 'm' || key == 'M') {
            render3D = !render3D;
            std::cout << (render3D ? "3D depth rendering ON\n" : "3D depth rendering OFF (colormap ON)\n");
        }
        else if(key == 'c' || key == 'C') {
            const int   maps[]  = { cv::COLORMAP_JET, cv::COLORMAP_TURBO, cv::COLORMAP_MAGMA, cv::COLORMAP_INFERNO, cv::COLORMAP_PLASMA };
            const char *names[] = { "JET", "TURBO", "MAGMA", "INFERNO", "PLASMA" };
            const int   count   = sizeof(maps) / sizeof(maps[0]);
            int         idx     = 0;
            for(int i = 0; i < count; i++) {
                if(maps[i] == colormapId) {
                    idx = (i + 1) % count;
                    break;
                }
            }
            colormapId = maps[idx];
            std::cout << "Colormap: " << names[idx] << "\n";
        }
        else if(key == 's' || key == 'S') {
            saveRequested = true;
        }

        // Save depth PNG
        if(saveRequested && latestDepth) {
            uint32_t    w = latestDepth->getWidth();
            uint32_t    h = latestDepth->getHeight();
            cv::Mat     rawMat(h, w, CV_16UC1, latestDepth->getData());
            std::string name = "depth_" + std::to_string(w) + "x" + std::to_string(h) + "_" + std::to_string(saveIndex) + ".png";
            cv::imwrite(name, rawMat, { cv::IMWRITE_PNG_COMPRESSION, 0 });
            std::cout << "Saved: " << name << "\n";
            saveIndex++;
            saveRequested = false;
        }
    }

    pipe.stop();
    cv::destroyAllWindows();
    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
