// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr << "Usage: ob_playback_smoke_test <path-to-bag>" << std::endl;
        return 2;
    }

    const std::string bagPath = argv[1];

    try {
        auto playback = std::make_shared<ob::PlaybackDevice>(bagPath);
        auto pipeline = std::make_shared<ob::Pipeline>(playback);
        auto config   = std::make_shared<ob::Config>();

        auto sensorList = playback->getSensorList();
        if(sensorList->getCount() == 0) {
            std::cerr << "No sensors found in playback file: " << bagPath << std::endl;
            return 1;
        }

        for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
            config->enableStream(sensorList->getSensorType(i));
        }
        config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);

        std::atomic<int> frameSetCount(0);
        std::shared_ptr<ob::DepthFrame> firstDepthFrame;
        std::vector<uint64_t> depthTimestamps;

        pipeline->start(config);

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while(std::chrono::steady_clock::now() < deadline && frameSetCount.load(std::memory_order_relaxed) < 30) {
            auto frameSet = pipeline->waitForFrames(3000);
            if(frameSet == nullptr) {
                continue;
            }

            frameSetCount.fetch_add(1, std::memory_order_relaxed);
            auto depthFrame = frameSet->depthFrame();
            if(depthFrame != nullptr) {
                if(firstDepthFrame == nullptr) {
                    firstDepthFrame = depthFrame;
                }
                depthTimestamps.push_back(depthFrame->timeStampUs());
            }
        }

        pipeline->stop();

        const int received = frameSetCount.load(std::memory_order_relaxed);
        std::cout << "playback frame sets received: " << received << std::endl;

        if(received <= 0) {
            std::cerr << "Playback smoke test failed: no frame set received." << std::endl;
            return 1;
        }

        if(firstDepthFrame == nullptr) {
            std::cerr << "Playback smoke test failed: no depth frame received." << std::endl;
            return 1;
        }

        // TC_CPP_10_01 subset: basic frame property integrity.
        if(firstDepthFrame->data() == nullptr || firstDepthFrame->dataSize() == 0) {
            std::cerr << "Playback smoke test failed: invalid depth frame basic properties." << std::endl;
            return 1;
        }

        if(firstDepthFrame->timeStampUs() == 0 || firstDepthFrame->systemTimeStampUs() == 0) {
            std::cerr << "Playback smoke test failed: invalid frame timestamps." << std::endl;
            return 1;
        }

        // TC_CPP_10_05 subset: video frame dimensions.
        if(firstDepthFrame->width() == 0 || firstDepthFrame->height() == 0) {
            std::cerr << "Playback smoke test failed: invalid depth frame width/height." << std::endl;
            return 1;
        }

        // TC_CPP_10_02 subset: timestamp monotonicity.
        if(depthTimestamps.size() < 2) {
            std::cerr << "Playback smoke test failed: insufficient depth frame samples for monotonicity check." << std::endl;
            return 1;
        }

        for(size_t i = 1; i < depthTimestamps.size(); ++i) {
            if(depthTimestamps[i] < depthTimestamps[i - 1]) {
                std::cerr << "Playback smoke test failed: depth timestamp is not monotonic." << std::endl;
                return 1;
            }
        }

        return 0;
    }
    catch(const ob::Error &e) {
        std::cerr << "Function: " << e.getFunction() << "\n"
                  << "Args: " << e.getArgs() << "\n"
                  << "Message: " << e.what() << "\n"
                  << "Type: " << e.getExceptionType() << std::endl;
        return 1;
    }
}
