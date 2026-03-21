// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

struct CaseResult {
    std::string name;
    bool        passed;
    std::string message;
};

std::string xmlEscape(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for(char c : s) {
        switch(c) {
        case '&':
            out += "&amp;";
            break;
        case '<':
            out += "&lt;";
            break;
        case '>':
            out += "&gt;";
            break;
        case '"':
            out += "&quot;";
            break;
        case '\'':
            out += "&apos;";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

bool writeJunit(const std::string &path, const std::vector<CaseResult> &results) {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if(!out.is_open()) {
        return false;
    }

    int failures = 0;
    for(const auto &r : results) {
        if(!r.passed) {
            ++failures;
        }
    }

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<testsuites>\n";
    out << "  <testsuite name=\"rc_hw_cpp_p0\" tests=\"" << results.size() << "\" failures=\"" << failures
        << "\" errors=\"0\" skipped=\"0\">\n";

    for(const auto &r : results) {
        out << "    <testcase classname=\"rc_hw_cpp.p0\" name=\"" << xmlEscape(r.name) << "\">\n";
        if(!r.passed) {
            out << "      <failure message=\"" << xmlEscape(r.message) << "\">" << xmlEscape(r.message) << "</failure>\n";
        }
        out << "    </testcase>\n";
    }

    out << "  </testsuite>\n";
    out << "</testsuites>\n";
    return true;
}

CaseResult case_device_enumeration() {
    try {
        auto context = std::make_shared<ob::Context>();
        if(context == nullptr) {
            return { "TC_HW_CPP_P0_001_device_enumeration", false, "context is null" };
        }

        auto devList = context->queryDeviceList();
        if(devList == nullptr || devList->deviceCount() == 0) {
            return { "TC_HW_CPP_P0_001_device_enumeration", false, "no connected device" };
        }

        auto device = devList->getDevice(0);
        if(device == nullptr) {
            return { "TC_HW_CPP_P0_001_device_enumeration", false, "getDevice(0) returned null" };
        }

        return { "TC_HW_CPP_P0_001_device_enumeration", true, "ok" };
    }
    catch(const ob::Error &e) {
        return { "TC_HW_CPP_P0_001_device_enumeration", false, e.what() ? e.what() : "ob::Error" };
    }
}

CaseResult case_pipeline_depth_stream() {
    try {
        auto context = std::make_shared<ob::Context>();
        auto devList = context->queryDeviceList();
        if(devList == nullptr || devList->deviceCount() == 0) {
            return { "TC_HW_CPP_P0_002_pipeline_depth_stream", false, "no connected device" };
        }

        auto device = devList->getDevice(0);
        if(device == nullptr) {
            return { "TC_HW_CPP_P0_002_pipeline_depth_stream", false, "getDevice(0) returned null" };
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
            return { "TC_HW_CPP_P0_002_pipeline_depth_stream", false, "no valid depth frame received" };
        }

        return { "TC_HW_CPP_P0_002_pipeline_depth_stream", true, "ok" };
    }
    catch(const ob::Error &e) {
        return { "TC_HW_CPP_P0_002_pipeline_depth_stream", false, e.what() ? e.what() : "ob::Error" };
    }
}

CaseResult case_device_info() {
    try {
        auto context = std::make_shared<ob::Context>();
        auto devList = context->queryDeviceList();
        if(devList == nullptr || devList->deviceCount() == 0) {
            return { "TC_HW_CPP_P0_003_device_info", false, "no connected device" };
        }

        auto device = devList->getDevice(0);
        if(device == nullptr) {
            return { "TC_HW_CPP_P0_003_device_info", false, "getDevice(0) returned null" };
        }

        auto info = device->getDeviceInfo();
        if(info == nullptr) {
            return { "TC_HW_CPP_P0_003_device_info", false, "getDeviceInfo returned null" };
        }

        auto name = info->name();
        auto pid  = info->pid();
        if(name.empty() || pid == 0) {
            return { "TC_HW_CPP_P0_003_device_info", false, "device name empty or pid invalid" };
        }

        return { "TC_HW_CPP_P0_003_device_info", true, "ok" };
    }
    catch(const ob::Error &e) {
        return { "TC_HW_CPP_P0_003_device_info", false, e.what() ? e.what() : "ob::Error" };
    }
}

}  // namespace

int main(int argc, char **argv) {
    std::string junitOut;
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == "--junit-out" && i + 1 < argc) {
            junitOut = argv[++i];
        }
    }

    try {
        std::vector<CaseResult> results;
        results.push_back(case_device_enumeration());
        results.push_back(case_pipeline_depth_stream());
        results.push_back(case_device_info());

        bool allPassed = true;
        for(const auto &r : results) {
            if(r.passed) {
                std::cout << r.name << " passed" << std::endl;
            }
            else {
                allPassed = false;
                std::cerr << r.name << " failed: " << r.message << std::endl;
            }
        }

        if(!junitOut.empty() && !writeJunit(junitOut, results)) {
            std::cerr << "failed to write junit file: " << junitOut << std::endl;
            return 1;
        }

        return allPassed ? 0 : 1;
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
