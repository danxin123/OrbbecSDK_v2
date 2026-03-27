// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// RGBD Viewer — Acquire aligned RGB-D streams and display.
// Demonstrates: Pipeline config, software Depth-to-Color alignment (ob::Align),
//               OVERLAY visualization, save depth/color frames.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

int main(void) try {
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->enableVideoStream(OB_STREAM_COLOR, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    auto pipe = std::make_shared<ob::Pipeline>();
    pipe->start(config);

    // Software Align: depth → color coordinate space
    auto alignFilter = std::make_shared<ob::Align>(OB_STREAM_COLOR);

    // Create a window for rendering with overlay mode.
    ob_smpl::CVWindow win("RGBD Viewer", 1280, 720, ob_smpl::ARRANGE_OVERLAY);
    win.setKeyPrompt("'S': Save frames, 'Esc': Exit");

    uint32_t saveIndex = 0;

    win.setKeyPressedCallback([&](int key) {
        if(key == 's' || key == 'S') {
            auto frameSet = pipe->waitForFrameset(100);
            if(!frameSet) return;

            auto aligned    = alignFilter->process(frameSet);
            auto alignedSet = aligned ? aligned->as<ob::FrameSet>() : nullptr;
            if(!alignedSet) return;

            auto depthRaw = alignedSet->getFrame(OB_FRAME_DEPTH);
            auto colorRaw = alignedSet->getFrame(OB_FRAME_COLOR);

            if(depthRaw) {
                auto baseName = "depth_" + std::to_string(saveIndex);
                auto saved    = ob_smpl::saveFrame(depthRaw, baseName);
                if(!saved.empty()) win.addLog("Saved: " + saved);
            }
            if(colorRaw) {
                auto baseName = "color_" + std::to_string(saveIndex);
                auto saved    = ob_smpl::saveFrame(colorRaw, baseName);
                if(!saved.empty()) win.addLog("Saved: " + saved);
            }
            saveIndex++;
        }
    });

    while(win.run()) {
        auto frameSet = pipe->waitForFrameset(100);
        if(!frameSet) {
            continue;
        }

        // Align depth to color
        auto alignedFrame = alignFilter->process(frameSet);
        if(!alignedFrame) {
            continue;
        }

        auto alignedFrameSet = alignedFrame->as<ob::FrameSet>();
        if(!alignedFrameSet) {
            continue;
        }

        // Push aligned frameset to display (CVWindow handles visualization)
        win.pushFramesToView(alignedFrameSet);
    }

    pipe->stop();

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
