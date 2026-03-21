// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file test_common.hpp
/// Shared GoogleTest infrastructure for OrbbecSDK V2 tests.
/// Provides TestEnvironment (env-var driven), skip helpers, and fixture base classes.

#pragma once

#include <libobsensor/ObSensor.hpp>
#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>

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
    bool allowDestructive() const { return allowDestructive_; }

    void skipIfNoHardware() const {
        if(!hwAvailable_) {
            GTEST_SKIP() << "HARDWARE_AVAILABLE is not 'true' — skipping hardware test";
        }
    }

    void skipIfNoPlaybackBag() const {
        if(bagPath_.empty()) {
            GTEST_SKIP() << "PLAYBACK_BAG_PATH not set — skipping playback test";
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
        hwAvailable_     = getEnv("HARDWARE_AVAILABLE") == "true";
        deviceType_      = getEnv("DEVICE_TYPE", "gemini-335");
        deviceSerial_    = getEnv("DEVICE_SERIAL");
        bagPath_         = getEnv("PLAYBACK_BAG_PATH");
        allowDestructive_= getEnv("ALLOW_DESTRUCTIVE_TESTS") == "true";

        const std::string countStr = getEnv("DEVICE_COUNT", "1");
        try { deviceCount_ = std::stoi(countStr); } catch(...) { deviceCount_ = 1; }
    }

    static std::string getEnv(const char *name, const char *fallback = "") {
        const char *val = std::getenv(name);
        return val ? std::string(val) : std::string(fallback);
    }

    bool        hwAvailable_;
    std::string deviceType_;
    std::string deviceSerial_;
    int         deviceCount_;
    std::string bagPath_;
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
        env_.skipIfNoHardware();
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
