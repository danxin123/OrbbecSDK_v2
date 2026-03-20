// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

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

}  // namespace

int main() {
    try {
        if(tc_cpp_01_01() != 0) return 1;
        if(tc_cpp_01_03() != 0) return 1;
        if(tc_cpp_01_04() != 0) return 1;
        if(tc_cpp_02_02() != 0) return 1;

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
