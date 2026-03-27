// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// RGBD Viewer — Acquire aligned RGB-D streams and display with OpenCV.
// Demonstrates: Pipeline config, software Depth-to-Color alignment (ob::Align),
//               OVERLAY visualization, save depth/color PNG, 3D depth rendering.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

static void safeDestroyWindow(const char *name) {
    try {
        if(cv::getWindowProperty(name, cv::WND_PROP_VISIBLE) >= 0) {
            cv::destroyWindow(name);
        }
    }
    catch(const cv::Exception &) {
        // Ignore: window may not exist yet.
    }
}

// Save the current depth frame as a 16-bit PNG (lossless, raw depth values).
void saveDepthFrame(const std::shared_ptr<ob::DepthFrame> &depthFrame, uint32_t index) {
    std::string name = "depth_" + std::to_string(depthFrame->getWidth()) + "x" + std::to_string(depthFrame->getHeight()) + "_" + std::to_string(index) + ".png";
    cv::Mat     mat(depthFrame->getHeight(), depthFrame->getWidth(), CV_16UC1, depthFrame->getData());
    cv::imwrite(name, mat, { cv::IMWRITE_PNG_COMPRESSION, 0 });
    std::cout << "Saved: " << name << std::endl;
}

// Save the current color frame as an 8-bit PNG.
void saveColorFrame(const std::shared_ptr<ob::ColorFrame> &colorFrame, uint32_t index) {
    auto                       converter = std::make_shared<ob::FormatConvertFilter>();
    std::shared_ptr<ob::Frame> rgbFrame;

    if(colorFrame->getFormat() == OB_FORMAT_MJPG) {
        converter->setFormatConvertType(FORMAT_MJPG_TO_BGR);
        auto        bgrFrame   = converter->process(colorFrame);
        auto        videoFrame = bgrFrame->as<ob::VideoFrame>();
        std::string name =
            "color_" + std::to_string(videoFrame->getWidth()) + "x" + std::to_string(videoFrame->getHeight()) + "_" + std::to_string(index) + ".png";
        cv::Mat mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData());
        cv::imwrite(name, mat, { cv::IMWRITE_PNG_COMPRESSION, 0 });
        std::cout << "Saved: " << name << std::endl;
        return;
    }
    else if(colorFrame->getFormat() == OB_FORMAT_UYVY) {
        converter->setFormatConvertType(FORMAT_UYVY_TO_RGB);
        rgbFrame = converter->process(colorFrame);
    }
    else if(colorFrame->getFormat() == OB_FORMAT_YUYV) {
        converter->setFormatConvertType(FORMAT_YUYV_TO_RGB);
        rgbFrame = converter->process(colorFrame);
    }
    else if(colorFrame->getFormat() == OB_FORMAT_RGB) {
        rgbFrame = std::const_pointer_cast<ob::Frame>(std::static_pointer_cast<const ob::Frame>(colorFrame));
    }
    else if(colorFrame->getFormat() == OB_FORMAT_BGR) {
        auto        videoFrame = colorFrame->as<ob::VideoFrame>();
        std::string name =
            "color_" + std::to_string(videoFrame->getWidth()) + "x" + std::to_string(videoFrame->getHeight()) + "_" + std::to_string(index) + ".png";
        cv::Mat mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData());
        cv::imwrite(name, mat, { cv::IMWRITE_PNG_COMPRESSION, 0 });
        std::cout << "Saved: " << name << std::endl;
        return;
    }
    else {
        std::cerr << "Unsupported color format for save, skipping." << std::endl;
        return;
    }

    converter->setFormatConvertType(FORMAT_RGB_TO_BGR);
    auto        bgrFrame   = converter->process(rgbFrame);
    auto        videoFrame = bgrFrame->as<ob::VideoFrame>();
    std::string name = "color_" + std::to_string(videoFrame->getWidth()) + "x" + std::to_string(videoFrame->getHeight()) + "_" + std::to_string(index) + ".png";
    cv::Mat     mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData());
    cv::imwrite(name, mat, { cv::IMWRITE_PNG_COMPRESSION, 0 });
    std::cout << "Saved: " << name << std::endl;
}

static cv::Mat colorFrameToBgr(const std::shared_ptr<ob::ColorFrame> &colorFrame) {
    if(!colorFrame) {
        return cv::Mat();
    }

    auto converter = std::make_shared<ob::FormatConvertFilter>();
    if(colorFrame->getFormat() == OB_FORMAT_MJPG) {
        converter->setFormatConvertType(FORMAT_MJPG_TO_BGR);
        auto bgrFrame   = converter->process(colorFrame);
        auto videoFrame = bgrFrame->as<ob::VideoFrame>();
        return cv::Mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData()).clone();
    }
    if(colorFrame->getFormat() == OB_FORMAT_BGR) {
        auto videoFrame = colorFrame->as<ob::VideoFrame>();
        return cv::Mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData()).clone();
    }

    std::shared_ptr<ob::Frame> rgbFrame;
    if(colorFrame->getFormat() == OB_FORMAT_UYVY) {
        converter->setFormatConvertType(FORMAT_UYVY_TO_RGB);
        rgbFrame = converter->process(colorFrame);
    }
    else if(colorFrame->getFormat() == OB_FORMAT_YUYV) {
        converter->setFormatConvertType(FORMAT_YUYV_TO_RGB);
        rgbFrame = converter->process(colorFrame);
    }
    else if(colorFrame->getFormat() == OB_FORMAT_RGB) {
        rgbFrame = std::const_pointer_cast<ob::Frame>(std::static_pointer_cast<const ob::Frame>(colorFrame));
    }
    else {
        return cv::Mat();
    }

    converter->setFormatConvertType(FORMAT_RGB_TO_BGR);
    auto bgrFrame   = converter->process(rgbFrame);
    auto videoFrame = bgrFrame->as<ob::VideoFrame>();
    return cv::Mat(videoFrame->getHeight(), videoFrame->getWidth(), CV_8UC3, videoFrame->getData()).clone();
}

static cv::Mat depthFrameToColorized(const std::shared_ptr<ob::DepthFrame> &depthFrame, int colormapId) {
    if(!depthFrame) {
        return cv::Mat();
    }

    uint32_t width  = depthFrame->getWidth();
    uint32_t height = depthFrame->getHeight();
    float    scale  = depthFrame->getValueScale();

    cv::Mat rawMat(height, width, CV_16UC1, depthFrame->getData());
    cv::Mat depthMm;
    rawMat.convertTo(depthMm, CV_32F, scale);
    cv::Mat clipped;
    cv::min(cv::max(depthMm, 200.0f), 8000.0f, clipped);
    cv::Mat normalized = (clipped - 200.0f) / (8000.0f - 200.0f);
    cv::Mat depth8;
    normalized.convertTo(depth8, CV_8UC1, 255.0);

    cv::Mat depthColor;
    cv::applyColorMap(depth8, depthColor, colormapId);

    cv::Mat zeroMask;
    cv::compare(rawMat, 0, zeroMask, cv::CMP_EQ);
    depthColor.setTo(cv::Scalar(0, 0, 0), zeroMask);
    return depthColor;
}

int main(void) try {
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->enableVideoStream(OB_STREAM_COLOR, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    auto pipe = std::make_shared<ob::Pipeline>();
    pipe->start(config);

    auto alignFilter = std::make_shared<ob::Align>(OB_STREAM_COLOR);

    const std::string mainWinName = "RGBD Viewer";
    cv::namedWindow(mainWinName, cv::WINDOW_NORMAL);
    cv::resizeWindow(mainWinName, 1280, 720);

    bool     render3D     = true;
    int      colormapId   = cv::COLORMAP_JET;
    float    overlayAlpha = 0.6f;
    uint32_t saveIndex    = 0;

    std::cout << "RGBD Viewer Controls: M/m=Toggle 3D, S/s=Save PNG, C/c=Cycle Colormap, +/-=Overlay Alpha, Esc=Exit" << std::endl;

    while(true) {
        auto frameSet = pipe->waitForFrameset(100);
        if(!frameSet) {
            int key = cv::waitKey(1);
            if(key == ESC_KEY) {
                break;
            }
            continue;
        }

        auto alignedFrame    = alignFilter->process(frameSet);
        auto alignedFrameSet = alignedFrame ? alignedFrame->as<ob::FrameSet>() : nullptr;
        if(!alignedFrameSet) {
            continue;
        }

        auto depthFrameRaw = alignedFrameSet->getFrame(OB_FRAME_DEPTH);
        auto colorFrameRaw = alignedFrameSet->getFrame(OB_FRAME_COLOR);
        if(!depthFrameRaw || !colorFrameRaw) {
            continue;
        }

        auto depthFrame = depthFrameRaw->as<ob::DepthFrame>();
        auto colorFrame = colorFrameRaw->as<ob::ColorFrame>();

        cv::Mat colorMat = colorFrameToBgr(colorFrame);
        cv::Mat depthMat = depthFrameToColorized(depthFrame, colormapId);
        if(colorMat.empty() || depthMat.empty()) {
            continue;
        }

        cv::Mat overlayMat;
        cv::addWeighted(colorMat, 1.0f - overlayAlpha, depthMat, overlayAlpha, 0.0, overlayMat);

        int     panelW = 640;
        int     panelH = 360;
        cv::Mat depthPanel;
        if(render3D) {
            cv::Mat depth3d = ob_smpl::renderDepth3D(depthFrame, colormapId);
            if(!depth3d.empty()) {
                cv::resize(depth3d, depthPanel, cv::Size(panelW, panelH));
            }
        }
        if(depthPanel.empty()) {
            cv::resize(depthMat, depthPanel, cv::Size(panelW, panelH));
        }

        cv::Mat colorPanel, overlayPanel;
        cv::resize(colorMat, colorPanel, cv::Size(panelW, panelH));
        cv::resize(overlayMat, overlayPanel, cv::Size(panelW, panelH * 2));

        cv::Mat leftCol, canvas;
        cv::vconcat(depthPanel, colorPanel, leftCol);
        cv::hconcat(leftCol, overlayPanel, canvas);

        // Draw labels
        cv::putText(canvas, render3D ? "Depth 3D" : "Depth", cv::Point(12, 24), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        cv::putText(canvas, "Color", cv::Point(12, panelH + 24), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        cv::putText(canvas, "Overlay", cv::Point(panelW + 12, 24), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

        // Mode indicator and operation prompt
        std::string modeStr = render3D ? "[3D Mode]" : "[Normal Mode]";
        cv::putText(canvas, modeStr, cv::Point(12, panelH * 2 - 40), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        cv::putText(canvas, "M:3D  S:Save  C:Colormap  +/-:Alpha  Esc:Exit", cv::Point(12, panelH * 2 - 14), cv::FONT_HERSHEY_DUPLEX, 0.55,
                    cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

        cv::imshow(mainWinName, canvas);

        int key = cv::waitKey(1);
        if(key == ESC_KEY) {
            break;
        }
        else if(key == 'm' || key == 'M') {
            render3D = !render3D;
            std::cout << (render3D ? "3D depth rendering ON" : "3D depth rendering OFF") << std::endl;
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
            std::cout << "Colormap: " << names[idx] << std::endl;
        }
        else if(key == '+' || key == '=') {
            overlayAlpha += 0.1f;
            if(overlayAlpha > 1.0f) {
                overlayAlpha = 1.0f;
            }
            std::cout << "Overlay alpha: " << overlayAlpha << std::endl;
        }
        else if(key == '-' || key == '_') {
            overlayAlpha -= 0.1f;
            if(overlayAlpha < 0.0f) {
                overlayAlpha = 0.0f;
            }
            std::cout << "Overlay alpha: " << overlayAlpha << std::endl;
        }
        else if(key == 's' || key == 'S') {
            saveDepthFrame(depthFrame, saveIndex);
            saveColorFrame(colorFrame, saveIndex);
            saveIndex++;
            std::cout << "Frames saved (#" << saveIndex << ")" << std::endl;
        }
    }

    pipe->stop();
    safeDestroyWindow(mainWinName.c_str());

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
