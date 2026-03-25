// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#pragma once

#include "../test_common.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace hw_test_main {

static const char *kDestructiveTestFilter =
    "TC_CPP_14_Property_HW.TC_CPP_14_07_customer_data:"
    "TC_CPP_21_Firmware.TC_CPP_21_08_firmware_update:"
    "TC_CPP_21_Firmware.TC_CPP_21_09_update_depth_presets";

inline void setDefaultFilterIfUnset(const std::string &filter) {
    auto &currentFilter = ::testing::GTEST_FLAG(filter);
    if(currentFilter.empty() || currentFilter == "*") {
        currentFilter = filter;
    }
}

inline void enableDestructiveMode() {
    // The dedicated destructive binary should execute its test set directly
    // without requiring callers to export ALLOW_DESTRUCTIVE_TESTS first.
    ENV().setAllowDestructive(true);
}

class FirmwareUpgradeEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        if(!ENV().allowDestructive() || ENV().firmwarePath().empty()) {
            return;
        }

        const std::string &fwPath = ENV().firmwarePath();
        std::cout << "\n[FW-UPGRADE] ===== Pre-test firmware upgrade =====\n"
                  << "[FW-UPGRADE] File: " << fwPath << "\n";

        auto ctx = std::make_shared<ob::Context>();
        std::shared_ptr<ob::DeviceList> devList;
        try {
            devList = ctx->queryDeviceList();
        }
        catch(const ob::Error &e) {
            std::cout << "[FW-UPGRADE] queryDeviceList() failed: " << e.getMessage() << " — skipping upgrade.\n";
            return;
        }

        if(!devList || devList->deviceCount() == 0) {
            std::cout << "[FW-UPGRADE] No device found — skipping upgrade.\n";
            return;
        }

        auto device = devList->getDevice(0);
        auto info   = device->getDeviceInfo();
        const std::string sn   = info->getSerialNumber() ? info->getSerialNumber() : "";
        const std::string name = info->getName() ? info->getName() : "?";
        std::cout << "[FW-UPGRADE] Device: " << name << "  SN=" << sn << "\n";

        std::atomic<OBFwUpdateState> lastState{ STAT_START };
        std::atomic<uint8_t>         lastPercent{ 0 };
        std::mutex                   mutex;
        std::condition_variable      cv;
        bool                         done = false;

        ob::Device::DeviceFwUpdateCallback callback = [&](OBFwUpdateState state, const char *msg, uint8_t percent) {
            lastState   = state;
            lastPercent = percent;
            std::cout << "[FW-UPGRADE] " << static_cast<int>(percent) << "% " << (msg ? msg : "") << "\n" << std::flush;
            if(state == STAT_DONE || state < 0) {
                std::lock_guard<std::mutex> lock(mutex);
                done = true;
                cv.notify_all();
            }
        };

        try {
            device->updateFirmware(fwPath.c_str(), callback, true);
        }
        catch(const ob::Error &e) {
            std::cout << "[FW-UPGRADE] Failed to start update: " << e.getMessage() << "\n";
            return;
        }

        {
            std::unique_lock<std::mutex> lock(mutex);
            if(!cv.wait_for(lock, std::chrono::minutes(5), [&] { return done; })) {
                std::cout << "[FW-UPGRADE] TIMEOUT — upgrade did not complete within 5 minutes.\n";
                return;
            }
        }

        OBFwUpdateState finalState = lastState.load();
        if(finalState < 0) {
            std::cout << "[FW-UPGRADE] FAILED with error state " << static_cast<int>(finalState) << "\n";
            return;
        }

        std::cout << "[FW-UPGRADE] Upgrade completed (state=" << static_cast<int>(finalState)
                  << "). Triggering automatic reboot...\n";

        auto reconnectWaiter = beginWaitForReconnect(ctx);

        try {
            device->reboot();
        }
        catch(const ob::Error &e) {
            ctx->unregisterDeviceChangedCallback(reconnectWaiter.callbackId);
            std::cout << "[FW-UPGRADE] Failed to trigger reboot after firmware upgrade: " << e.getMessage() << "\n";
            return;
        }

        device.reset();
        devList.reset();

        if(!sn.empty()) {
            waitForReconnect(ctx, reconnectWaiter);
        }
        else {
            ctx->unregisterDeviceChangedCallback(reconnectWaiter.callbackId);
        }

        std::cout << "[FW-UPGRADE] ===== Firmware upgrade done =====\n\n";
    }

private:
    struct ReconnectWaiter {
        OBCallbackId             callbackId = INVALID_CALLBACK_ID;
        std::shared_ptr<std::atomic<bool>> reconnected;
    };

    static ReconnectWaiter beginWaitForReconnect(const std::shared_ptr<ob::Context> &ctx) {
        ReconnectWaiter waiter;
        waiter.reconnected = std::make_shared<std::atomic<bool>>(false);
        auto reconnected   = waiter.reconnected;
        waiter.callbackId  = ctx->registerDeviceChangedCallback(
            [reconnected](std::shared_ptr<ob::DeviceList> /*removed*/, std::shared_ptr<ob::DeviceList> added) {
                if(added && added->getCount() > 0) {
                    reconnected->store(true);
                }
            });
        return waiter;
    }

    static void waitForReconnect(const std::shared_ptr<ob::Context> &ctx, const ReconnectWaiter &waiter) {
        constexpr int kMaxWaitMs = 60000;
        constexpr int kPollMs    = 500;
        for(int elapsed = 0; !waiter.reconnected->load() && elapsed < kMaxWaitMs; elapsed += kPollMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(kPollMs));
        }
        ctx->unregisterDeviceChangedCallback(waiter.callbackId);

        if(waiter.reconnected->load()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "[FW-UPGRADE] Device reconnected and ready.\n";
        }
        else {
            std::cout << "[FW-UPGRADE] Device did not reconnect within 60 s.\n";
        }
    }
};

}  // namespace hw_test_main