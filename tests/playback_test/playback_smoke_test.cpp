// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

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
        pipeline->start(config, [&](std::shared_ptr<ob::FrameSet> frameSet) {
            if(frameSet != nullptr) {
                frameSetCount.fetch_add(1, std::memory_order_relaxed);
            }
        });

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while(std::chrono::steady_clock::now() < deadline) {
            if(frameSetCount.load(std::memory_order_relaxed) >= 5) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        pipeline->stop();

        const int received = frameSetCount.load(std::memory_order_relaxed);
        std::cout << "playback frame sets received: " << received << std::endl;

        if(received <= 0) {
            std::cerr << "Playback smoke test failed: no frame set received." << std::endl;
            return 1;
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
