// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Depth Viewer — View depth stream with optional save-to-disk.
// Demonstrates: Pipeline depth config, CVWindow visualization, frame saving.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

int main(void) try {
    // Create a pipeline with default device.
    ob::Pipeline pipe;

    // Enable the depth stream.
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH);

    // Start the pipeline.
    pipe.start(config);

    // Create a window for rendering.
    ob_smpl::CVWindow win("Depth Viewer", 1280, 720, ob_smpl::ARRANGE_SINGLE);

    while(win.run()) {
        auto frameSet = pipe.waitForFrameset(100);
        if(frameSet == nullptr) {
            continue;
        }

        auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH);
        if(!depthFrame) {
            continue;
        }

        // Print center distance every 30 frames
        auto df = depthFrame->as<ob::DepthFrame>();
        if(df->getIndex() % 30 == 0 && (df->getFormat() == OB_FORMAT_Y16 || df->getFormat() == OB_FORMAT_Z16)) {
            uint32_t        width          = df->getWidth();
            uint32_t        height         = df->getHeight();
            float           scale          = df->getValueScale();
            const uint16_t *data           = reinterpret_cast<const uint16_t *>(df->getData());
            float           centerDistance = data[width * height / 2 + width / 2] * scale;
            std::cout << "Center distance: " << centerDistance << " mm\n";
        }

        win.pushFramesToView(depthFrame);
    }

    pipe.stop();
    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
