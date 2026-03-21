// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

int main(void) try {
    // Create a pipeline.
    ob::Pipeline pipe;

    // Start the pipeline with default config.
    // Modify the default configuration by the configuration file: "*SDKConfig.xml"
    pipe.start();

    // Create a window for showing the frames, and set the size of the window.
    ob_smpl::CVWindow win("QuickStart", 1280, 720, ob_smpl::ARRANGE_ONE_ROW);

    bool    render3D   = false;
    int     colormapId = cv::COLORMAP_JET;
    win.setKeyPrompt("'Esc': Exit Window, '?': Show Key Map, 'M': Toggle 3D Depth, 'C': Cycle Colormap");
    win.setKeyPressedCallback([&](int key) {
        if(key == 'm' || key == 'M') {
            render3D = !render3D;
            win.addLog(render3D ? "3D depth rendering ON" : "3D depth rendering OFF");
        }
        else if(key == 'c' || key == 'C') {
            // Cycle through colormaps: JET -> TURBO -> MAGMA -> INFERNO -> PLASMA -> JET
            const int maps[] = { cv::COLORMAP_JET, cv::COLORMAP_TURBO, cv::COLORMAP_MAGMA, cv::COLORMAP_INFERNO, cv::COLORMAP_PLASMA };
            const char *names[] = { "JET", "TURBO", "MAGMA", "INFERNO", "PLASMA" };
            const int count = sizeof(maps) / sizeof(maps[0]);
            int idx = 0;
            for(int i = 0; i < count; i++) {
                if(maps[i] == colormapId) { idx = (i + 1) % count; break; }
            }
            colormapId = maps[idx];
            win.addLog(std::string("Colormap: ") + names[idx]);
        }
    });

    while(win.run()) {
        // Wait for frameSet from the pipeline, the default timeout is 1000ms.
        auto frameSet = pipe.waitForFrameset();
        if(!frameSet) continue;

        if(render3D) {
            // In 3D mode, render depth with lighting effect and show color alongside
            auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH);
            auto colorFrame = frameSet->getFrame(OB_FRAME_COLOR);

            std::vector<std::shared_ptr<const ob::Frame>> frames;
            if(colorFrame) frames.push_back(colorFrame);

            if(depthFrame) {
                cv::Mat depth3d = ob_smpl::renderDepth3D(depthFrame, colormapId);
                if(!depth3d.empty()) {
                    cv::imshow("Depth 3D", depth3d);
                }
            }
            // Still push color (and other frames) to the CVWindow
            if(!frames.empty()) {
                win.pushFramesToView(frames);
            }
            else {
                win.pushFramesToView(frameSet);
            }
        }
        else {
            cv::destroyWindow("Depth 3D");
            // Default 2D mode: push all frames to the window
            win.pushFramesToView(frameSet);
        }
    }

    // Stop the Pipeline, no frame data will be generated
    pipe.stop();

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}

