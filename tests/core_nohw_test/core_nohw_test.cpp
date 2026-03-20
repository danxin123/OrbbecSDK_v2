// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <chrono>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

namespace {

uint64_t getProcessMemoryUsageBytes() {
#if defined(__linux__)
    std::ifstream statusFile("/proc/self/status");
    std::string line;
    while(std::getline(statusFile, line)) {
        if(line.rfind("VmRSS:", 0) == 0) {
            std::string key;
            uint64_t valueKb = 0;
            std::string unit;
            std::istringstream iss(line);
            iss >> key >> valueKb >> unit;
            return valueKb * 1024;
        }
    }
#endif
    return 0;
}

int tc_cpp_01_01() {
    auto context = std::make_shared<ob::Context>();
    if(context == nullptr) {
        std::cerr << "TC_CPP_01_01 failed: Context construction returned null" << std::endl;
        return 1;
    }
    auto devList = context->queryDeviceList();
    if(devList == nullptr) {
        std::cerr << "TC_CPP_01_01 failed: queryDeviceList returned null" << std::endl;
        return 1;
    }
    std::cout << "TC_CPP_01_01 passed" << std::endl;
    return 0;
}

int tc_cpp_01_03() {
    const int loops = 10;
    const uint64_t before = getProcessMemoryUsageBytes();

    for(int i = 0; i < loops; ++i) {
        auto context = std::make_shared<ob::Context>();
        if(context == nullptr || context->queryDeviceList() == nullptr) {
            std::cerr << "TC_CPP_01_03 failed at iteration " << i << std::endl;
            return 1;
        }
    }

    const uint64_t after = getProcessMemoryUsageBytes();
    if(before > 0 && after > before) {
        const uint64_t delta = after - before;
        if(delta > 1ULL * 1024ULL * 1024ULL) {
            std::cerr << "TC_CPP_01_03 failed: memory delta too large: " << delta << std::endl;
            return 1;
        }
    }

    std::cout << "TC_CPP_01_03 passed" << std::endl;
    return 0;
}

int tc_cpp_01_04() {
    auto context = std::make_shared<ob::Context>();
    if(context == nullptr) {
        std::cerr << "TC_CPP_01_04 failed: Context construction returned null" << std::endl;
        return 1;
    }

    context->freeIdleMemory();
    context->freeIdleMemory();
    context->freeIdleMemory();

    auto devList = context->queryDeviceList();
    if(devList == nullptr) {
        std::cerr << "TC_CPP_01_04 failed: context unusable after freeIdleMemory" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_01_04 passed" << std::endl;
    return 0;
}

int tc_cpp_02_02() {
    auto context = std::make_shared<ob::Context>();
    if(context == nullptr) {
        std::cerr << "TC_CPP_02_02 failed: Context construction returned null" << std::endl;
        return 1;
    }

    context->enableNetDeviceEnumeration(true);
    context->enableNetDeviceEnumeration(false);
    context->enableNetDeviceEnumeration(true);

    auto devList = context->queryDeviceList();
    if(devList == nullptr) {
        std::cerr << "TC_CPP_02_02 failed: context unusable after network enumeration switch" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_02_02 passed" << std::endl;
    return 0;
}

int tc_cpp_09_nohw_config_api() {
    auto config = std::make_shared<ob::Config>();
    if(config == nullptr) {
        std::cerr << "TC_CPP_09_nohw failed: Config construction returned null" << std::endl;
        return 1;
    }

    config->enableStream(OB_STREAM_DEPTH);
    config->enableVideoStream(OB_STREAM_DEPTH, 640, 480, 30, OB_FORMAT_Y16);
    config->enableAccelStream();
    config->enableGyroStream();
    config->enableLiDARStream();
    config->setAlignMode(ALIGN_DISABLE);
    config->setAlignMode(ALIGN_D2C_HW_MODE);
    config->setAlignMode(ALIGN_D2C_SW_MODE);
    config->setDepthScaleRequire(true);
    config->setDepthScaleRequire(false);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    config->disableStream(OB_STREAM_DEPTH);
    config->disableAllStream();

    auto enabledProfiles = config->getEnabledStreamProfileList();
    if(enabledProfiles == nullptr) {
        std::cerr << "TC_CPP_09_nohw failed: getEnabledStreamProfileList returned null" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_09_nohw passed" << std::endl;
    return 0;
}

int tc_cpp_22_version() {
    const int major = ob::Version::getMajor();
    const int minor = ob::Version::getMinor();
    const int patch = ob::Version::getPatch();
    const int full  = ob::Version::getVersion();

    if(major < 0 || minor < 0 || patch < 0) {
        std::cerr << "TC_CPP_22 failed: invalid version components" << std::endl;
        return 1;
    }

    const int composed = major * 10000 + minor * 100 + patch;
    if(full != composed) {
        std::cerr << "TC_CPP_22 failed: full version mismatch with components" << std::endl;
        return 1;
    }

    const char *stage = ob::Version::getStageVersion();
    if(stage == nullptr) {
        std::cerr << "TC_CPP_22 failed: stage version pointer is null" << std::endl;
        return 1;
    }

    if(ob_get_version() != full || ob_get_major_version() != major || ob_get_minor_version() != minor || ob_get_patch_version() != patch
       || ob_get_stage_version() == nullptr) {
        std::cerr << "TC_CPP_22 failed: C API version values mismatch" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_22 passed" << std::endl;
    return 0;
}

int tc_cpp_23_logging() {
    std::atomic<int> callbackCount(0);
    std::string      lastMessage;

    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_DEBUG);
    ob::Context::setLoggerToConsole(OB_LOG_SEVERITY_DEBUG);
    ob::Context::setLoggerToCallback(OB_LOG_SEVERITY_DEBUG, [&callbackCount, &lastMessage](OBLogSeverity, const char *msg) {
        if(msg != nullptr) {
            lastMessage = msg;
        }
        callbackCount.fetch_add(1, std::memory_order_relaxed);
    });

    const std::string marker = "TC_CPP_23_external_message_marker";
    ob::Context::logExternalMessage(OB_LOG_SEVERITY_INFO, "core_nohw_test", marker, __FILE__, __func__, __LINE__);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(callbackCount.load(std::memory_order_relaxed) <= 0) {
        std::cerr << "TC_CPP_23 failed: logger callback was not triggered" << std::endl;
        return 1;
    }

    if(lastMessage.find(marker) == std::string::npos) {
        std::cerr << "TC_CPP_23 failed: callback message does not contain marker" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_23 passed" << std::endl;
    return 0;
}

int tc_cpp_24_error_handling() {
    bool caught = false;
    try {
        auto invalidPlayback = std::make_shared<ob::PlaybackDevice>("__definitely_not_existing__.bag");
        (void)invalidPlayback;
    }
    catch(const ob::Error &e) {
        caught = true;
        if(e.what() == nullptr || std::strlen(e.what()) == 0 || e.getFunction() == nullptr || std::strlen(e.getFunction()) == 0
           || e.getExceptionType() == OB_EXCEPTION_TYPE_UNKNOWN) {
            std::cerr << "TC_CPP_24 failed: incomplete error information" << std::endl;
            return 1;
        }
    }

    if(!caught) {
        std::cerr << "TC_CPP_24 failed: expected ob::Error was not thrown" << std::endl;
        return 1;
    }

    std::cout << "TC_CPP_24 passed" << std::endl;
    return 0;
}

int tc_cpp_25_data_structures() {
    OBDepthWorkMode depthWorkMode{};
    std::memset(depthWorkMode.checksum, 0xAB, sizeof(depthWorkMode.checksum));
    std::strncpy(depthWorkMode.name, "NOHW_MODE", sizeof(depthWorkMode.name) - 1);
    depthWorkMode.tag = OB_CUSTOM_DEPTH_WORK_MODE;
    if(depthWorkMode.name[0] == '\0' || depthWorkMode.tag != OB_CUSTOM_DEPTH_WORK_MODE) {
        std::cerr << "TC_CPP_25 failed: OBDepthWorkMode field read/write invalid" << std::endl;
        return 1;
    }

    OBNetIpConfig ip{};
    ip.dhcp       = 0;
    ip.address[0] = 192;
    ip.address[1] = 168;
    ip.address[2] = 1;
    ip.address[3] = 10;
    ip.mask[0]    = 255;
    ip.mask[1]    = 255;
    ip.mask[2]    = 255;
    ip.mask[3]    = 0;
    ip.gateway[0] = 192;
    ip.gateway[1] = 168;
    ip.gateway[2] = 1;
    ip.gateway[3] = 1;
    if(ip.address[0] > 255 || ip.mask[0] > 255 || ip.gateway[0] > 255) {
        std::cerr << "TC_CPP_25 failed: OBNetIpConfig value range invalid" << std::endl;
        return 1;
    }

    OBHdrConfig hdr{};
    hdr.enable     = 1;
    hdr.sequence_name = 1;
    hdr.exposure_1 = 100;
    hdr.gain_1     = 10;
    hdr.exposure_2 = 200;
    hdr.gain_2     = 20;

    OBRegionOfInterest roi{};
    roi.x0_left  = 0;
    roi.y0_top   = 0;
    roi.x1_right = 639;
    roi.y1_bottom = 479;
    if(roi.x1_right <= roi.x0_left || roi.y1_bottom <= roi.y0_top) {
        std::cerr << "TC_CPP_25 failed: OBRegionOfInterest rectangle invalid" << std::endl;
        return 1;
    }

    OBPoint3f point{ 1.0f, 2.0f, 3.0f };
    OBColorPoint colorPoint{ 1.0f, 2.0f, 3.0f, 10.0f, 20.0f, 30.0f };
    OBAccelValue accel{ 0.1f, 0.2f, 0.3f };
    OBGyroValue gyro{ 1.1f, 1.2f, 1.3f };
    if(point.z <= 0 || colorPoint.r <= 0 || accel.x <= 0 || gyro.z <= 0) {
        std::cerr << "TC_CPP_25 failed: point/imu structures invalid" << std::endl;
        return 1;
    }

    OBIntPropertyRange intRange{ 5, 10, 0, 1, 3 };
    OBFloatPropertyRange floatRange{ 0.5f, 1.0f, 0.0f, 0.1f, 0.2f };
    if(intRange.min > intRange.max || intRange.cur < intRange.min || intRange.cur > intRange.max || floatRange.min > floatRange.max
       || floatRange.cur < floatRange.min || floatRange.cur > floatRange.max) {
        std::cerr << "TC_CPP_25 failed: property range structures invalid" << std::endl;
        return 1;
    }

    OBFilterConfigSchemaItem schema{};
    schema.name = "alpha";
    schema.type = OB_FILTER_CONFIG_VALUE_TYPE_FLOAT;
    schema.min  = 0.0;
    schema.max  = 1.0;
    schema.step = 0.1;
    schema.def  = 0.5;
    schema.desc = "test schema";
    if(schema.name == nullptr || schema.desc == nullptr || schema.min > schema.max || schema.def < schema.min || schema.def > schema.max) {
        std::cerr << "TC_CPP_25 failed: filter schema structure invalid" << std::endl;
        return 1;
    }

    (void)hdr;

    std::cout << "TC_CPP_25 passed" << std::endl;
    return 0;
}

}  // namespace

int main() {
    try {
        if(tc_cpp_01_01() != 0) return 1;
        if(tc_cpp_01_03() != 0) return 1;
        if(tc_cpp_01_04() != 0) return 1;
        if(tc_cpp_02_02() != 0) return 1;
        if(tc_cpp_09_nohw_config_api() != 0) return 1;
        if(tc_cpp_22_version() != 0) return 1;
        if(tc_cpp_23_logging() != 0) return 1;
        if(tc_cpp_24_error_handling() != 0) return 1;
        if(tc_cpp_25_data_structures() != 0) return 1;

        std::cout << "All no-hardware core test cases passed" << std::endl;
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
