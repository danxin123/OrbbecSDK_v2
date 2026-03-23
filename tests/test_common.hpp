// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file test_common.hpp
/// Shared GoogleTest infrastructure for OrbbecSDK V2 tests.
/// Provides TestEnvironment (env-var driven), skip helpers, and fixture base classes.

#pragma once

#include <libobsensor/ObSensor.hpp>
#include <gtest/gtest.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dirent.h>
#endif

// ---------------------------------------------------------------------------
// TestEnvironment — singleton that reads env vars once
// ---------------------------------------------------------------------------
class TestEnvironment {
public:
    static TestEnvironment &instance() {
        static TestEnvironment env;
        return env;
    }

    bool hardwareAvailable() const { return hwAvailable_; }
    const std::string &deviceType() const { return deviceType_; }
    const std::string &deviceSerial() const { return deviceSerial_; }
    int  deviceCount() const { return deviceCount_; }
    const std::string &playbackBagPath() const { return bagPath_; }
    const std::string &firmwarePath() const { return firmwarePath_; }
    const std::string &depthPresetPath() const { return depthPresetPath_; }
    bool allowDestructive() const { return allowDestructive_; }

    void skipIfNoHardware() const {
        if(!hwAvailable_) {
            GTEST_SKIP() << "HARDWARE_AVAILABLE is not 'true' — skipping hardware test";
        }
    }

    void skipIfNoPlaybackBag() const {
        if(bagPath_.empty()) {
            GTEST_SKIP() << "PLAYBACK_BAG_PATH not set and no local .bag found — skipping playback test";
        }
    }

    void skipIfNoFirmware() const {
        if(firmwarePath_.empty()) {
            GTEST_SKIP() << "No firmware file found — set FIRMWARE_FILE_PATH or place a .bin in tests/resource/firmware/";
        }
    }

    void skipIfNoDepthPreset() const {
        if(depthPresetPath_.empty()) {
            GTEST_SKIP() << "No depth preset file found — set DEPTH_PRESET_PATH or place a .bin in tests/resource/present/";
        }
    }

    void skipIfNot335le() const {
        skipIfNoHardware();
        if(deviceType_ != "gemini-335le") {
            GTEST_SKIP() << "Test requires Gemini 335Le, current: " << deviceType_;
        }
    }

    void skipIfNot335lg() const {
        skipIfNoHardware();
        if(deviceType_ != "gemini-335lg") {
            GTEST_SKIP() << "Test requires Gemini 335Lg, current: " << deviceType_;
        }
    }

    void skipIfMultiDeviceNotAvailable() const {
        skipIfNoHardware();
        if(deviceCount_ < 2) {
            GTEST_SKIP() << "Test requires >= 2 devices, available: " << deviceCount_;
        }
    }

    void skipUnlessDestructive() const {
        if(!allowDestructive_) {
            GTEST_SKIP() << "ALLOW_DESTRUCTIVE_TESTS not set — skipping destructive test";
        }
    }

private:
    TestEnvironment() {
        hwAvailable_     = true;
        deviceType_      = getEnv("DEVICE_TYPE", "gemini-335");
        deviceSerial_    = getEnv("DEVICE_SERIAL");

        bagPath_ = getEnv("PLAYBACK_BAG_PATH");
        if(bagPath_.empty()) {
            bagPath_ = findLocalPlaybackBag();
        }

        firmwarePath_ = getEnv("FIRMWARE_FILE_PATH");
        if(firmwarePath_.empty()) {
            firmwarePath_ = findLocalFirmware();
        }

        depthPresetPath_ = getEnv("DEPTH_PRESET_PATH");
        if(depthPresetPath_.empty()) {
            depthPresetPath_ = findLocalDepthPreset();
        }

        allowDestructive_ = getEnv("ALLOW_DESTRUCTIVE_TESTS") == "true";

        const std::string countStr = getEnv("DEVICE_COUNT", "1");
        try { deviceCount_ = std::stoi(countStr); } catch(...) { deviceCount_ = 1; }
    }

    static std::string getEnv(const char *name, const char *fallback = "") {
        const char *val = std::getenv(name);
        return val ? std::string(val) : std::string(fallback);
    }

    // ---------------------------------------------------------------------------
    // Generic file discovery helpers
    // ---------------------------------------------------------------------------

    // Case-insensitive extension comparison (e.g. ext = ".bin" or ".bag").
    static bool endsWithExt(const char *name, const char *ext) {
        if(name == nullptr || ext == nullptr) return false;
        const char *found = std::strrchr(name, '.');
        if(found == nullptr) return false;
#ifdef _WIN32
        return _stricmp(found, ext) == 0;
#else
        return std::strcmp(found, ext) == 0;
#endif
    }

    // Return the full path of the first file with the given extension found in dir.
    static std::string findFirstFileWithExt(const std::string &dir, const std::string &ext) {
#ifdef _WIN32
        WIN32_FIND_DATAA findData;
        HANDLE           hFind = FindFirstFileA((dir + "\\*" + ext).c_str(), &findData);
        if(hFind == INVALID_HANDLE_VALUE) {
            return "";
        }

        std::string result;
        do {
            if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                result = dir + "\\" + findData.cFileName;
                break;
            }
        } while(FindNextFileA(hFind, &findData));

        FindClose(hFind);
        return result;
#else
        DIR *d = opendir(dir.c_str());
        if(d == nullptr) {
            return "";
        }

        std::string result;
        struct dirent *entry = nullptr;
        while((entry = readdir(d)) != nullptr) {
            if(entry->d_type == DT_DIR) {
                continue;
            }
            if(endsWithExt(entry->d_name, ext.c_str())) {
                result = dir + "/" + entry->d_name;
                break;
            }
        }
        closedir(d);
        return result;
#endif
    }

    static std::string findLocalPlaybackBag() {
        // Prefer the canonical resource layout; keep legacy paths as fallback.
        const std::vector<std::string> candidateDirs = {
            "tests/resource/rosbag",
            "../tests/resource/rosbag",
            "../../tests/resource/rosbag",
            "../../../tests/resource/rosbag",
            "../../../../tests/resource/rosbag",
            // legacy paths
            "tests/rosbag",
            "../tests/rosbag",
            "../../tests/rosbag",
            "../../../tests/rosbag",
            "../../../../tests/rosbag",
        };

        for(const auto &dir : candidateDirs) {
            auto bag = findFirstFileWithExt(dir, ".bag");
            if(!bag.empty()) {
                return bag;
            }
        }

        return "";
    }

    static std::string findLocalFirmware() {
        const std::vector<std::string> candidateDirs = {
            "tests/resource/firmware",
            "../tests/resource/firmware",
            "../../tests/resource/firmware",
            "../../../tests/resource/firmware",
            "../../../../tests/resource/firmware",
        };

        for(const auto &dir : candidateDirs) {
            auto f = findFirstFileWithExt(dir, ".bin");
            if(!f.empty()) {
                return f;
            }
        }

        return "";
    }

    static std::string findLocalDepthPreset() {
        const std::vector<std::string> candidateDirs = {
            "tests/resource/present",
            "../tests/resource/present",
            "../../tests/resource/present",
            "../../../tests/resource/present",
            "../../../../tests/resource/present",
        };

        for(const auto &dir : candidateDirs) {
            auto f = findFirstFileWithExt(dir, ".bin");
            if(!f.empty()) {
                return f;
            }
        }

        return "";
    }

    bool        hwAvailable_;
    std::string deviceType_;
    std::string deviceSerial_;
    int         deviceCount_;
    std::string bagPath_;
    std::string firmwarePath_;
    std::string depthPresetPath_;
    bool        allowDestructive_;
};

// ---------------------------------------------------------------------------
// Convenience macro
// ---------------------------------------------------------------------------
#define ENV() TestEnvironment::instance()

// ---------------------------------------------------------------------------
// Fixture: SDKTestBase — lightweight base, no device needed
// ---------------------------------------------------------------------------
class SDKTestBase : public ::testing::Test {
protected:
    const TestEnvironment &env_ = TestEnvironment::instance();
};

// ---------------------------------------------------------------------------
// Fixture: HardwareTest — auto-skips when no hardware
// ---------------------------------------------------------------------------
class HardwareTest : public SDKTestBase {
protected:
    void SetUp() override {
        if(!env_.hardwareAvailable()) {
            GTEST_SKIP() << "HARDWARE_AVAILABLE is not 'true' — skipping hardware test";
        }
    }
};

// ---------------------------------------------------------------------------
// Fixture: ContextTest — creates ob::Context in SetUp
// ---------------------------------------------------------------------------
class ContextTest : public SDKTestBase {
protected:
    std::shared_ptr<ob::Context> ctx_;

    void SetUp() override {
        ctx_ = std::make_shared<ob::Context>();
        ASSERT_NE(ctx_, nullptr);
    }

    void TearDown() override {
        ctx_.reset();
    }
};

// ---------------------------------------------------------------------------
// Fixture: DeviceTest — creates Context + first device
// ---------------------------------------------------------------------------
class DeviceTest : public HardwareTest {
protected:
    std::shared_ptr<ob::Context>    ctx_;
    std::shared_ptr<ob::Device>     device_;
    std::shared_ptr<ob::DeviceInfo> devInfo_;

    void SetUp() override {
        HardwareTest::SetUp();
        ctx_ = std::make_shared<ob::Context>();
        ASSERT_NE(ctx_, nullptr);

        auto devList = ctx_->queryDeviceList();
        ASSERT_NE(devList, nullptr);
        ASSERT_GT(devList->deviceCount(), 0u) << "No connected device";

        device_ = devList->getDevice(0);
        ASSERT_NE(device_, nullptr);

        devInfo_ = device_->getDeviceInfo();
        ASSERT_NE(devInfo_, nullptr);
    }

    void TearDown() override {
        devInfo_.reset();
        device_.reset();
        ctx_.reset();
    }
};
