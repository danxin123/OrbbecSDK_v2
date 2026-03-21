// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <chrono>
#include <iostream>
#include <memory>

int main() {
    try {
        auto context = std::make_shared<ob::Context>();
        if(context == nullptr) {
            std::cerr << "TC_HW_CPP_P0_001 failed: context is null" << std::endl;
            return 1;
        }

        auto devList = context->queryDeviceList();
        if(devList == nullptr || devList->deviceCount() == 0) {
            std::cerr << "TC_HW_CPP_P0_001 failed: no connected device" << std::endl;
            return 1;
        }

        auto device = devList->getDevice(0);
        if(device == nullptr) {
            std::cerr << "TC_HW_CPP_P0_001 failed: getDevice(0) returned null" << std::endl;
            return 1;
        }

        auto pipeline = std::make_shared<ob::Pipeline>(device);
        auto config   = std::make_shared<ob::Config>();

        config->enableStream(OB_STREAM_DEPTH);
        pipeline->start(config);

        int validFrameCount = 0;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while(std::chrono::steady_clock::now() < deadline && validFrameCount < 3) {
            auto frameSet = pipeline->waitForFrames(3000);
            if(frameSet == nullptr) {
                continue;
            }

            auto depth = frameSet->depthFrame();
            if(depth != nullptr && depth->data() != nullptr && depth->dataSize() > 0) {
                ++validFrameCount;
            }
        }

        pipeline->stop();

        if(validFrameCount <= 0) {
            std::cerr << "TC_HW_CPP_P0_001 failed: no valid depth frame received" << std::endl;
            return 1;
        }

        std::cout << "TC_HW_CPP_P0_001 passed" << std::endl;
        return 0;
    }
    catch(const ob::Error &e) {
        std::cerr << "Function: " << e.getFunction() << "\n"
                  << "Args: " << e.getArgs() << "\n"
                  << "Message: " << e.what() << "\n"
                  << "Type: " << e.getExceptionType() << std::endl;
        return 1;
    }
    catch(const std::exception &e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return 1;
    }
}
