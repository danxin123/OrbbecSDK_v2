// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

int main() try {
    // Create a pipeline.
    std::shared_ptr<ob::Pipeline> pipeline = std::make_shared<ob::Pipeline>();

    // Create a config and enable the depth and color streams.
    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    config->enableStream(OB_STREAM_DEPTH);
    // Set the frame aggregate output mode to all type frame require.
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    uint32_t frameIndex = 0;

    // Start the pipeline.
    pipeline->start(config);

    // Drop several frames
    for(int i = 0; i < 15; ++i) {
        auto lost = pipeline->waitForFrameset(100);
    }

    while(true) {
        // Wait for frameSet from the pipeline.
        std::shared_ptr<ob::FrameSet> frameSet = pipeline->waitForFrameset(100);

        if(!frameSet) {
            std::cout << "No frames received in 100ms..." << std::endl;
            continue;
        }

        if(++frameIndex >= 5) {
            std::cout << "The demo is over, please press ESC to exit manually!" << std::endl;
            break;
        }

        // Get the depth and color frames.
        auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH)->as<ob::DepthFrame>();
        auto colorFrame = frameSet->getFrame(OB_FRAME_COLOR)->as<ob::ColorFrame>();

        // Save depth frame
        std::string depthName = "Depth_" + std::to_string(depthFrame->width()) + "x" + std::to_string(depthFrame->height()) + "_" + std::to_string(frameIndex)
                                + "_" + std::to_string(depthFrame->timeStamp()) + "ms";
        auto savedDepth = ob_smpl::saveFrame(depthFrame, depthName);
        std::cout << "Depth saved:" << savedDepth << std::endl;

        // Save color frame
        std::string colorName = "Color_" + std::to_string(colorFrame->width()) + "x" + std::to_string(colorFrame->height()) + "_" + std::to_string(frameIndex)
                                + "_" + std::to_string(colorFrame->timeStamp()) + "ms";
        auto savedColor = ob_smpl::saveFrame(colorFrame, colorName);
        std::cout << "Color saved:" << savedColor << std::endl;
    }

    // Stop the Pipeline, no frame data will be generated
    pipeline->stop();

    std::cout << "Press any key to exit." << std::endl;
    ob_smpl::waitForKeyPressed();

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
