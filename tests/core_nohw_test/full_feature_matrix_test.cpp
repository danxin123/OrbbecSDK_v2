// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace {

int check_context_and_discovery() {
    auto context = std::make_shared<ob::Context>();
    if(context == nullptr || context->queryDeviceList() == nullptr) {
        std::cerr << "[FULL] context/discovery failed" << std::endl;
        return 1;
    }

    context->enableNetDeviceEnumeration(true);
    context->enableNetDeviceEnumeration(false);
    context->freeIdleMemory();

    if(context->queryDeviceList() == nullptr) {
        std::cerr << "[FULL] context unusable after net/memory operations" << std::endl;
        return 1;
    }

    return 0;
}

int check_config_and_version() {
    auto config = std::make_shared<ob::Config>();
    if(config == nullptr) {
        std::cerr << "[FULL] config create failed" << std::endl;
        return 1;
    }

    config->enableStream(OB_STREAM_DEPTH);
    config->enableVideoStream(OB_STREAM_DEPTH, 640, 480, 30, OB_FORMAT_Y16);
    config->enableAccelStream();
    config->enableGyroStream();
    config->setAlignMode(ALIGN_D2C_SW_MODE);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    config->setDepthScaleRequire(false);

    if(config->getEnabledStreamProfileList() == nullptr) {
        std::cerr << "[FULL] enabled stream profile list is null" << std::endl;
        return 1;
    }

    const int major = ob::Version::getMajor();
    const int minor = ob::Version::getMinor();
    const int patch = ob::Version::getPatch();
    const int full  = ob::Version::getVersion();
    if(major < 0 || minor < 0 || patch < 0 || full != major * 10000 + minor * 100 + patch) {
        std::cerr << "[FULL] version check failed" << std::endl;
        return 1;
    }

    return 0;
}

int check_logging_and_errors() {
    std::atomic<int> callbackCount(0);
    const std::string marker = "TC_CPP_FULL_FEATURE_LOG_MARKER";

    ob::Context::setLoggerToCallback(OB_LOG_SEVERITY_DEBUG, [&callbackCount](OBLogSeverity, const char *) {
        callbackCount.fetch_add(1, std::memory_order_relaxed);
    });

    ob::Context::logExternalMessage(OB_LOG_SEVERITY_INFO, "full_feature_matrix_test", marker, __FILE__, __func__, __LINE__);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if(callbackCount.load(std::memory_order_relaxed) <= 0) {
        std::cerr << "[FULL] logger callback not triggered" << std::endl;
        return 1;
    }

    bool playbackErrorCaught = false;
    try {
        auto invalidPlayback = std::make_shared<ob::PlaybackDevice>("__invalid_path__.bag");
        (void)invalidPlayback;
    }
    catch(const ob::Error &) {
        playbackErrorCaught = true;
    }

    if(!playbackErrorCaught) {
        std::cerr << "[FULL] invalid playback path should throw" << std::endl;
        return 1;
    }

    bool invalidFilterCaught = false;
    try {
        auto f = ob::FilterFactory::createFilter("__invalid_filter_name__");
        (void)f;
    }
    catch(const ob::Error &) {
        invalidFilterCaught = true;
    }

    if(!invalidFilterCaught) {
        std::cerr << "[FULL] invalid filter should throw" << std::endl;
        return 1;
    }

    return 0;
}

int check_data_structures() {
    OBPoint3f p{ 1.0f, 2.0f, 3.0f };
    OBAccelValue accel{ 0.1f, 0.2f, 0.3f };
    OBGyroValue gyro{ 1.1f, 1.2f, 1.3f };
    OBIntPropertyRange intRange{ 0, 100, 0, 1, 30 };

    if(p.z <= 0 || accel.x <= 0 || gyro.z <= 0 || intRange.cur < intRange.min || intRange.cur > intRange.max) {
        std::cerr << "[FULL] data structure values are invalid" << std::endl;
        return 1;
    }

    return 0;
}

int check_optional_playback_path(const std::string &bagPath) {
    if(bagPath.empty()) {
        std::cout << "[FULL] playback branch skipped: no bag path provided" << std::endl;
        return 0;
    }

    auto playback = std::make_shared<ob::PlaybackDevice>(bagPath);
    auto pipeline = std::make_shared<ob::Pipeline>(playback);
    auto config   = std::make_shared<ob::Config>();

    auto sensorList = playback->getSensorList();
    if(sensorList == nullptr || sensorList->getCount() == 0) {
        std::cerr << "[FULL] playback has no sensors" << std::endl;
        return 1;
    }

    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        config->enableStream(sensorList->getSensorType(i));
    }

    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline->start(config);

    std::shared_ptr<ob::DepthFrame> firstDepth;
    for(int i = 0; i < 10; ++i) {
        auto fs = pipeline->waitForFrames(3000);
        if(fs == nullptr) {
            continue;
        }
        auto depth = fs->depthFrame();
        if(depth != nullptr) {
            firstDepth = depth;
            break;
        }
    }

    pipeline->stop();

    if(firstDepth == nullptr || firstDepth->data() == nullptr || firstDepth->dataSize() == 0) {
        std::cerr << "[FULL] no valid playback depth frame" << std::endl;
        return 1;
    }

    auto cloned = ob::FrameFactory::createFrameFromOtherFrame(firstDepth, true);
    if(cloned == nullptr || cloned->dataSize() != firstDepth->dataSize()) {
        std::cerr << "[FULL] frame clone failed" << std::endl;
        return 1;
    }

    return 0;
}

}  // namespace

int main(int argc, char **argv) {
    try {
        std::string bagPath;
        if(argc > 1) {
            bagPath = argv[1];
        }

        if(check_context_and_discovery() != 0) return 1;
        if(check_config_and_version() != 0) return 1;
        if(check_logging_and_errors() != 0) return 1;
        if(check_data_structures() != 0) return 1;
        if(check_optional_playback_path(bagPath) != 0) return 1;

        std::cout << "TC_CPP_FULL_001_full_feature_matrix passed" << std::endl;
        return 0;
    }
    catch(const ob::Error &e) {
        std::cerr << "[FULL] ob::Error caught: " << (e.what() ? e.what() : "") << std::endl;
        return 1;
    }
    catch(const std::exception &e) {
        std::cerr << "[FULL] std::exception caught: " << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "[FULL] unknown exception" << std::endl;
        return 1;
    }
}
