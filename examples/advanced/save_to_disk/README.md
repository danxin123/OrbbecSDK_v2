# Save to Disk

## Overview

This sample demonstrates how to configure a pipeline to capture color and depth frames and save the first 5 valid frames to disk as PNG files using the SDK's built-in `FrameSaveHelper`. The program discards initial frames to ensure stable stream capture. No OpenCV dependency is required — format conversion is handled internally by the SDK.

### Knowledge

Pipeline is a pipeline for processing data streams, providing multi-channel stream configuration, switching, frame aggregation, and frame synchronization functions.

Frameset is a combination of different types of Frames.

Metadata is used to describe the various properties and states of a frame.

## Code overview

1. Pipeline Configuration and Initialization.

    ```cpp
    // Create a pipeline.
    std::shared_ptr<ob::Pipeline> pipeline = std::make_shared<ob::Pipeline>();

    // Create a config and enable the depth and color streams.
    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    config->enableStream(OB_STREAM_DEPTH);
    // Set the frame aggregate output mode to all type frame require.
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);
    ```

2. Get frameSet from pipeline.

    ```cpp
    // Wait for frameSet from the pipeline, the default timeout is 1000ms.
    auto frameSet   = pipe.waitForFrameset();
    ```

3. Save frames to disk as PNG using `ob_smpl::saveFrame`.

    ```cpp
    // Get the depth and color frames.
    auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH)->as<ob::DepthFrame>();
    auto colorFrame = frameSet->getFrame(OB_FRAME_COLOR)->as<ob::ColorFrame>();

    // Save depth frame as PNG (16-bit grayscale, lossless)
    std::string depthName = "Depth_" + std::to_string(depthFrame->width()) + "x"
                            + std::to_string(depthFrame->height()) + "_" + std::to_string(frameIndex)
                            + "_" + std::to_string(depthFrame->timeStamp()) + "ms";
    auto savedDepth = ob_smpl::saveFrame(depthFrame, depthName);

    // Save color frame as PNG (8-bit RGB, format conversion handled internally)
    std::string colorName = "Color_" + std::to_string(colorFrame->width()) + "x"
                            + std::to_string(colorFrame->height()) + "_" + std::to_string(frameIndex)
                            + "_" + std::to_string(colorFrame->timeStamp()) + "ms";
    auto savedColor = ob_smpl::saveFrame(colorFrame, colorName);
    ```

## Run Sample

Press any key in the window to exit the program.
