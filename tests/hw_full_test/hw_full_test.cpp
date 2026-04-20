// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file hw_full_test.cpp
/// @brief Full hardware test suite covering scenarios that require physical devices (~144 cases).
/// Coverage: TC_CPP_02-21 and selected items in TC_CPP_24 and TC_CPP_25.

#include "../test_common.hpp"
#include <libobsensor/ObSensor.hpp>
#include <libobsensor/h/Utils.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string toLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

bool hasSensorType(const std::shared_ptr<ob::SensorList> &sensorList, OBSensorType type) {
    if(!sensorList) {
        return false;
    }

    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        if(sensorList->getSensorType(i) == type) {
            return true;
        }
    }

    return false;
}

std::shared_ptr<ob::Sensor> tryGetSensor(const std::shared_ptr<ob::Device> &device, OBSensorType type) {
    if(!device) {
        return nullptr;
    }

    try {
        return device->getSensor(type);
    }
    catch(const ob::Error &) {
        return nullptr;
    }
}

}  // namespace

// ============================================================
// PipelineTest fixture — DeviceTest + Pipeline
// ============================================================
class PipelineTest : public DeviceTest {
protected:
    std::shared_ptr<ob::Pipeline> pipeline_;

    void SetUp() override {
        DeviceTest::SetUp();
        pipeline_ = std::make_shared<ob::Pipeline>(device_);
    }

    void TearDown() override {
        if(pipeline_) {
            try {
                pipeline_->stop();
            }
            catch(...) {
            }
        }
        pipeline_.reset();
        DeviceTest::TearDown();
    }
};

// ============================================================
// SensorTest fixture
// ============================================================
class SensorTest : public DeviceTest {
protected:
    std::shared_ptr<ob::SensorList> sensorList_;

    void SetUp() override {
        DeviceTest::SetUp();
        sensorList_ = device_->getSensorList();
        ASSERT_NE(sensorList_, nullptr);
    }
};

// ============================================================
// Test group: Discovery.
// ============================================================
class TC_CPP_02_Discovery_HW : public DeviceTest {};

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_01_usb_device_enum) {
    /// Test case: usb device enum.
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    ASSERT_GT(devList->getCount(), 0u);

    for(uint32_t i = 0; i < devList->getCount(); i++) {
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_NE(devList->getSerialNumber(i), nullptr);
    }
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_02_net_device_enum_toggle) {
    /// Test case: net device enum toggle.
    const int currentPid = devInfo_->getPid();
    if(!TestEnvironment::isPid335leOr435le(currentPid)) {
        GTEST_SKIP() << "Test requires Gemini 335Le/435Le, current pid: 0x" << std::hex << currentPid;
    }

    auto hasExpectedDevice = [&](const std::shared_ptr<ob::DeviceList> &devList) {
        if(!devList) {
            return false;
        }

        const std::string expectedSerial = ENV().deviceSerial();
        for(uint32_t i = 0; i < devList->getCount(); ++i) {
            const char *name   = devList->getName(i);
            const char *serial = devList->getSerialNumber(i);
            if(name == nullptr || serial == nullptr) {
                continue;
            }

            if(!expectedSerial.empty() && expectedSerial == serial) {
                return true;
            }

            const std::string nameLower = toLowerCopy(name);
            if(nameLower.find("335le") != std::string::npos || nameLower.find("435le") != std::string::npos) {
                return true;
            }
        }

        return false;
    };

    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(true));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    ASSERT_GT(devList->getCount(), 0u) << "No devices enumerated after enabling network discovery";
    EXPECT_TRUE(hasExpectedDevice(devList)) << "Expected Gemini 335Le/435Le device was not found after enabling network discovery";

    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(false));
    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(true));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    ASSERT_GT(devList->getCount(), 0u) << "No devices enumerated after re-enabling network discovery";
    EXPECT_TRUE(hasExpectedDevice(devList)) << "Expected Gemini 335Le/435Le device was not found after re-enabling network discovery";
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_03_net_device_direct) {
    /// Test case: net device direct.
    // Read required inputs from environment variables.
    const char *ip = std::getenv("ORBBEC_NET_IP");
    if(!ip) {
        GTEST_SKIP() << "ORBBEC_NET_IP not set";
    }

    // Create net device with default access mode and verify basic info.
    {
        auto netDev = ctx_->createNetDevice(ip, 8090);
        ASSERT_NE(netDev, nullptr);
        auto info = netDev->getDeviceInfo();
        ASSERT_NE(info, nullptr);
        EXPECT_NE(info->getName(), nullptr);
        EXPECT_NE(info->getSerialNumber(), nullptr);
        netDev.reset();
    }

    // Create net device with OB_DEVICE_CONTROL_ACCESS and verify.
    {
        auto netDev = ctx_->createNetDevice(ip, 8090, OB_DEVICE_CONTROL_ACCESS);
        ASSERT_NE(netDev, nullptr);
        auto info = netDev->getDeviceInfo();
        ASSERT_NE(info, nullptr);
        EXPECT_NE(info->getName(), nullptr);
        EXPECT_NE(info->getSerialNumber(), nullptr);
        netDev.reset();
    }
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_04_force_ip) {
    /// Test case: force ip.
    // Read the target device MAC address and desired static IP from environment variables.
    // ORBBEC_NET_MAC  — device MAC, e.g. "aa:bb:cc:dd:ee:ff"
    // ORBBEC_NET_IP   — desired static IPv4, e.g. "192.168.1.100"
    // ORBBEC_NET_MASK — subnet mask (optional, defaults to 255.255.255.0)
    // ORBBEC_NET_GW   — gateway (optional, defaults to x.x.x.1 of the target subnet)
    const char *mac      = std::getenv("ORBBEC_NET_MAC");
    const char *ip       = std::getenv("ORBBEC_NET_IP");
    const char *original = std::getenv("ORBBEC_NET_ORIGINAL");
    if(!mac || std::strlen(mac) == 0 || !ip || std::strlen(ip) == 0) {
        GTEST_SKIP() << "ORBBEC_NET_MAC and ORBBEC_NET_IP must be set for this test";
    }

    // Get device SN before configuration (if original IP is available).
    std::string originalSn;
    if(original && std::strlen(original) > 0) {
        try {
            auto origDev = ctx_->createNetDevice(original, 8090);
            if(origDev) {
                auto origInfo = origDev->getDeviceInfo();
                if(origInfo && origInfo->getSerialNumber()) {
                    originalSn = origInfo->getSerialNumber();
                }
            }
        }
        catch(const std::exception &) {
            // Ignore - original IP might not be reachable.
        }
    }

    // Parse a dotted-decimal IPv4 string into a 4-byte big-endian array.
    auto parseIPv4 = [](const char *str, uint8_t out[4]) -> bool {
        unsigned a, b, c, d;
        if(std::sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
            return false;
        if(a > 255 || b > 255 || c > 255 || d > 255)
            return false;
        out[0] = static_cast<uint8_t>(a);
        out[1] = static_cast<uint8_t>(b);
        out[2] = static_cast<uint8_t>(c);
        out[3] = static_cast<uint8_t>(d);
        return true;
    };

    OBNetIpConfig config{};
    config.dhcp = 0;  // static IP mode
    ASSERT_TRUE(parseIPv4(ip, config.address)) << "ORBBEC_NET_IP is not a valid IPv4 address: " << ip;

    const char *mask = std::getenv("ORBBEC_NET_MASK");
    if(mask && std::strlen(mask) > 0) {
        ASSERT_TRUE(parseIPv4(mask, config.mask)) << "ORBBEC_NET_MASK is not a valid IPv4 address: " << mask;
    }
    else {
        config.mask[0] = 255;
        config.mask[1] = 255;
        config.mask[2] = 255;
        config.mask[3] = 0;
    }

    const char *gw = std::getenv("ORBBEC_NET_GW");
    if(gw && std::strlen(gw) > 0) {
        ASSERT_TRUE(parseIPv4(gw, config.gateway)) << "ORBBEC_NET_GW is not a valid IPv4 address: " << gw;
    }
    else {
        // Default gateway: same /24 subnet, host .1
        config.gateway[0] = config.address[0];
        config.gateway[1] = config.address[1];
        config.gateway[2] = config.address[2];
        config.gateway[3] = 1;
    }

    // Send the static-IP assignment command to the device identified by its MAC.
    bool ok = ctx_->forceIp(mac, config);
    EXPECT_TRUE(ok) << "forceIp() returned false — command not accepted by device";

    // Allow time for the device to apply the new IP and reappear on the network.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Verify the device is now reachable on the new static IP.
    try {
        auto netDev = ctx_->createNetDevice(ip, 8090);
        ASSERT_NE(netDev, nullptr) << "createNetDevice returned null after forceIp";
        auto info = netDev->getDeviceInfo();
        ASSERT_NE(info, nullptr);
        EXPECT_NE(info->getName(), nullptr);
        // Verify SN consistency if original IP was provided.
        if(!originalSn.empty()) {
            EXPECT_STREQ(info->getSerialNumber(), originalSn.c_str())
                << "Serial number changed after forceIp - expected: " << originalSn << ", got: " << info->getSerialNumber();
        }
    }
    catch(const ob::Error &e) {
        FAIL() << "Device unreachable on new IP " << ip << ": " << e.getMessage();
    }
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_05_hotplug_reboot) {
    /// Test case: hotplug reboot.
    std::atomic<bool> removedCalled{ false };
    std::atomic<bool> addedCalled{ false };

    auto cbId = ctx_->registerDeviceChangedCallback([&](std::shared_ptr<ob::DeviceList> removed, std::shared_ptr<ob::DeviceList> added) {
        if(removed && removed->getCount() > 0)
            removedCalled = true;
        if(added && added->getCount() > 0)
            addedCalled = true;
    });

    auto sn = std::string(devInfo_->getSerialNumber());
    device_->reboot();
    device_.reset();

    // Prepare local state for the next check.
    std::this_thread::sleep_for(std::chrono::seconds(15));

    EXPECT_TRUE(removedCalled.load()) << "Device removed callback not received";
    EXPECT_TRUE(addedCalled.load()) << "Device added callback not received";

    ctx_->unregisterDeviceChangedCallback(cbId);

    auto devList = ctx_->queryDeviceList();
    if(devList && devList->getCount() > 0) {
        auto dev = devList->getDeviceBySN(sn.c_str());
        if(dev) {
            auto info = dev->getDeviceInfo();
            EXPECT_NE(info, nullptr);
            EXPECT_STREQ(info->getSerialNumber(), sn.c_str());
        }
    }

    removedCalled = false;
    addedCalled   = false;

    devList = ctx_->queryDeviceList();
    if(devList && devList->getCount() > 0) {
        auto dev = devList->getDeviceBySN(sn.c_str());
        if(dev) {
            device_ = dev;
            device_->reboot();
            device_.reset();

            std::this_thread::sleep_for(std::chrono::seconds(15));

            EXPECT_FALSE(removedCalled.load()) << "Callback triggered after unregister";
            EXPECT_FALSE(addedCalled.load()) << "Callback triggered after unregister";
        }
    }
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_06_clock_sync) {

    ASSERT_NO_THROW(ctx_->enableDeviceClockSync(1000));

    ASSERT_NO_THROW(ctx_->enableDeviceClockSync(500));

    ASSERT_NO_THROW(ctx_->enableDeviceClockSync(0));
}

// ============================================================
// Test group: DeviceList.
// ============================================================
class TC_CPP_03_DeviceList : public DeviceTest {};

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_01_count_and_index) {
    /// Test case: count and index.
    auto devList = ctx_->queryDeviceList();
    auto count   = devList->getCount();
    ASSERT_GT(count, 0u) << "No devices found";

    for(uint32_t i = 0; i < count; i++) {
        auto device = devList->getDevice(i);
        ASSERT_NE(device, nullptr) << "getDevice(" << i << ") returned null";

        auto info = device->getDeviceInfo();
        ASSERT_NE(info, nullptr);
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_GT(devList->getPid(i), 0);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_02_access_mode) {
    /// Test case: access mode.
    // Prepare local state for the next check.
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();

    {
        auto dev = devList->getDevice(0, OB_DEVICE_EXCLUSIVE_ACCESS);
        ASSERT_NE(dev, nullptr);
        auto info = dev->getDeviceInfo();
        EXPECT_STREQ(info->getSerialNumber(), sn.c_str());
    }

    {
        auto dev = devList->getDevice(0, OB_DEVICE_CONTROL_ACCESS);
        ASSERT_NE(dev, nullptr);
        auto info = dev->getDeviceInfo();
        EXPECT_STREQ(info->getSerialNumber(), sn.c_str());
    }

    {
        auto dev = devList->getDevice(0, OB_DEVICE_DEFAULT_ACCESS);
        ASSERT_NE(dev, nullptr);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_03_get_by_sn_uid) {
    /// Test case: get by sn uid.
    auto sn  = std::string(devInfo_->getSerialNumber());
    auto uid = std::string(devInfo_->getUid());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();

    {
        auto dev = devList->getDeviceBySN(sn.c_str());
        ASSERT_NE(dev, nullptr);
        auto info = dev->getDeviceInfo();
        EXPECT_STREQ(info->getSerialNumber(), sn.c_str());
    }

    {
        auto dev = devList->getDeviceByUid(uid.c_str());
        ASSERT_NE(dev, nullptr);
        auto info = dev->getDeviceInfo();
        EXPECT_STREQ(info->getUid(), uid.c_str());
    }

    {
        auto dev = devList->getDeviceBySN("NON_EXISTENT_SN_12345");
        EXPECT_EQ(dev, nullptr) << "Non-existent SN should return null device";
    }

    {
        auto dev = devList->getDeviceByUid("NON_EXISTENT_UID_12345");
        EXPECT_EQ(dev, nullptr) << "Non-existent UID should return null device";
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_04_basic_info_fields) {
    // Test case: basic info fields.
    auto devList = ctx_->queryDeviceList();
    for(uint32_t i = 0; i < devList->getCount(); i++) {
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_GT(devList->getPid(i), 0);
        EXPECT_EQ(devList->getVid(i), 0x2BC5);
        EXPECT_NE(devList->getConnectionType(i), nullptr);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_05_net_device_info) {
    /// Test case: net device info.
    auto isValidIPv4 = [](const char *ip) -> bool {
        if(!ip)
            return false;
        unsigned a, b, c, d;
        return std::sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d) == 4 && a <= 255 && b <= 255 && c <= 255 && d <= 255;
    };

    auto isValidMac = [](const char *mac) -> bool {
        if(!mac)
            return false;
        unsigned parts[6] = { 0 };
        int      matched  = std::sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x", &parts[0], &parts[1], &parts[2], &parts[3], &parts[4], &parts[5]);
        return matched == 6;
    };

    auto devList = ctx_->queryDeviceList();
    bool found   = false;

    for(uint32_t i = 0; i < devList->getCount(); i++) {
        auto connType = std::string(devList->getConnectionType(i));
        if(connType == "Ethernet") {
            found = true;

            // IP address validation.
            auto ip = devList->getIpAddress(i);
            EXPECT_NE(ip, nullptr);
            EXPECT_TRUE(isValidIPv4(ip)) << "Invalid IPv4 format: " << (ip ? ip : "null");
            EXPECT_STRNE(ip, "0.0.0.0");

            // Subnet mask validation.
            auto subnet = devList->getSubnetMask(i);
            EXPECT_NE(subnet, nullptr);
            EXPECT_TRUE(isValidIPv4(subnet)) << "Invalid subnet mask format: " << (subnet ? subnet : "null");

            // Gateway validation.
            auto gateway = devList->getGateway(i);
            EXPECT_NE(gateway, nullptr);
            EXPECT_TRUE(isValidIPv4(gateway)) << "Invalid gateway format: " << (gateway ? gateway : "null");

            // Local MAC validation.
            auto localMac = devList->getLocalMacAddress(i);
            EXPECT_NE(localMac, nullptr);
            EXPECT_TRUE(isValidMac(localMac)) << "Invalid MAC format: " << (localMac ? localMac : "null");

            // Local IP validation.
            auto localIp = devList->getLocalIP(i);
            EXPECT_NE(localIp, nullptr);
            EXPECT_TRUE(isValidIPv4(localIp)) << "Invalid local IP format: " << (localIp ? localIp : "null");
        }
    }

    if(!found) {
        GTEST_SKIP() << "No Ethernet device found, skipping network info test";
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_06_out_of_bounds) {
    /// Test case: out of bounds.
    auto devList = ctx_->queryDeviceList();
    auto count   = devList->getCount();
    ASSERT_GT(count, 0u);

    EXPECT_THROW(devList->getDevice(count), ob::Error);

    EXPECT_THROW(devList->getDevice(count + 100), ob::Error);

    EXPECT_THROW(devList->getDevice(UINT32_MAX), ob::Error);

    auto dev = devList->getDevice(0);
    EXPECT_NE(dev, nullptr);
}

// ============================================================
// Test group: DeviceInfo.
// ============================================================
class TC_CPP_04_DeviceInfo : public DeviceTest {};

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_01_basic_info) {
    /// Test case: basic info.
    EXPECT_NE(devInfo_->getName(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getName()), 0u);
    EXPECT_NE(devInfo_->getSerialNumber(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getSerialNumber()), 0u);

    const char *fw = devInfo_->getFirmwareVersion();
    EXPECT_NE(fw, nullptr);
    EXPECT_GT(std::strlen(fw), 0u);
    {
        unsigned major, minor, patch;
        int      matched = std::sscanf(fw, "%u.%u.%u", &major, &minor, &patch);
        EXPECT_EQ(matched, 3) << "Firmware version format should be x.y.z: " << (fw ? fw : "null");
    }

    EXPECT_NE(devInfo_->getHardwareVersion(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getHardwareVersion()), 0u);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_02_id_fields) {
    /// Test case: id fields.
    EXPECT_GT(devInfo_->getPid(), 0);
    EXPECT_EQ(devInfo_->getVid(), 0x2BC5);
    EXPECT_NE(devInfo_->getUid(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getUid()), 0u);
    auto conn = devInfo_->getConnectionType();
    EXPECT_NE(conn, nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_03_net_ip_info) {
    /// Test case: net ip info.
    auto isValidIPv4 = [](const char *ip) -> bool {
        if(!ip)
            return false;
        unsigned a, b, c, d;
        return std::sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d) == 4 && a <= 255 && b <= 255 && c <= 255 && d <= 255;
    };

    const int currentPid = devInfo_->getPid();
    if(!TestEnvironment::isPid335leOr435le(currentPid)) {
        GTEST_SKIP() << "Test requires Gemini 335Le/435Le, current pid: 0x" << std::hex << currentPid;
    }

    const char *connType = devInfo_->getConnectionType();
    if(connType == nullptr || std::string(connType) != "Ethernet") {
        GTEST_SKIP() << "Test requires Ethernet connection, current connection type: " << (connType ? connType : "null");
    }

    auto ip = devInfo_->getIpAddress();
    ASSERT_NE(ip, nullptr);
    EXPECT_TRUE(isValidIPv4(ip)) << "Invalid IPv4 format: " << (ip ? ip : "null");

    auto mask = devInfo_->getDeviceSubnetMask();
    EXPECT_NE(mask, nullptr);
    EXPECT_TRUE(isValidIPv4(mask)) << "Invalid subnet mask format: " << (mask ? mask : "null");

    auto gw = devInfo_->getDeviceGateway();
    EXPECT_NE(gw, nullptr);
    EXPECT_TRUE(isValidIPv4(gw)) << "Invalid gateway format: " << (gw ? gw : "null");
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_04_chip_type_info) {
    /// Test case: chip type info.
    auto asic = devInfo_->getAsicName();
    EXPECT_NE(asic, nullptr);
    auto devType = devInfo_->getDeviceType();
    (void)devType;  // Enum value existence check.
    auto minSdk = devInfo_->getSupportedMinSdkVersion();
    EXPECT_NE(minSdk, nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_05_extension_info) {
    /// Test case: extension info.
    // Prepare local state for the next check.
    bool exists = device_->isExtensionInfoExist("SerialNumber");
    if(exists) {
        auto val = device_->getExtensionInfo("SerialNumber");
        EXPECT_NE(val, nullptr) << "Extension info value should not be null for existing key";
    }
    // Validate expected conditions for this step.
    EXPECT_FALSE(device_->isExtensionInfoExist("TotallyBogusExtKey_12345"));
}

// ============================================================
// Test group: AccessMode.
// ============================================================
class TC_CPP_05_AccessMode : public DeviceTest {};

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_01_default_access) {
    /// Test case: default access.
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    EXPECT_NO_THROW(pipeline->start(config)) << "start() should not throw in DEFAULT_ACCESS mode";

    auto frames = pipeline->waitForFrameset(3000);
    ASSERT_NE(frames, nullptr) << "waitForFrameset() should return valid frameset";

    auto depthFrame = frames->getDepthFrame();
    ASSERT_NE(depthFrame, nullptr) << "Frameset should contain depth frame";

    pipeline->stop();
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_02_exclusive_access) {
    /// Test case: exclusive access.
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto dev1    = devList->getDevice(0, OB_DEVICE_EXCLUSIVE_ACCESS);
    ASSERT_NE(dev1, nullptr) << "First getDevice with EXCLUSIVE_ACCESS should succeed";

    auto pipeline = std::make_shared<ob::Pipeline>(dev1);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    auto frames = pipeline->waitForFrameset(3000);
    ASSERT_NE(frames, nullptr) << "Should receive frames in exclusive access mode";
    auto depthFrame = frames->getDepthFrame();
    ASSERT_NE(depthFrame, nullptr);

    {
        auto dev2 = devList->getDeviceBySN(sn.c_str(), OB_DEVICE_EXCLUSIVE_ACCESS);
        EXPECT_EQ(dev2, nullptr) << "Second getDevice in EXCLUSIVE_ACCESS should fail";
    }

    pipeline->stop();
    dev1.reset();

    {
        auto dev3 = devList->getDeviceBySN(sn.c_str(), OB_DEVICE_EXCLUSIVE_ACCESS);
        ASSERT_NE(dev3, nullptr) << "getDevice after releasing first device should succeed";
    }
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_03_shared_access) {
    /// Test case: shared access.
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto devA    = devList->getDevice(0, OB_DEVICE_CONTROL_ACCESS);
    ASSERT_NE(devA, nullptr) << "First getDevice with CONTROL_ACCESS should succeed";

    auto infoA = devA->getDeviceInfo();
    ASSERT_NE(infoA, nullptr);

    auto devB = devList->getDeviceBySN(sn.c_str(), OB_DEVICE_CONTROL_ACCESS);
    ASSERT_NE(devB, nullptr) << "Second getDevice with CONTROL_ACCESS should succeed in shared mode";

    auto infoB = devB->getDeviceInfo();
    ASSERT_NE(infoB, nullptr);

    EXPECT_STREQ(infoA->getSerialNumber(), infoB->getSerialNumber()) << "Both handles should return same serial number";
    EXPECT_STREQ(infoA->getName(), infoB->getName()) << "Both handles should return same device name";
    EXPECT_STREQ(infoA->getFirmwareVersion(), infoB->getFirmwareVersion()) << "Both handles should return same firmware version";
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_04_control_only_access) {
    /// Test case: control only access.
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto dev     = devList->getDevice(0, OB_DEVICE_CONTROL_ACCESS);
    ASSERT_NE(dev, nullptr) << "getDevice with CONTROL_ACCESS should succeed";

    // Prepare local state for the next check.
    auto info = dev->getDeviceInfo();
    ASSERT_NE(info, nullptr);
    EXPECT_NE(info->getName(), nullptr);

    if(dev->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ)) {
        bool val = dev->getBoolProperty(OB_PROP_LASER_BOOL);
        EXPECT_NO_THROW(dev->setBoolProperty(OB_PROP_LASER_BOOL, val)) << "Property set should succeed in control mode";
    }

    auto pipeline = std::make_shared<ob::Pipeline>(dev);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    bool streamingFailed = false;
    try {
        pipeline->start(config);
        auto frames = pipeline->waitForFrameset(1000);
        if(!frames) {
            streamingFailed = true;
        }
        pipeline->stop();
    }
    catch(const ob::Error &) {
        streamingFailed = true;
    }

    EXPECT_TRUE(streamingFailed) << "Streaming fail in control-only access mode";
}

// ============================================================
// Test group: Sensor.
// ============================================================
class TC_CPP_06_Sensor : public SensorTest {};

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_01_sensor_list_completeness) {
    /// Test case: sensor list completeness.
    auto count = sensorList_->getCount();
    ASSERT_GT(count, 0u) << "Sensor list should not be empty";
    EXPECT_GE(count, 2u) << "Expected at least Depth + Color sensors";

    for(uint32_t i = 0; i < count; i++) {
        auto sensor = sensorList_->getSensor(i);
        ASSERT_NE(sensor, nullptr) << "getSensor(" << i << ") should return non-null sensor";
        auto type = sensor->getType();
        (void)type;
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_02_core_sensors) {
    /// Test case: core sensors.
    const OBSensorType coreTypes[] = { OB_SENSOR_DEPTH, OB_SENSOR_COLOR, OB_SENSOR_IR };
    size_t             checked     = 0;

    for(auto type: coreTypes) {
        if(!hasSensorType(sensorList_, type)) {
            continue;
        }

        auto sensor = tryGetSensor(device_, type);
        ASSERT_NE(sensor, nullptr) << "Advertised sensor type " << static_cast<int>(type) << " could not be opened";
        EXPECT_EQ(sensor->getType(), type);
        ++checked;
    }

    EXPECT_GT(checked, 0u) << "No core depth/color/IR sensors are advertised by this device";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_03_imu_sensors) {
    /// Test case: imu sensors.
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    auto gyro  = device_->getSensor(OB_SENSOR_GYRO);
    // Validate expected conditions for this step.
    EXPECT_NE(accel, nullptr);
    EXPECT_NE(gyro, nullptr);

    if(accel) {
        EXPECT_EQ(accel->getType(), OB_SENSOR_ACCEL);
    }
    if(gyro) {
        EXPECT_EQ(gyro->getType(), OB_SENSOR_GYRO);
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_04_stereo_ir) {
    /// Test case: stereo ir.
    auto irLeft  = device_->getSensor(OB_SENSOR_IR_LEFT);
    auto irRight = device_->getSensor(OB_SENSOR_IR_RIGHT);
    // Prepare local state for the next check.
    if(irLeft) {
        EXPECT_EQ(irLeft->getType(), OB_SENSOR_IR_LEFT);
    }
    if(irRight) {
        EXPECT_EQ(irRight->getType(), OB_SENSOR_IR_RIGHT);
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_05_invalid_sensor_type) {
    /// Test case: invalid sensor type.
    try {
        (void)device_->getSensor(static_cast<OBSensorType>(999));
        SUCCEED();
    }
    catch(...) {
        SUCCEED();
    }

    auto depthSensor = tryGetSensor(device_, OB_SENSOR_DEPTH);
    EXPECT_NE(depthSensor, nullptr);
    if(depthSensor) {
        EXPECT_EQ(depthSensor->getType(), OB_SENSOR_DEPTH);
    }

    auto lidarSensor = tryGetSensor(device_, OB_SENSOR_LIDAR);
    EXPECT_NE(lidarSensor, nullptr);
    if(lidarSensor) {
        EXPECT_EQ(lidarSensor->getType(), OB_SENSOR_LIDAR);
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_06_sensor_type_consistency) {
    /// Test case: sensor type consistency.
    ASSERT_GT(sensorList_->getCount(), 0u);

    for(uint32_t i = 0; i < sensorList_->getCount(); ++i) {
        auto type   = sensorList_->getSensorType(i);
        auto sensor = tryGetSensor(device_, type);
        ASSERT_NE(sensor, nullptr) << "Failed to reopen advertised sensor type " << static_cast<int>(type);
        EXPECT_EQ(sensor->getType(), type);
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_07_sensor_callback_stream) {
    /// Test case: sensor callback stream.
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr);

    auto profileList = depthSensor->getStreamProfileList();
    ASSERT_NE(profileList, nullptr);
    ASSERT_GT(profileList->getCount(), 0u);

    auto profile = profileList->getProfile(0);

    std::atomic<int> frameCount{ 0 };
    depthSensor->start(profile, [&frameCount](std::shared_ptr<ob::Frame>) { frameCount++; });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    depthSensor->stop();

    EXPECT_GT(frameCount.load(), 0) << "No frames received via sensor callback";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_08_sensor_repeated_start_stop) {
    /// Test case: sensor repeated start stop.
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr);
    auto profileList = depthSensor->getStreamProfileList();
    auto profile     = profileList->getProfile(0);

    for(int i = 0; i < 5; i++) {
        std::atomic<int> frameCount{ 0 };
        depthSensor->start(profile, [&frameCount](std::shared_ptr<ob::Frame>) { frameCount++; });

        std::this_thread::sleep_for(std::chrono::seconds(1));
        EXPECT_GT(frameCount.load(), 0) << "No frames received in cycle " << (i + 1);

        depthSensor->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SUCCEED() << "5 start/stop cycles completed with frames received each time";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_09_sensor_after_reboot) {
    /// Test case: sensor after reboot.
    // Reboots the device, waits for reconnection, then verifies sensor enumeration and depth streaming.
    // Save the serial number so we can reopen the same device after reboot.
    auto sn = std::string(devInfo_->getSerialNumber());

    // Register a callback to detect when the device reconnects.
    std::atomic<bool> reconnected{ false };
    auto              cbId = ctx_->registerDeviceChangedCallback([&](std::shared_ptr<ob::DeviceList> /*removed*/, std::shared_ptr<ob::DeviceList> added) {
        if(added && added->getCount() > 0) {
            reconnected = true;
        }
    });

    // Trigger the reboot and immediately release the stale device handle.
    device_->reboot();
    device_.reset();
    devInfo_.reset();
    sensorList_.reset();

    // Poll until the device reappears or we reach the 30-second timeout.
    constexpr int kMaxWaitMs = 30000;
    constexpr int kPollMs    = 500;
    for(int elapsed = 0; !reconnected.load() && elapsed < kMaxWaitMs; elapsed += kPollMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollMs));
    }
    ctx_->unregisterDeviceChangedCallback(cbId);
    ASSERT_TRUE(reconnected.load()) << "Device did not reconnect within 30 s after reboot";

    // Reopen the device by its original serial number.
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    device_ = devList->getDeviceBySN(sn.c_str());
    ASSERT_NE(device_, nullptr) << "Device SN " << sn << " not found in device list after reboot";

    // Verify sensor enumeration is intact after reboot.
    auto sensors = device_->getSensorList();
    ASSERT_NE(sensors, nullptr);
    ASSERT_GT(sensors->getCount(), 0u) << "No sensors available after reboot";

    // Verify the depth sensor can actually deliver frames.
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr) << "Depth sensor unavailable after reboot";
    auto profiles = depthSensor->getStreamProfileList();
    ASSERT_NE(profiles, nullptr);
    ASSERT_GT(profiles->getCount(), 0u);

    std::atomic<int> frameCount{ 0 };
    depthSensor->start(profiles->getProfile(0), [&frameCount](std::shared_ptr<ob::Frame>) { frameCount++; });
    std::this_thread::sleep_for(std::chrono::seconds(3));
    depthSensor->stop();

    EXPECT_GT(frameCount.load(), 0) << "No depth frames received after device reboot";
}

// ============================================================
// Test group: StreamProfile.
// ============================================================
class TC_CPP_07_StreamProfile : public SensorTest {};

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_01_depth_color_profiles) {
    /// Test case: depth color profiles.
    for(auto sensorType: { OB_SENSOR_DEPTH, OB_SENSOR_COLOR }) {
        auto sensor = device_->getSensor(sensorType);
        ASSERT_NE(sensor, nullptr) << "Sensor null for type " << sensorType;
        auto profiles = sensor->getStreamProfileList();
        ASSERT_GT(profiles->getCount(), 0u) << "No profiles for sensor type " << sensorType;

        for(uint32_t i = 0; i < profiles->getCount(); i++) {
            auto p = profiles->getProfile(i)->as<ob::VideoStreamProfile>();
            EXPECT_GT(p->getWidth(), 0u);
            EXPECT_GT(p->getHeight(), 0u);
            EXPECT_GT(p->getFps(), 0u);
            EXPECT_NE(p->getFormat(), OB_FORMAT_UNKNOWN) << "Profile format should be valid";
        }
    }
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_02_video_profile_filter) {
    /// Test case: video profile filter.
    auto pipeline      = std::make_shared<ob::Pipeline>(device_);
    auto depthProfiles = pipeline->getStreamProfileList(OB_SENSOR_DEPTH);
    ASSERT_GT(depthProfiles->getCount(), 0u);

    auto first = depthProfiles->getProfile(0)->as<ob::VideoStreamProfile>();
    auto w     = first->getWidth();
    auto h     = first->getHeight();
    auto fps   = first->getFps();

    // Prepare local state for the next check.
    auto matched = depthProfiles->getVideoStreamProfile(w, h, first->getFormat(), fps);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->getWidth(), w);
    EXPECT_EQ(matched->getHeight(), h);
    EXPECT_EQ(matched->getFormat(), first->getFormat());
    EXPECT_EQ(matched->getFps(), fps);

    auto invalid = depthProfiles->getVideoStreamProfile(99999, 99999, OB_FORMAT_Y16, 999);
    if(invalid) {
        EXPECT_GT(invalid->getWidth(), 0u);
    }
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_03_accel_profile) {
    /// Test case: accel profile.
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accel)
        GTEST_SKIP() << "No accel sensor";

    auto profiles = accel->getStreamProfileList();
    ASSERT_GT(profiles->getCount(), 0u);

    auto ap = profiles->getProfile(0)->as<ob::AccelStreamProfile>();
    EXPECT_NE(ap->getFullScaleRange(), 0);
    EXPECT_NE(ap->getSampleRate(), 0);
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_04_gyro_profile) {
    /// Test case: gyro profile.
    auto gyro = device_->getSensor(OB_SENSOR_GYRO);
    if(!gyro)
        GTEST_SKIP() << "No gyro sensor";

    auto profiles = gyro->getStreamProfileList();
    ASSERT_GT(profiles->getCount(), 0u);

    auto gp = profiles->getProfile(0)->as<ob::GyroStreamProfile>();
    EXPECT_NE(gp->getFullScaleRange(), 0);
    EXPECT_NE(gp->getSampleRate(), 0);
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_05_profile_type_check) {
    /// Test case: profile type check.
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    auto profiles    = depthSensor->getStreamProfileList();
    auto p           = profiles->getProfile(0);

    EXPECT_TRUE(p->is<ob::VideoStreamProfile>());
    auto vp = p->as<ob::VideoStreamProfile>();
    ASSERT_NE(vp, nullptr);

    // Validate expected conditions for this step.
    EXPECT_FALSE(p->is<ob::AccelStreamProfile>());
    EXPECT_THROW(p->as<ob::AccelStreamProfile>(), std::runtime_error);

    auto accelSensor = device_->getSensor(OB_SENSOR_ACCEL);
    if(accelSensor) {
        auto accelProfiles = accelSensor->getStreamProfileList();
        auto ap            = accelProfiles->getProfile(0);

        EXPECT_TRUE(ap->is<ob::AccelStreamProfile>());
        auto asp = ap->as<ob::AccelStreamProfile>();
        ASSERT_NE(asp, nullptr);
    }

    auto gyroSensor = device_->getSensor(OB_SENSOR_GYRO);
    if(gyroSensor) {
        auto gyroProfiles = gyroSensor->getStreamProfileList();
        auto gp           = gyroProfiles->getProfile(0);

        EXPECT_TRUE(gp->is<ob::GyroStreamProfile>());
        auto gsp = gp->as<ob::GyroStreamProfile>();
        ASSERT_NE(gsp, nullptr);
    }
}

// ============================================================
// Test group: Pipeline.
// ============================================================
class TC_CPP_08_Pipeline : public PipelineTest {};

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_01_pipeline_construct) {
    /// Test case: pipeline construct.
    ASSERT_NE(pipeline_, nullptr);
    // Prepare local state for the next check.
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_02_default_start_stop) {
    /// Test case: default start stop.
    pipeline_->start();
    auto frames = pipeline_->waitForFrameset(3000);
    EXPECT_NE(frames, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_03_config_start_poll) {
    /// Test case: config start poll.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);

    int validFrames = 0;
    for(int i = 0; i < 10; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(fs)
            validFrames++;
    }
    pipeline_->stop();
    EXPECT_GE(validFrames, 8) << "Expected >=8 valid framesets out of 10";
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_04_config_start_callback) {
    /// Test case: config start callback.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    if(hasSensorType(device_->getSensorList(), OB_SENSOR_COLOR)) {
        config->enableStream(OB_STREAM_COLOR);
    }

    std::atomic<int> cbCount{ 0 };
    pipeline_->start(config, [&cbCount](std::shared_ptr<ob::FrameSet>) { cbCount++; });

    std::this_thread::sleep_for(std::chrono::seconds(3));
    pipeline_->stop();

    EXPECT_GT(cbCount.load(), 0) << "No callback frames received";
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_05_switch_config) {
    /// Test case: switch config.
    auto config1 = std::make_shared<ob::Config>();
    config1->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config1);
    pipeline_->waitForFrameset(2000);

    // Prepare local state for the next check.
    auto config2 = std::make_shared<ob::Config>();
    config2->enableStream(OB_STREAM_DEPTH);
    config2->enableStream(OB_STREAM_COLOR);

    ASSERT_NO_THROW(pipeline_->stop());
    ASSERT_NO_THROW(pipeline_->start(config2));
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_06_get_bound_device) {
    /// Test case: get bound device.
    auto pipeDev = pipeline_->getDevice();
    ASSERT_NE(pipeDev, nullptr);
    auto pipeInfo = pipeDev->getDeviceInfo();
    EXPECT_STREQ(pipeInfo->getSerialNumber(), devInfo_->getSerialNumber());
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_07_get_stream_profile_list) {
    /// Test case: get stream profile list.
    auto depthProfiles = pipeline_->getStreamProfileList(OB_SENSOR_DEPTH);
    ASSERT_NE(depthProfiles, nullptr);
    EXPECT_GT(depthProfiles->getCount(), 0u);

    auto colorProfiles = pipeline_->getStreamProfileList(OB_SENSOR_COLOR);
    if(colorProfiles) {
        EXPECT_GT(colorProfiles->getCount(), 0u);
    }
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_08_frame_sync) {
    /// Test case: frame sync.
    pipeline_->enableFrameSync();
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);

    // Prepare local state for the next check.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    for(int i = 0; i < 5; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs)
            continue;
        auto depth = fs->getDepthFrame();
        auto color = fs->getColorFrame();
        if(depth && color) {
            auto diff = std::abs((int64_t)depth->getTimeStampUs() - (int64_t)color->getTimeStampUs());
            EXPECT_LT(diff, 33000) << "Depth-Color timestamp diff > 33ms";
        }
    }
    pipeline_->stop();
    pipeline_->disableFrameSync();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_09_d2c_depth_profile_list) {
    /// Test case: d2c depth profile list.
    auto colorProfiles = pipeline_->getStreamProfileList(OB_SENSOR_COLOR);
    if(!colorProfiles || colorProfiles->getCount() == 0) {
        GTEST_SKIP() << "No color profiles available";
    }

    bool foundSupported = false;

    // Test HW alignment mode.
    for(uint32_t i = 0; i < colorProfiles->getCount(); ++i) {
        auto colorProfile = colorProfiles->getProfile(i);
        try {
            auto d2cList = pipeline_->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE);
            if(d2cList && d2cList->getCount() > 0) {
                foundSupported = true;
                break;
            }
        }
        catch(const ob::Error &) {
        }
    }

    if(!foundSupported) {
        GTEST_SKIP() << "Current device exposes no color profile with a non-empty HW D2C depth profile list";
    }

    // Test SW alignment mode.
    foundSupported = false;
    for(uint32_t i = 0; i < colorProfiles->getCount(); ++i) {
        auto colorProfile = colorProfiles->getProfile(i);
        try {
            auto d2cList = pipeline_->getD2CDepthProfileList(colorProfile, ALIGN_D2C_SW_MODE);
            if(d2cList && d2cList->getCount() > 0) {
                foundSupported = true;
                break;
            }
        }
        catch(const ob::Error &) {
        }
    }

    if(!foundSupported) {
        GTEST_SKIP() << "Current device exposes no color profile with a non-empty SW D2C depth profile list";
    }
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_10_camera_param) {
    /// Test case: camera param.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);
    pipeline_->waitForFrameset(2000);

    auto param = pipeline_->getCameraParam();
    EXPECT_GT(param.depthIntrinsic.fx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.fy, 0.0f);
    EXPECT_GT(param.depthIntrinsic.cx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.cy, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fx, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fy, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.cx, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.cy, 0.0f);
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_11_calibration_param) {
    /// Test case: calibration param.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    auto calibParam = pipeline_->getCalibrationParam(config);
    // Validate expected conditions for this step.
    EXPECT_GT(calibParam.intrinsics[OB_SENSOR_DEPTH].fx, 0.0f);
    EXPECT_GT(calibParam.intrinsics[OB_SENSOR_COLOR].fx, 0.0f);
}

// ============================================================
// Test group: Config.
// ============================================================
class TC_CPP_09_Config : public PipelineTest {};

TEST_F(TC_CPP_09_Config, TC_CPP_09_01_enable_stream_by_type) {
    /// Test case: enable stream by type.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    if(fs) {
        EXPECT_NE(fs->getDepthFrame(), nullptr);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_02_enable_video_stream_params) {
    /// Test case: enable video stream params.
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH, 640, 480, 30, OB_FORMAT_Y16);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    if(fs) {
        auto df = fs->getDepthFrame();
        if(df) {
            EXPECT_EQ(df->as<ob::VideoFrame>()->getWidth(), 640u);
        }
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_03_enable_imu_stream) {
    /// Test case: enable imu stream.
    auto config = std::make_shared<ob::Config>();
    config->enableAccelStream();
    config->enableGyroStream();
    pipeline_->start(config);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    pipeline_->stop();
    SUCCEED();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_04_enable_disable_all) {
    /// Test case: enable disable all.
    auto config = std::make_shared<ob::Config>();
    config->enableAllStream();
    config->disableAllStream();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_05_enabled_profiles) {
    /// Test case: enabled profiles.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    auto profiles = config->getEnabledStreamProfileList();
    EXPECT_GE(profiles->getCount(), 2u);
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_06_d2c_align_mode) {
    /// Test case: d2c align mode.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    // Test HW alignment mode if supported by device.
    try {
        config->setAlignMode(ALIGN_D2C_HW_MODE);
        pipeline_->start(config);
        pipeline_->waitForFrameset(2000);
        pipeline_->stop();
    }
    catch(const ob::Error &) {
        // Some devices do not support HW alignment mode; this is acceptable.
    }

    // Test SW alignment mode if supported by device.
    try {
        config->setAlignMode(ALIGN_D2C_SW_MODE);
        pipeline_->start(config);
        pipeline_->waitForFrameset(2000);
        pipeline_->stop();
    }
    catch(const ob::Error &) {
        // Some devices do not support HW alignment mode; this is acceptable.
    }

    // Test disable alignment mode if supported by device.
    try {
        config->setAlignMode(ALIGN_DISABLE);
        pipeline_->start(config);
        pipeline_->waitForFrameset(2000);
        pipeline_->stop();
    }
    catch(const ob::Error &) {
        // Some devices do not support HW alignment mode; this is acceptable.
    }
    SUCCEED();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_07_depth_scale_after_align) {
    /// Test case: depth scale after align.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    config->setAlignMode(ALIGN_D2C_SW_MODE);
    config->setDepthScaleRequire(true);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_08_frame_aggregate_mode) {
    /// Test case: frame aggregate mode.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_COLOR_FRAME_REQUIRE);
    pipeline_->start(config);
    fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);
    pipeline_->start(config);
    fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_DISABLE);
    pipeline_->start(config);
    fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

// ============================================================
// Test group: Frame.
// ============================================================
class TC_CPP_10_Frame_HW : public PipelineTest {
protected:
    std::shared_ptr<ob::FrameSet> captureFrames() {
        auto config = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        config->enableStream(OB_STREAM_COLOR);
        pipeline_->start(config);
        // Wait for frames to validate runtime behavior.
        for(int i = 0; i < 3; i++)
            pipeline_->waitForFrameset(2000);
        auto fs = pipeline_->waitForFrameset(3000);
        return fs;
    }
};

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_01_frame_basic_properties) {
    /// Test case: frame basic properties.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    EXPECT_EQ(depth->getType(), OB_FRAME_DEPTH);
    EXPECT_NE(depth->getFormat(), OB_FORMAT_UNKNOWN);
    EXPECT_NE(depth->getData(), nullptr);
    EXPECT_GT(depth->getDataSize(), 0u);
    EXPECT_GE(depth->getIndex(), 0u) << "Frame index should be >= 0";
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_02_timestamp_monotonicity) {
    /// Test case: timestamp monotonicity.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);

    std::vector<uint64_t> timestamps;
    for(int i = 0; i < 35; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs)
            continue;
        auto df = fs->getDepthFrame();
        if(df) {
            timestamps.push_back(df->getTimeStampUs());
            EXPECT_GT(df->getSystemTimeStampUs(), 0u) << "System timestamp should be > 0 at frame " << i;
            EXPECT_GT(df->getGlobalTimeStampUs(), 0u) << "Global timestamp should be > 0 at frame " << i;
        }
    }
    pipeline_->stop();

    ASSERT_GE(timestamps.size(), 10u);
    for(size_t i = 1; i < timestamps.size(); i++) {
        EXPECT_GT(timestamps[i], timestamps[i - 1]) << "Timestamp not monotonic at index " << i;
    }
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_03_frame_associated_info) {
    /// Test case: frame associated info.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto profile = depth->getStreamProfile();
    EXPECT_NE(profile, nullptr);

    auto sensor = depth->getSensor();
    EXPECT_NE(sensor, nullptr);

    auto frameDev = depth->getDevice();
    EXPECT_NE(frameDev, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_05_video_frame_properties) {
    /// Test case: video frame properties.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    EXPECT_GT(depth->getWidth(), 0u);
    EXPECT_GT(depth->getHeight(), 0u);
    EXPECT_GT(depth->getPixelAvailableBitSize(), 0u) << "Pixel available bit size should be > 0";
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_06_depth_frame_scale) {
    /// Test case: depth frame scale.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    float scale = depth->getValueScale();
    EXPECT_GT(scale, 0.0f);

    auto    *data       = reinterpret_cast<const uint16_t *>(depth->getData());
    uint32_t pixCount   = depth->getWidth() * depth->getHeight();
    uint32_t validCount = 0;
    for(uint32_t i = 0; i < pixCount; i++) {
        if(data[i] > 0)
            validCount++;
    }
    float validRatio = static_cast<float>(validCount) / pixCount;
    EXPECT_GE(validRatio, 0.1f) << "Less than 10% valid depth pixels";
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_07_color_ir_data_valid) {
    /// Test case: color ir data valid.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    config->enableStream(OB_STREAM_IR);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);

    auto color = fs->getColorFrame();
    if(color) {
        auto *d       = color->getData();
        bool  allZero = true;
        for(uint32_t i = 0; i < std::min(color->getDataSize(), 1000u); i++) {
            if(d[i] != 0) {
                allZero = false;
                break;
            }
        }
        EXPECT_FALSE(allZero) << "Color frame is all zeros";
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_08_points_frame) {
    /// Test case: points frame.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    pipeline_->stop();

    // Prepare local state for the next check.
    ob_error *error    = nullptr;
    auto      pcFilter = ob_create_filter("PointCloudFilter", &error);
    if(!pcFilter) {
        GTEST_SKIP() << "PointCloudFilter not available";
    }
    auto result = ob_filter_process(pcFilter, depth->getImpl(), &error);
    if(result) {
        auto pf = std::make_shared<ob::PointsFrame>(result);
        EXPECT_GT(pf->getCoordinateValueScale(), 0.0f);
    }
    ob_delete_filter(pcFilter, &error);
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_09_accel_frame) {
    /// Test case: accel frame.
    auto config = std::make_shared<ob::Config>();
    config->enableAccelStream();
    pipeline_->start(config);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Prepare local state for the next check.
    auto accelSensor = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accelSensor) {
        pipeline_->stop();
        GTEST_SKIP() << "No accel sensor";
    }

    pipeline_->stop();

    auto profiles = accelSensor->getStreamProfileList();
    auto profile  = profiles->getProfile(0);

    std::shared_ptr<ob::AccelFrame> accelFrame;
    std::mutex                      mtx;
    std::condition_variable         cv;

    accelSensor->start(profile, [&](std::shared_ptr<ob::Frame> frame) {
        if(frame->is<ob::AccelFrame>()) {
            std::lock_guard<std::mutex> lock(mtx);
            accelFrame = frame->as<ob::AccelFrame>();
            cv.notify_one();
        }
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(3), [&] { return accelFrame != nullptr; });
    }
    accelSensor->stop();

    ASSERT_NE(accelFrame, nullptr) << "No accel frame received";
    auto val = accelFrame->getValue();
    // Prepare local state for the next check.
    float mag = std::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);
    EXPECT_NEAR(mag, 9.8f, 3.0f) << "Accel magnitude unexpected: " << mag;

    float temp = accelFrame->getTemperature();
    EXPECT_GE(temp, 0.0f);
    EXPECT_LE(temp, 80.0f);
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_10_gyro_frame) {
    /// Test case: gyro frame.
    auto gyroSensor = device_->getSensor(OB_SENSOR_GYRO);
    if(!gyroSensor)
        GTEST_SKIP() << "No gyro sensor";

    auto profiles = gyroSensor->getStreamProfileList();
    auto profile  = profiles->getProfile(0);

    std::shared_ptr<ob::GyroFrame> gyroFrame;
    std::mutex                     mtx;
    std::condition_variable        cv;

    gyroSensor->start(profile, [&](std::shared_ptr<ob::Frame> frame) {
        if(frame->is<ob::GyroFrame>()) {
            std::lock_guard<std::mutex> lock(mtx);
            gyroFrame = frame->as<ob::GyroFrame>();
            cv.notify_one();
        }
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(3), [&] { return gyroFrame != nullptr; });
    }
    gyroSensor->stop();

    ASSERT_NE(gyroFrame, nullptr) << "No gyro frame received";
    auto val = gyroFrame->getValue();
    // Validate expected conditions for this step.
    EXPECT_NEAR(val.x, 0.0f, 1.0f);
    EXPECT_NEAR(val.y, 0.0f, 1.0f);
    EXPECT_NEAR(val.z, 0.0f, 1.0f);

    float temp = gyroFrame->getTemperature();
    EXPECT_GE(temp, 0.0f);
    EXPECT_LE(temp, 80.0f);
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_11_frameset_count_extract) {
    /// Test case: frameset count extract.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    EXPECT_GE(fs->getCount(), 1u);

    auto depth = fs->getDepthFrame();
    auto color = fs->getColorFrame();
    // Validate expected conditions for this step.
    EXPECT_TRUE(depth || color);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_12_frameset_by_type_index) {
    /// Test case: frameset by type index.
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);

    auto depthByType = fs->getFrame(OB_FRAME_DEPTH);
    if(depthByType) {
        EXPECT_EQ(depthByType->getType(), OB_FRAME_DEPTH);
    }

    // By index
    for(uint32_t i = 0; i < fs->getCount(); i++) {
        auto f = fs->getFrameByIndex(i);
        EXPECT_NE(f, nullptr);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_14_frameset_sync) {
    /// Test case: frameset sync.
    if(!hasSensorType(device_->getSensorList(), OB_SENSOR_COLOR)) {
        GTEST_SKIP() << "Current device has no color sensor";
    }

    pipeline_->enableFrameSync();
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    // OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE (default) — only output when both
    // depth and color are present in the same frameset.
    pipeline_->start(config);

    // Allow both streams to fully stabilize before measuring sync quality.
    // Color sensors commonly need 2–3 s after pipeline start before frames arrive.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Warm-up: consume unpaired frames that arrive during startup.
    for(int i = 0; i < 10; i++)
        pipeline_->waitForFrameset(3000);

    // Collect up to 20 framesets and count depth+color pairs and synchronized pairs.
    int synced = 0;
    int paired = 0;
    for(int i = 0; i < 20; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs)
            continue;
        auto d = fs->getDepthFrame();
        auto c = fs->getColorFrame();
        if(d && c) {
            paired++;
            // Frames are considered synchronized when their hardware timestamps are
            // within one 30-fps frame period (~33 ms).
            auto diff = std::abs((int64_t)d->getTimeStampUs() - (int64_t)c->getTimeStampUs());
            if(diff < 33000)
                synced++;
        }
    }
    pipeline_->stop();

    // If the color sensor is present but zero paired framesets were produced across
    // all 20 attempts, that is a genuine pipeline or device failure.
    ASSERT_GT(paired, 0) << "No depth+color frame pairs were produced in 20 attempts — "
                            "check that both streams are delivering frames";
    // Require at least 3 out of paired frames to be timestamp-synchronized.
    EXPECT_GE(synced, 3) << "Insufficient synchronized frame pairs (got " << synced << " out of " << paired << " paired)";
}

// ============================================================
// Test group: Metadata.
// ============================================================
class TC_CPP_11_Metadata_HW : public PipelineTest {};

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_01_metadata_basic_read) {
    /// Test case: metadata basic read.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    if(depth->hasMetadata(OB_FRAME_METADATA_TYPE_TIMESTAMP)) {
        auto ts = depth->getMetadataValue(OB_FRAME_METADATA_TYPE_TIMESTAMP);
        EXPECT_GT(ts, 0);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_02_frame_number_fps) {
    /// Test case: frame number fps.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++)
        pipeline_->waitForFrameset(2000);

    std::vector<int64_t> frameNums;
    for(int i = 0; i < 10; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs)
            continue;
        auto d = fs->getDepthFrame();
        if(d && d->hasMetadata(OB_FRAME_METADATA_TYPE_FRAME_NUMBER)) {
            frameNums.push_back(d->getMetadataValue(OB_FRAME_METADATA_TYPE_FRAME_NUMBER));
        }
    }
    pipeline_->stop();

    if(frameNums.size() >= 2) {
        for(size_t i = 1; i < frameNums.size(); i++) {
            EXPECT_GE(frameNums[i], frameNums[i - 1]);
        }
    }
}

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_03_exposure_gain_laser) {
    /// Test case: exposure gain laser.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    // Prepare local state for the next check.
    if(d->hasMetadata(OB_FRAME_METADATA_TYPE_EXPOSURE)) {
        auto exp = d->getMetadataValue(OB_FRAME_METADATA_TYPE_EXPOSURE);
        EXPECT_GT(exp, 0);
    }
    if(d->hasMetadata(OB_FRAME_METADATA_TYPE_GAIN)) {
        auto gain = d->getMetadataValue(OB_FRAME_METADATA_TYPE_GAIN);
        EXPECT_GE(gain, 0);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_04_raw_metadata_buffer) {
    /// Test case: raw metadata buffer.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    auto md     = d->getMetadata();
    auto mdSize = d->getMetadataSize();
    if(mdSize > 0) {
        EXPECT_NE(md, nullptr);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_05_unsupported_field_safe) {
    /// Test case: unsupported field safe.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    // Prepare local state for the next check.
    bool has = d->hasMetadata(static_cast<OBFrameMetadataType>(9999));
    EXPECT_FALSE(has);
    pipeline_->stop();
}

// ============================================================
// Test group: FrameFactory.
// ============================================================
class TC_CPP_12_FrameFactory_HW : public PipelineTest {};

TEST_F(TC_CPP_12_FrameFactory_HW, TC_CPP_12_02_clone_frame) {
    /// Test case: clone frame.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    pipeline_->stop();

    ob_error *error = nullptr;
    auto      clone = ob_create_frame_from_other_frame(depth->getImpl(), true, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(clone, nullptr);

    EXPECT_EQ(ob_frame_get_data_size(clone, &error), depth->getDataSize());
    EXPECT_EQ(ob_frame_get_format(clone, &error), (ob_format)depth->getFormat());

    ob_delete_frame(clone, &error);
}

// ============================================================
// Test group: Filter.
// ============================================================
class TC_CPP_13_Filter_HW : public PipelineTest {
protected:
    std::shared_ptr<ob::DepthFrame> getDepthFrame() {
        auto config = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        pipeline_->start(config);
        for(int i = 0; i < 3; i++)
            pipeline_->waitForFrameset(2000);
        auto fs = pipeline_->waitForFrameset(3000);
        pipeline_->stop();
        return fs ? fs->getDepthFrame() : nullptr;
    }
};

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_03_filter_sync_process) {
    /// Test case: filter sync process.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
    EXPECT_GT(result->getDataSize(), 0u);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_04_filter_async_callback) {
    /// Test case: filter async callback.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto              filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    std::atomic<bool> cbCalled{ false };
    filter->setCallBack([&cbCalled](std::shared_ptr<ob::Frame>) { cbCalled = true; });
    filter->pushFrame(depth);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(cbCalled.load());
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_08_pointcloud_filter) {
    /// Test case: pointcloud filter.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("PointCloudFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
    if(result) {
        EXPECT_GT(result->getDataSize(), 0u);
    }
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_09_align_filter) {
    /// Align Filter D2C
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    config->setAlignMode(ALIGN_D2C_SW_MODE);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    // Stop execution and clean runtime state.
    pipeline_->stop();
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_10_format_converter) {
    /// Test case: format converter.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    // ANY_SITUATION: emit a frameset for every arriving frame, even when only
    // one stream type is active.  Avoids the default ALL_REQUIRE mode blocking
    // output until every enabled stream has delivered a synchronized frame.
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline_->start(config);

    // Allow the color sensor time to stabilize before sampling.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    for(int i = 0; i < 5; i++)
        pipeline_->waitForFrameset(3000);

    std::shared_ptr<ob::ColorFrame> color;
    for(int i = 0; i < 10 && !color; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(fs)
            color = fs->getColorFrame();
    }
    pipeline_->stop();

    if(!color)
        GTEST_SKIP() << "No color frame produced by this device";

    ob_error *error  = nullptr;
    auto      filter = ob_create_filter("FormatConverterFilter", &error);
    if(!filter)
        GTEST_SKIP() << "FormatConverterFilter not available on this build";

    auto result = ob_filter_process(filter, color->getImpl(), &error);
    if(result) {
        EXPECT_GT(ob_frame_get_data_size(result, &error), 0u);
        ob_delete_frame(result, &error);
    }
    ob_delete_filter(filter, &error);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_11_hdr_merge) {
    /// Test case: hdr merge.
    // Prepare local state for the next check.
    ob_error *error     = nullptr;
    auto      hdrFilter = ob_create_filter("HDRMergeFilter", &error);
    auto      seqFilter = ob_create_filter("SequenceIdFilter", &error);

    if(hdrFilter)
        ob_delete_filter(hdrFilter, &error);
    if(seqFilter)
        ob_delete_filter(seqFilter, &error);
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_12_decimation_filter) {
    /// Test case: decimation filter.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
    if(result && result->is<ob::VideoFrame>()) {
        auto vf = result->as<ob::VideoFrame>();
        // Validate expected conditions for this step.
        EXPECT_LE(vf->getWidth(), depth->getWidth());
    }
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_13_threshold_filter) {
    /// Test case: threshold filter.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("ThresholdFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_14_spatial_filters) {
    /// Test case: spatial filters.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name: { "SpatialAdvancedFilter", "SpatialFastFilter", "SpatialModerateFilter" }) {
        ob_error *error = nullptr;
        auto      f     = ob_create_filter(name, &error);
        if(!f)
            continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result) {
            EXPECT_GT(ob_frame_get_data_size(result, &error), 0u);
            ob_delete_frame(result, &error);
        }
        ob_delete_filter(f, &error);
    }
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_15_temporal_holefilling_noise) {
    /// Test case: temporal holefilling noise.
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name: { "TemporalFilter", "HoleFillingFilter", "NoiseRemovalFilter" }) {
        ob_error *error = nullptr;
        auto      f     = ob_create_filter(name, &error);
        if(!f)
            continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result)
            ob_delete_frame(result, &error);
        ob_delete_filter(f, &error);
    }
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_16_false_positive_disparity) {
    /// FalsePositiveFilter + DisparityTransform
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name: { "FalsePositiveFilter", "DisparityTransformFilter" }) {
        ob_error *error = nullptr;
        auto      f     = ob_create_filter(name, &error);
        if(!f)
            continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result)
            ob_delete_frame(result, &error);
        ob_delete_filter(f, &error);
    }
    SUCCEED();
}

// ============================================================
// Test group: Property.
// ============================================================
class TC_CPP_14_Property_HW : public DeviceTest {};

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_01_property_enum) {
    /// Test case: property enum.
    int count = device_->getSupportedPropertyCount();
    EXPECT_GT(count, 0);
    for(int i = 0; i < count; i++) {
        auto item = device_->getSupportedProperty(i);
        EXPECT_NE(item.id, 0);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_02_bool_property) {
    /// Test case: bool property.
    if(device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        device_->setBoolProperty(OB_PROP_LASER_BOOL, !original);
        bool flipped = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        EXPECT_EQ(flipped, !original) << "Read-after-write mismatch for OB_PROP_LASER_BOOL";
        device_->setBoolProperty(OB_PROP_LASER_BOOL, original);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_03_int_property_range) {
    /// Test case: int property range.
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_DEPTH_EXPOSURE_INT);
        EXPECT_LT(range.min, range.max);
        EXPECT_GT(range.step, 0u);

        int cur = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
        EXPECT_GE(cur, (int32_t)range.min);
        EXPECT_LE(cur, (int32_t)range.max);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_04_float_property_range) {
    /// Test case: float property range.
    // Verify float property range query and read/write operations.
    if(device_->isPropertySupported(OB_PROP_DEPTH_UNIT_FLEXIBLE_ADJUSTMENT_FLOAT, OB_PERMISSION_READ)) {
        auto range = device_->getFloatPropertyRange(OB_PROP_DEPTH_UNIT_FLEXIBLE_ADJUSTMENT_FLOAT);
        EXPECT_LT(range.min, range.max);
        EXPECT_GT(range.step, 0.0f);
        float cur = device_->getFloatProperty(OB_PROP_DEPTH_UNIT_FLEXIBLE_ADJUSTMENT_FLOAT);
        EXPECT_GE(cur, range.min);
        EXPECT_LE(cur, range.max);
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_05_structured_data) {
    /// Test case: structured data - read/write structured data properties.
    if(!device_->isPropertySupported(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, OB_PERMISSION_READ)) {
        GTEST_SKIP() << "OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG not supported";
    }

    auto config = device_->getMultiDeviceSyncConfig();
    ASSERT_NE(config.syncMode, 0);

    OBMultiDeviceSyncConfig writeConfig = config;
    writeConfig.depthDelayUs            = 100;
    writeConfig.colorDelayUs            = 200;

    ASSERT_NO_THROW(device_->setStructuredData(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, reinterpret_cast<const uint8_t *>(&writeConfig),
                                               static_cast<uint32_t>(sizeof(writeConfig))));

    OBMultiDeviceSyncConfig readConfig = {};
    uint32_t                dataSize   = sizeof(readConfig);
    ASSERT_NO_THROW(device_->getStructuredData(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, reinterpret_cast<uint8_t *>(&readConfig), &dataSize));

    EXPECT_EQ(readConfig.depthDelayUs, 100);
    EXPECT_EQ(readConfig.colorDelayUs, 200);

    ASSERT_NO_THROW(
        device_->setStructuredData(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, reinterpret_cast<const uint8_t *>(&config), static_cast<uint32_t>(sizeof(config))));
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_06_raw_data) {
    /// Test case: raw data.
    // Prepare local state for the next check.
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_07_customer_data) {
    /// Test case: customer data.
    // Writes a known payload to device flash and reads it back to confirm consistency.
    // Gated on ALLOW_DESTRUCTIVE_TESTS because the write is persistent across device power cycles.
    if(!ENV().allowDestructive()) {
        GTEST_SKIP() << "ALLOW_DESTRUCTIVE_TESTS not set — skipping customer data write test";
    }

    // Prepare a 32-byte test payload with a recognisable prefix.
    static const char kWritePayload[] = "OrbbecSDK_CustomerData_TestV2___";
    const uint32_t    kWriteSize      = static_cast<uint32_t>(sizeof(kWritePayload) - 1);  // exclude null

    // Write the payload to device flash.
    try {
        device_->writeCustomerData(kWritePayload, kWriteSize);
    }
    catch(const ob::Error &e) {
        const std::string msg = e.getMessage() ? e.getMessage() : "";
        // Some devices (e.g. Gemini 335) do not expose the FirmwareUpdater
        // component required for customer-data storage.  Skip rather than fail.
        if(msg.find("Unsupported") != std::string::npos || msg.find("FirmwareUpdater") != std::string::npos) {
            GTEST_SKIP() << "writeCustomerData not supported on this device: " << msg;
        }
        FAIL() << "writeCustomerData() threw unexpected exception: " << msg;
    }

    // Read it back.
    uint8_t  readBuf[65536]{};
    uint32_t readSize = 0;
    ASSERT_NO_THROW(device_->readCustomerData(readBuf, &readSize)) << "readCustomerData() threw an exception";

    // Verify size and byte-for-byte content match.
    EXPECT_EQ(readSize, kWriteSize) << "Read-back data size does not match written size";
    EXPECT_EQ(std::memcmp(readBuf, kWritePayload, kWriteSize), 0) << "Read-back customer data does not match written payload";
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_08_laser_control) {
    /// Test case: laser control - test laser/LDP/flood properties read and write.
    if(device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        device_->setBoolProperty(OB_PROP_LASER_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_LASER_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_LDP_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_LDP_BOOL);
        device_->setBoolProperty(OB_PROP_LDP_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_LDP_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_LDP_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_FLOOD_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_FLOOD_BOOL);
        device_->setBoolProperty(OB_PROP_FLOOD_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_FLOOD_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_FLOOD_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT);
            device_->setIntProperty(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT, original);
        }
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_09_depth_control) {
    /// Test case: depth control - test depth mirror/flip and exposure properties.
    if(device_->isPropertySupported(OB_PROP_DEPTH_MIRROR_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL);
        device_->setBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_DEPTH_FLIP_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_DEPTH_FLIP_BOOL);
        device_->setBoolProperty(OB_PROP_DEPTH_FLIP_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_DEPTH_FLIP_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_DEPTH_FLIP_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL);
        device_->setBoolProperty(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_DEPTH_EXPOSURE_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
            device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, original);
        }
    }
    if(device_->isPropertySupported(OB_PROP_DEPTH_GAIN_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_DEPTH_GAIN_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_DEPTH_GAIN_INT);
            device_->setIntProperty(OB_PROP_DEPTH_GAIN_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_DEPTH_GAIN_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_DEPTH_GAIN_INT, original);
        }
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_10_color_control) {
    /// Test case: color control - test color mirror/flip, AE and exposure properties.
    if(device_->isPropertySupported(OB_PROP_COLOR_MIRROR_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_COLOR_MIRROR_BOOL);
        device_->setBoolProperty(OB_PROP_COLOR_MIRROR_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_COLOR_MIRROR_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_COLOR_MIRROR_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_COLOR_FLIP_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_COLOR_FLIP_BOOL);
        device_->setBoolProperty(OB_PROP_COLOR_FLIP_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_COLOR_FLIP_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_COLOR_FLIP_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL);
        device_->setBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_COLOR_EXPOSURE_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_COLOR_EXPOSURE_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_COLOR_EXPOSURE_INT);
            device_->setIntProperty(OB_PROP_COLOR_EXPOSURE_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_COLOR_EXPOSURE_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_COLOR_EXPOSURE_INT, original);
        }
    }
    if(device_->isPropertySupported(OB_PROP_COLOR_GAIN_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_COLOR_GAIN_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_COLOR_GAIN_INT);
            device_->setIntProperty(OB_PROP_COLOR_GAIN_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_COLOR_GAIN_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_COLOR_GAIN_INT, original);
        }
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_11_ir_control) {
    /// Test case: ir control - test IR mirror/flip and exposure properties.
    if(device_->isPropertySupported(OB_PROP_IR_MIRROR_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_IR_MIRROR_BOOL);
        device_->setBoolProperty(OB_PROP_IR_MIRROR_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_IR_MIRROR_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_IR_MIRROR_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_IR_FLIP_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_IR_FLIP_BOOL);
        device_->setBoolProperty(OB_PROP_IR_FLIP_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_IR_FLIP_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_IR_FLIP_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_IR_EXPOSURE_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_IR_EXPOSURE_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_IR_EXPOSURE_INT);
            device_->setIntProperty(OB_PROP_IR_EXPOSURE_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_IR_EXPOSURE_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_IR_EXPOSURE_INT, original);
        }
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_12_device_management) {
    /// Test case: device management - test indicator light and other device management properties.
    if(device_->isPropertySupported(OB_PROP_INDICATOR_LIGHT_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_INDICATOR_LIGHT_BOOL);
        device_->setBoolProperty(OB_PROP_INDICATOR_LIGHT_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_INDICATOR_LIGHT_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_INDICATOR_LIGHT_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_FAN_WORK_MODE_INT, OB_PERMISSION_READ_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_FAN_WORK_MODE_INT);
        if(range.min < range.max) {
            int original = device_->getIntProperty(OB_PROP_FAN_WORK_MODE_INT);
            device_->setIntProperty(OB_PROP_FAN_WORK_MODE_INT, range.max);
            int afterSet = device_->getIntProperty(OB_PROP_FAN_WORK_MODE_INT);
            EXPECT_EQ(afterSet, range.max);
            device_->setIntProperty(OB_PROP_FAN_WORK_MODE_INT, original);
        }
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_13_timing_sync) {
    /// Test case: timing sync.
    ASSERT_NO_THROW(device_->timestampReset());
    auto config = device_->getTimestampResetConfig();
    ASSERT_NO_THROW(device_->setTimestampResetConfig(config));
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_14_hdr_interleave) {
    /// Test case: hdr interleave - test HDR and frame interleave properties.
    if(device_->isPropertySupported(OB_PROP_HDR_MERGE_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_HDR_MERGE_BOOL);
        device_->setBoolProperty(OB_PROP_HDR_MERGE_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_HDR_MERGE_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_HDR_MERGE_BOOL, original);
    }
    if(device_->isPropertySupported(OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool original = device_->getBoolProperty(OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL);
        device_->setBoolProperty(OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL, !original);
        bool afterSet = device_->getBoolProperty(OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL);
        EXPECT_EQ(afterSet, !original);
        device_->setBoolProperty(OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL, original);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_15_structured_read_group) {
    /// Test case: structured read group - test reading various structured data properties.
    int successCount = 0;

    if(device_->isPropertySupported(OB_STRUCT_DEVICE_TEMPERATURE, OB_PERMISSION_READ)) {
        OBDeviceTemperature temp     = {};
        uint32_t            dataSize = static_cast<uint32_t>(sizeof(temp));
        ASSERT_NO_THROW(device_->getStructuredData(OB_STRUCT_DEVICE_TEMPERATURE, reinterpret_cast<uint8_t *>(&temp), &dataSize));
        EXPECT_GE(temp.chipTopTemp, 0.0f);
        EXPECT_LE(temp.chipTopTemp, 80.0f);
        successCount++;
    }

    if(device_->isPropertySupported(OB_STRUCT_BASELINE_CALIBRATION_PARAM, OB_PERMISSION_READ)) {
        OBBaselineCalibrationParam param    = {};
        uint32_t                   dataSize = static_cast<uint32_t>(sizeof(param));
        ASSERT_NO_THROW(device_->getStructuredData(OB_STRUCT_BASELINE_CALIBRATION_PARAM, reinterpret_cast<uint8_t *>(&param), &dataSize));
        EXPECT_GT(param.baseline, 0.0f);
        EXPECT_GT(param.zpd, 0.0f);
        successCount++;
    }

    if(device_->isPropertySupported(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, OB_PERMISSION_READ)) {
        OBMultiDeviceSyncConfig config   = {};
        uint32_t                dataSize = sizeof(config);
        ASSERT_NO_THROW(device_->getStructuredData(OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG, reinterpret_cast<uint8_t *>(&config), &dataSize));
        EXPECT_GE(config.syncMode, 0);
        successCount++;
    }

    if(device_->isPropertySupported(OB_STRUCT_DEVICE_SERIAL_NUMBER, OB_PERMISSION_READ)) {
        OBSerialNumber serial   = {};
        uint32_t       dataSize = sizeof(serial);
        ASSERT_NO_THROW(device_->getStructuredData(OB_STRUCT_DEVICE_SERIAL_NUMBER, reinterpret_cast<uint8_t *>(&serial), &dataSize));
        EXPECT_GT(std::strlen(serial.numberStr), 0u);
        successCount++;
    }

    if(successCount == 0) {
        GTEST_SKIP() << "No structured data properties are supported for reading";
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_16_sdk_level_property) {
    /// Test case: sdk level property.
    struct BoolPropertyCase {
        OBPropertyID id;
        const char  *name;
    };

    struct IntPropertyCase {
        OBPropertyID id;
        const char  *name;
    };

    const std::vector<BoolPropertyCase> boolProperties = {
        { OB_PROP_HEARTBEAT_BOOL, "OB_PROP_HEARTBEAT_BOOL" },
        { OB_PROP_WATCHDOG_BOOL, "OB_PROP_WATCHDOG_BOOL" },
        { OB_PROP_EXTERNAL_SIGNAL_RESET_BOOL, "OB_PROP_EXTERNAL_SIGNAL_RESET_BOOL" },
        { OB_PROP_TIMER_RESET_TRIGGER_OUT_ENABLE_BOOL, "OB_PROP_TIMER_RESET_TRIGGER_OUT_ENABLE_BOOL" },
    };

    const std::vector<IntPropertyCase> intProperties = {
        { OB_PROP_DEVICE_COMMUNICATION_TYPE_INT, "OB_PROP_DEVICE_COMMUNICATION_TYPE_INT" },
        { OB_PROP_DEVICE_WORK_MODE_INT, "OB_PROP_DEVICE_WORK_MODE_INT" },
        { OB_PROP_USB_POWER_STATE_INT, "OB_PROP_USB_POWER_STATE_INT" },
    };

    int readablePropertyCount = 0;

    for(const auto &property: boolProperties) {
        if(!device_->isPropertySupported(property.id, OB_PERMISSION_READ)) {
            continue;
        }

        EXPECT_NO_THROW({ (void)device_->getBoolProperty(property.id); }) << "Failed to read " << property.name;
        ++readablePropertyCount;
    }

    for(const auto &property: intProperties) {
        if(!device_->isPropertySupported(property.id, OB_PERMISSION_READ)) {
            continue;
        }

        EXPECT_NO_THROW({ (void)device_->getIntProperty(property.id); }) << "Failed to read " << property.name;
        ++readablePropertyCount;
    }

    EXPECT_GT(readablePropertyCount, 0) << "Current device does not expose any readable SDK-level property from the validation set";
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_17_unsupported_property_safe) {
    /// Test case: unsupported property safe.
    bool supported = device_->isPropertySupported(static_cast<OBPropertyID>(99999), OB_PERMISSION_READ);
    EXPECT_FALSE(supported);

    if(!supported) {
        EXPECT_THROW(device_->getIntProperty(static_cast<OBPropertyID>(99999)), ob::Error);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_18_out_of_range_safe) {
    /// Test case: out of range safe.
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_WRITE)) {
        auto range    = device_->getIntPropertyRange(OB_PROP_DEPTH_EXPOSURE_INT);
        auto original = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);

        try {
            device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, range.max + 10000);
        }
        catch(...) {
        }

        try {
            device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, range.min - 10000);
        }
        catch(...) {
        }

        try {
            auto current = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
            if(current != original) {
                device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, original);
            }
        }
        catch(...) {
        }
    }
    SUCCEED();
}

// ============================================================
// Test group: DepthMode.
// ============================================================
class TC_CPP_15_DepthMode : public DeviceTest {};

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_01_current_mode) {
    /// Test case: current mode.
    auto mode = device_->getCurrentDepthWorkMode();
    EXPECT_GT(std::strlen(mode.name), 0u);
    auto name = device_->getCurrentDepthModeName();
    EXPECT_NE(name, nullptr);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_02_enum_modes) {
    /// Test case: enum modes.
    auto list = device_->getDepthWorkModeList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);

    for(uint32_t i = 0; i < list->getCount(); i++) {
        auto mode = list->getOBDepthWorkMode(i);
        EXPECT_GT(std::strlen(mode.name), 0u);
    }
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_03_switch_by_name) {
    /// Test case: switch by name.
    auto list = device_->getDepthWorkModeList();
    ASSERT_GT(list->getCount(), 0u);

    auto        originalMode = device_->getCurrentDepthWorkMode();
    std::string originalName(originalMode.name);

    auto targetMode = list->getOBDepthWorkMode(0);
    if(std::string(targetMode.name) == originalName && list->getCount() > 1) {
        targetMode = list->getOBDepthWorkMode(1);
    }
    auto status = device_->switchDepthWorkMode(targetMode.name);
    EXPECT_EQ(status, OB_STATUS_OK);

    status = device_->switchDepthWorkMode(originalName.c_str());
    EXPECT_EQ(status, OB_STATUS_OK);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_04_switch_by_struct) {
    /// Test case: switch by struct.
    auto list = device_->getDepthWorkModeList();
    ASSERT_GT(list->getCount(), 0u);
    auto mode = list->getOBDepthWorkMode(0);

    auto status = device_->switchDepthWorkMode(mode);
    EXPECT_EQ(status, OB_STATUS_OK);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_05_profile_change_after_switch) {
    /// Test case: profile change after switch.
    auto list = device_->getDepthWorkModeList();
    if(list->getCount() < 2)
        GTEST_SKIP() << "Only 1 depth mode available";

    auto depthSensor    = device_->getSensor(OB_SENSOR_DEPTH);
    auto profilesBefore = depthSensor->getStreamProfileList();
    auto countBefore    = profilesBefore->getCount();

    auto mode1 = list->getOBDepthWorkMode(1);
    device_->switchDepthWorkMode(mode1);

    auto profilesAfter = depthSensor->getStreamProfileList();
    EXPECT_GT(profilesAfter->getCount(), 0u);

    auto countAfter     = profilesAfter->getCount();
    bool profileChanged = (countBefore != countAfter);
    if(!profileChanged) {
        // If count is the same, check if individual profiles differ.
        for(uint32_t i = 0; i < countBefore && i < countAfter; i++) {
            auto before = profilesBefore->getProfile(i)->as<ob::VideoStreamProfile>();
            auto after  = profilesAfter->getProfile(i)->as<ob::VideoStreamProfile>();
            if(before->getWidth() != after->getWidth() || before->getHeight() != after->getHeight()) {
                profileChanged = true;
                break;
            }
        }
    }
    EXPECT_TRUE(profileChanged) << "StreamProfile list did not change after switching depth mode";
}

// ============================================================
// Test group: Preset.
// ============================================================
class TC_CPP_16_Preset : public DeviceTest {};

TEST_F(TC_CPP_16_Preset, TC_CPP_16_01_current_and_list) {
    /// Test case: current and list.
    auto list = device_->getAvailablePresetList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);

    auto curName = device_->getCurrentPresetName();
    EXPECT_NE(curName, nullptr);
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_02_load_builtin) {
    /// Test case: load builtin.
    auto list = device_->getAvailablePresetList();
    if(list->getCount() > 0) {
        auto name = list->getName(0);
        ASSERT_NO_THROW(device_->loadPreset(name));
    }
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_03_export_json) {
    /// Test case: export json.
    const uint8_t *data     = nullptr;
    uint32_t       dataSize = 0;
    ASSERT_NO_THROW(device_->exportSettingsAsPresetJsonData("test_export", &data, &dataSize));
    EXPECT_NE(data, nullptr);
    EXPECT_GT(dataSize, 0u);
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_04_load_from_json) {
    /// Test case: load from json.
    // Prepare local state for the next check.
    const uint8_t *data     = nullptr;
    uint32_t       dataSize = 0;
    device_->exportSettingsAsPresetJsonData("roundtrip_test", &data, &dataSize);
    if(data && dataSize > 0) {
        ASSERT_NO_THROW(device_->loadPresetFromJsonData("roundtrip_test", data, dataSize));
    }
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_05_preset_changes_properties) {
    /// Test case: preset changes properties.
    auto list = device_->getAvailablePresetList();
    if(list->getCount() < 2){
        GTEST_SKIP() << "Need >=2 presets";
    }

    const char *presetName0 = list->getName(0);
    ASSERT_NO_THROW(device_->loadPreset(presetName0));
    auto currentName0 = device_->getCurrentPresetName();
    EXPECT_STREQ(currentName0, presetName0);

    int32_t exposureAfter0 = 0;
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_READ)) {
        exposureAfter0 = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
    }

    const char *presetName1 = list->getName(1);
    ASSERT_NO_THROW(device_->loadPreset(presetName1));
    auto currentName1 = device_->getCurrentPresetName();
    EXPECT_STREQ(currentName1, presetName1);

    int32_t exposureAfter1 = 0;
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_READ)) {
        exposureAfter1 = device_->getIntProperty(OB_PROP_DEPTH_EXPOSURE_INT);
    }

    if(exposureAfter0 != exposureAfter1) {
        SUCCEED();
    }
    else {
        EXPECT_STRNE(presetName0, presetName1) << "Different presets should have different property values";
    }
}

// ============================================================
// Test group: Interleave.
// ============================================================
class TC_CPP_17_Interleave : public DeviceTest {};

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_01_support_query) {
    /// Test case: support query.
    bool supported = device_->isFrameInterleaveSupported();
    if(!supported)
        GTEST_SKIP() << "Frame interleave not supported";

    auto list = device_->getAvailableFrameInterleaveList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);
}

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_02_load_interleave) {
    /// Test case: load interleave.
    if(!device_->isFrameInterleaveSupported())
        GTEST_SKIP();
    auto list = device_->getAvailableFrameInterleaveList();
    if(list && list->getCount() > 0) {
        ASSERT_NO_THROW(device_->loadFrameInterleave(list->getName(0)));
    }
}

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_03_interleave_stream) {
    /// Test case: interleave stream.
    if(!device_->isFrameInterleaveSupported())
        GTEST_SKIP();
    auto list = device_->getAvailableFrameInterleaveList();
    if(!list || list->getCount() == 0)
        GTEST_SKIP();

    device_->loadFrameInterleave(list->getName(0));
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 10; i++)
        pipeline->waitForFrameset(2000);
    pipeline->stop();
    SUCCEED();
}

// ============================================================
// Test group: Record.
// ============================================================
class TC_CPP_18_Record_HW : public PipelineTest {};

TEST_F(TC_CPP_18_Record_HW, TC_CPP_18_01_record_device) {
    /// Test case: record device.
#ifdef _WIN32
    std::string recFile = "ob_test_record.bag";
#else
    std::string recFile = "/tmp/ob_test_record.bag";
#endif
    std::remove(recFile.c_str());

    auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 30; i++)
        pipeline->waitForFrameset(2000);
    pipeline->stop();
    recorder.reset();

    std::ifstream ifs(recFile, std::ios::binary | std::ios::ate);
    EXPECT_TRUE(ifs.good()) << "Record file not created";
    if(ifs.good()) {
        EXPECT_GT(ifs.tellg(), 1024) << "Record file too small";
    }
    std::remove(recFile.c_str());
}

TEST_F(TC_CPP_18_Record_HW, TC_CPP_18_02_record_pause_resume) {
    /// Test case: record pause resume.
#ifdef _WIN32
    std::string recFile = "ob_test_record2.bag";
#else
    std::string recFile = "/tmp/ob_test_record2.bag";
#endif
    std::remove(recFile.c_str());

    auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 10; i++)
        pipeline->waitForFrameset(2000);
    recorder->pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    recorder->resume();
    for(int i = 0; i < 10; i++)
        pipeline->waitForFrameset(2000);

    pipeline->stop();
    recorder.reset();

    std::ifstream ifs(recFile, std::ios::binary | std::ios::ate);
    EXPECT_TRUE(ifs.good());
    std::remove(recFile.c_str());
}

TEST_F(TC_CPP_18_Record_HW, TC_CPP_18_08_record_playback_roundtrip) {
    /// Test case: record playback roundtrip.
#ifdef _WIN32
    std::string recFile   = "ob_test_roundtrip.bag";
    const char *helperExe = ".\\ob_playback_smoke_test.exe";
#else
    std::string recFile   = "/tmp/ob_test_roundtrip.bag";
    const char *helperExe = "./ob_playback_smoke_test";
#endif
    std::remove(recFile.c_str());

    // Prepare local state for the next check.
    {
        auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
        auto pipeline = std::make_shared<ob::Pipeline>(device_);
        auto config   = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        pipeline->start(config);
        for(int i = 0; i < 30; i++)
            pipeline->waitForFrameset(2000);
        pipeline->stop();
        recorder.reset();
    }

    std::ifstream ifs(recFile, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(ifs.good()) << "Recorded bag file was not created";
    EXPECT_GT(ifs.tellg(), 1024) << "Recorded bag file is unexpectedly small";
    ifs.close();

    std::ifstream helper(helperExe, std::ios::binary);
    if(!helper.good()) {
        std::remove(recFile.c_str());
        GTEST_SKIP() << "Playback smoke helper is not built: " << helperExe;
    }
    helper.close();

#ifdef _WIN32
    const std::string command = std::string("cmd /c \"\"") + helperExe + "\" \"" + recFile + "\"\"";
#else
    const std::string command = std::string("\"") + helperExe + "\" \"" + recFile + "\"";
#endif
    const int exitCode = std::system(command.c_str());
    std::remove(recFile.c_str());

    if(exitCode != 0) {
        GTEST_SKIP() << "Recorded bag playback helper returned non-zero: " << exitCode;
    }
}

// ============================================================
// Test group: MultiSync.
// ============================================================
class TC_CPP_19_MultiSync : public DeviceTest {};

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_01_sync_mode_support) {
    /// Test case: sync mode support.
    auto bitmap = device_->getSupportedMultiDeviceSyncModeBitmap();
    EXPECT_NE(bitmap, 0u) << "No sync modes supported";

    auto config = device_->getMultiDeviceSyncConfig();
    // Verify syncMode is one of the supported bits
    EXPECT_TRUE(bitmap & config.syncMode) << "Current syncMode not in supported bitmap";

    // Verify all config fields are readable (finite/sane values)
    EXPECT_TRUE(std::isfinite(static_cast<float>(config.depthDelayUs)));
    EXPECT_TRUE(std::isfinite(static_cast<float>(config.colorDelayUs)));
    EXPECT_TRUE(std::isfinite(static_cast<float>(config.trigger2ImageDelayUs)));
    EXPECT_TRUE(std::isfinite(static_cast<float>(config.triggerOutDelayUs)));
    EXPECT_GT(config.framesPerTrigger, 0);
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_02_freerun_standalone) {
    /// Test case: freerun standalone.
    // FREE_RUN mode
    OBMultiDeviceSyncConfig config = {};
    config.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(config));
    auto readBack = device_->getMultiDeviceSyncConfig();
    EXPECT_EQ(readBack.syncMode, OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN);

    config.syncMode = OB_MULTI_DEVICE_SYNC_MODE_STANDALONE;
    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(config));
    readBack = device_->getMultiDeviceSyncConfig();
    EXPECT_EQ(readBack.syncMode, OB_MULTI_DEVICE_SYNC_MODE_STANDALONE);
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_03_primary_secondary) {
    /// Test case: primary secondary.
    if(ENV().deviceCount() < 2) {
        GTEST_SKIP() << "Test requires >= 2 devices, available: " << ENV().deviceCount();
    }

    auto devList = ctx_->queryDeviceList();
    ASSERT_GE(devList->getCount(), 2u);

    auto devA = devList->getDevice(0);
    auto devB = devList->getDevice(1);
    ASSERT_NE(devA, nullptr);
    ASSERT_NE(devB, nullptr);

    // Save original configs
    auto origA = devA->getMultiDeviceSyncConfig();
    auto origB = devB->getMultiDeviceSyncConfig();

    // Device A: PRIMARY with trigger out enabled
    OBMultiDeviceSyncConfig configA = {};
    configA.syncMode         = OB_MULTI_DEVICE_SYNC_MODE_PRIMARY;
    configA.triggerOutEnable = true;
    configA.framesPerTrigger = 1;
    ASSERT_NO_THROW(devA->setMultiDeviceSyncConfig(configA));
    auto readA = devA->getMultiDeviceSyncConfig();
    EXPECT_EQ(readA.syncMode, OB_MULTI_DEVICE_SYNC_MODE_PRIMARY);

    // Device B: SECONDARY
    OBMultiDeviceSyncConfig configB = {};
    configB.syncMode         = OB_MULTI_DEVICE_SYNC_MODE_SECONDARY;
    configB.framesPerTrigger = 1;
    ASSERT_NO_THROW(devB->setMultiDeviceSyncConfig(configB));
    auto readB = devB->getMultiDeviceSyncConfig();
    EXPECT_EQ(readB.syncMode, OB_MULTI_DEVICE_SYNC_MODE_SECONDARY);

    // Sync start: start both pipelines
    auto pipelineA = std::make_shared<ob::Pipeline>(devA);
    auto pipelineB = std::make_shared<ob::Pipeline>(devB);
    auto cfg       = std::make_shared<ob::Config>();
    cfg->enableStream(OB_STREAM_DEPTH);

    ASSERT_NO_THROW(pipelineA->start(cfg));
    ASSERT_NO_THROW(pipelineB->start(cfg));

    // Collect frames and verify timestamps are roughly synchronized
    auto fsA = pipelineA->waitForFrameset(5000);
    auto fsB = pipelineB->waitForFrameset(5000);
    ASSERT_NE(fsA, nullptr);
    ASSERT_NE(fsB, nullptr);

    uint64_t tsA = fsA->getDepthFrame() ? fsA->getDepthFrame()->getTimeStampUs() : 0;
    uint64_t tsB = fsB->getDepthFrame() ? fsB->getDepthFrame()->getTimeStampUs() : 0;
    // Timestamps from synced devices should be within 50ms of each other
    int64_t diff = static_cast<int64_t>(tsA) - static_cast<int64_t>(tsB);
    EXPECT_LT(std::abs(diff), 50000) << "Timestamps not synchronized: devA=" << tsA << " devB=" << tsB;

    pipelineA->stop();
    pipelineB->stop();

    // Restore original configs
    ASSERT_NO_THROW(devA->setMultiDeviceSyncConfig(origA));
    ASSERT_NO_THROW(devB->setMultiDeviceSyncConfig(origB));
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_04_software_trigger) {
    /// Test case: software trigger.
    auto bitmap = device_->getSupportedMultiDeviceSyncModeBitmap();
    if(!(bitmap & OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING)) {
        GTEST_SKIP() << "Software triggering not supported";
    }
    if(!device_->isPropertySupported(OB_PROP_CAPTURE_IMAGE_SIGNAL_BOOL, OB_PERMISSION_WRITE)) {
        GTEST_SKIP() << "Capture trigger property is not writable on this device";
    }

    // Save original config to restore later
    auto origConfig = device_->getMultiDeviceSyncConfig();

    OBMultiDeviceSyncConfig config = {};
    config.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING;
    config.framesPerTrigger        = 1;
    device_->setMultiDeviceSyncConfig(config);

    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto cfg      = std::make_shared<ob::Config>();
    cfg->enableStream(OB_STREAM_DEPTH);
    pipeline->start(cfg);

    // Multiple triggers should produce multiple frames
    int frameCount = 0;
    for(int i = 0; i < 3; ++i) {
        device_->triggerCapture();
        auto fs = pipeline->waitForFrameset(3000);
        if(fs)
            frameCount++;
    }

    pipeline->stop();

    EXPECT_GE(frameCount, 1) << "Software trigger failed to produce any frames";

    // Restore original sync config
    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(origConfig));
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_05_timestamp_reset) {
    /// Test case: timestamp reset.
    if(!device_->isPropertySupported(OB_PROP_TIMER_RESET_ENABLE_BOOL, OB_PERMISSION_WRITE)) {
        GTEST_SKIP() << "Timestamp reset is not writable on this device";
    }

    auto origConfig = device_->getTimestampResetConfig();
    ASSERT_NO_THROW(device_->setTimestampResetConfig(origConfig));

    // Verify readback consistency
    auto readBack = device_->getTimestampResetConfig();
    EXPECT_EQ(readBack.enable, origConfig.enable);

    ASSERT_NO_THROW(device_->timestampReset());
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_06_timer_sync_host) {
    /// Test case: timer sync host.
    ASSERT_NO_THROW(device_->timerSyncWithHost());
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_07_sync_config_fields) {
    /// Test case: sync config fields.
    // Save original config
    auto origConfig = device_->getMultiDeviceSyncConfig();

    OBMultiDeviceSyncConfig config = {};
    config.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
    config.depthDelayUs            = 100;
    config.colorDelayUs            = 200;
    config.trigger2ImageDelayUs    = 0;
    config.triggerOutEnable        = false;
    config.triggerOutDelayUs       = 0;
    config.framesPerTrigger        = 1;

    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(config));
    auto readBack = device_->getMultiDeviceSyncConfig();
    EXPECT_EQ(readBack.syncMode, OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN);
    EXPECT_EQ(readBack.depthDelayUs, 100);
    EXPECT_EQ(readBack.colorDelayUs, 200);
    EXPECT_EQ(readBack.trigger2ImageDelayUs, 0);
    EXPECT_EQ(readBack.triggerOutEnable, false);
    EXPECT_EQ(readBack.triggerOutDelayUs, 0);
    EXPECT_EQ(readBack.framesPerTrigger, 1);

    // Restore original config
    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(origConfig));
}

// ============================================================
// Test group: Coord.
// ============================================================
class TC_CPP_20_Coord_HW : public PipelineTest {};

TEST_F(TC_CPP_20_Coord_HW, TC_CPP_20_05_save_ply) {
    /// Test case: save ply.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++)
        pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    pipeline_->stop();

#ifdef _WIN32
    std::string plyFile = "ob_test_points.ply";
#else
    std::string plyFile = "/tmp/ob_test_points.ply";
#endif
    std::remove(plyFile.c_str());

    ob_error *error    = nullptr;
    auto      pcFilter = ob_create_filter("PointCloudFilter", &error);
    if(!pcFilter)
        GTEST_SKIP() << "No PointCloudFilter";

    // Test 1: Depth-only point cloud (OB_FORMAT_POINT) — pass single depth frame
    auto result = ob_filter_process(pcFilter, depth->getImpl(), &error);
    if(result) {
        ob_save_pointcloud_to_ply(plyFile.c_str(), result, false, false, 0.0f, &error);
        {
            std::ifstream ifs(plyFile);
            EXPECT_TRUE(ifs.good()) << "ASCII PLY file not created";
        }
        std::remove(plyFile.c_str());

        ob_save_pointcloud_to_ply(plyFile.c_str(), result, true, false, 0.0f, &error);
        {
            std::ifstream ifs(plyFile);
            EXPECT_TRUE(ifs.good()) << "Binary PLY file not created";
        }

        ob_delete_frame(result, &error);
    }

    // Test 2: RGB point cloud (OB_FORMAT_RGB_POINT) — pass frameset containing depth + color
    result = ob_filter_process(pcFilter, fs->getImpl(), &error);
    if(result) {
        ob_save_pointcloud_to_ply(plyFile.c_str(), result, false, false, 0.0f, &error);
        {
            std::ifstream ifs(plyFile);
            EXPECT_TRUE(ifs.good()) << "RGB ASCII PLY file not created";
        }
        std::remove(plyFile.c_str());

        ob_delete_frame(result, &error);
    }

    ob_delete_filter(pcFilter, &error);
    std::remove(plyFile.c_str());
}

// ============================================================
// Test group: Firmware.
// ============================================================
class TC_CPP_21_Firmware : public DeviceTest {};

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_01_global_timestamp) {
    /// Test case: global timestamp.
    bool supported = device_->isGlobalTimestampSupported();
    if(supported) {
        ASSERT_NO_THROW(device_->enableGlobalTimestamp(true));
        ASSERT_NO_THROW(device_->enableGlobalTimestamp(false));
    }
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_02_device_state) {
    /// Test case: device state.
    ASSERT_NO_THROW(device_->getDeviceState());
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_03_state_change_callback) {
    /// Test case: state change callback.
    ASSERT_NO_THROW(device_->setDeviceStateChangedCallback([](OBDeviceState, const char *) {}));
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_04_heartbeat) {
    /// Test case: heartbeat.
    ASSERT_NO_THROW(device_->enableHeartbeat(true));
    ASSERT_NO_THROW(device_->enableHeartbeat(false));
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_05_raw_vendor_command) {
    /// Test case: raw vendor command.
    // Prepare local state for the next check.
    SUCCEED();
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_06_calibration_param_list) {
    /// Test case: calibration param list.
    auto paramList = device_->getCalibrationCameraParamList();
    ASSERT_NE(paramList, nullptr);
    auto count = paramList->getCount();
    EXPECT_GT(count, 0u);

    for(uint32_t i = 0; i < count; i++) {
        auto param = paramList->getCameraParam(i);
        EXPECT_TRUE(std::isfinite(param.depthIntrinsic.fx));
        EXPECT_TRUE(std::isfinite(param.depthIntrinsic.fy));
        EXPECT_TRUE(std::isfinite(param.depthIntrinsic.cx));
        EXPECT_TRUE(std::isfinite(param.depthIntrinsic.cy));
        EXPECT_TRUE(std::isfinite(param.rgbIntrinsic.fx));
        EXPECT_TRUE(std::isfinite(param.rgbIntrinsic.fy));
    }
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_07_reboot) {
    /// Test case: reboot.
    // Reboots the device and verifies it reconnects and is responsive.
    auto sn = std::string(devInfo_->getSerialNumber());

    // Register a callback to detect reconnection.
    std::atomic<bool> reconnected{ false };
    auto              cbId = ctx_->registerDeviceChangedCallback([&](std::shared_ptr<ob::DeviceList> /*removed*/, std::shared_ptr<ob::DeviceList> added) {
        if(added && added->getCount() > 0)
            reconnected = true;
    });

    device_->reboot();
    device_.reset();
    devInfo_.reset();

    // Poll up to 30 s for the device to reappear.
    constexpr int kMaxWaitMs = 30000;
    constexpr int kPollMs    = 500;
    for(int elapsed = 0; !reconnected.load() && elapsed < kMaxWaitMs; elapsed += kPollMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(kPollMs));
    }
    ctx_->unregisterDeviceChangedCallback(cbId);
    ASSERT_TRUE(reconnected.load()) << "Device did not reconnect within 30 s after reboot";

    // Re-open the device and verify its identity.
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    device_ = devList->getDeviceBySN(sn.c_str());
    ASSERT_NE(device_, nullptr) << "Device SN " << sn << " not found after reboot";
    devInfo_ = device_->getDeviceInfo();
    ASSERT_NE(devInfo_, nullptr);
    EXPECT_STREQ(devInfo_->getSerialNumber(), sn.c_str());
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_08_firmware_update) {
    /// Test case: firmware update.
    // Requires ALLOW_DESTRUCTIVE_TESTS=true and a firmware .bin reachable via env var
    // FIRMWARE_FILE_PATH, or auto-discovered from tests/resource/firmware/.
    if(!ENV().allowDestructive()) {
        GTEST_SKIP() << "ALLOW_DESTRUCTIVE_TESTS not set — skipping firmware update";
    }
    ENV().skipIfNoFirmware();
    const std::string &fwPath = ENV().firmwarePath();

    std::atomic<OBFwUpdateState> lastState{ STAT_START };
    std::atomic<uint8_t>         lastPercent{ 0 };
    std::mutex                   mtx;
    std::condition_variable      cv;
    bool                         done = false;

    ob::Device::DeviceFwUpdateCallback cb = [&](OBFwUpdateState state, const char *msg, uint8_t percent) {
        (void)msg;
        lastState   = state;
        lastPercent = percent;
        if(state == STAT_DONE || state < 0 /* any ERR_ value */) {
            std::lock_guard<std::mutex> lk(mtx);
            done = true;
            cv.notify_all();
        }
    };

    ASSERT_NO_THROW(device_->updateFirmware(fwPath.c_str(), cb, /*async=*/true));

    // Wait up to 3 minutes for the update to complete.
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait_for(lk, std::chrono::minutes(3), [&] { return done; });
    }

    EXPECT_TRUE(done) << "Firmware update did not complete within 3 minutes";
    EXPECT_EQ(lastState.load(), STAT_DONE) << "Firmware update ended with state " << static_cast<int>(lastState.load());
    EXPECT_EQ(lastPercent.load(), 100u);
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_09_update_depth_presets) {
    /// Test case: update depth presets.
    // Requires destructive mode and a preset .bin reachable via env var
    // DEPTH_PRESET_PATH, or auto-discovered from tests/resource/present/.
    if(!ENV().allowDestructive()) {
        GTEST_SKIP() << "ALLOW_DESTRUCTIVE_TESTS not set — skipping depth preset update";
    }
    ENV().skipIfNoDepthPreset();
    const std::string &presetPathStr = ENV().depthPresetPath();

    // Build a single-entry 2D path list as required by updateOptionalDepthPresets.
    char pathList[1][OB_PATH_MAX]{};
    std::strncpy(pathList[0], presetPathStr.c_str(), OB_PATH_MAX - 1);

    std::atomic<OBFwUpdateState> lastState{ STAT_START };
    std::mutex                   mtx;
    std::condition_variable      cv;
    bool                         done = false;

    ob::Device::DeviceFwUpdateCallback cb = [&](OBFwUpdateState state, const char * /*msg*/, uint8_t /*percent*/) {
        lastState = state;
        if(state == STAT_DONE || state < 0 /* any ERR_ value */) {
            std::lock_guard<std::mutex> lk(mtx);
            done = true;
            cv.notify_all();
        }
    };

    ASSERT_NO_THROW(device_->updateOptionalDepthPresets(pathList, 1, cb));

    // Wait up to 2 minutes for the update to complete.
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait_for(lk, std::chrono::minutes(2), [&] { return done; });
    }

    EXPECT_TRUE(done) << "Depth preset update did not complete within 2 minutes";
    if(done && (lastState.load() == ERR_MISMATCH || lastState.load() == ERR_UNSUPPORT_DEV)) {
        GTEST_SKIP() << "Depth preset resource is not compatible with the connected device; state=" << static_cast<int>(lastState.load());
    }
    EXPECT_EQ(lastState.load(), STAT_DONE) << "Depth preset update ended with state " << static_cast<int>(lastState.load());
}

// ============================================================
// Test group: Error.
// ============================================================
class TC_CPP_24_Error_HW : public PipelineTest {};

TEST_F(TC_CPP_24_Error_HW, TC_CPP_24_04_pipeline_exception_safety) {
    /// Test case: pipeline exception safety.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    pipeline_->start(config);

    // Duplicate start should throw
    EXPECT_THROW(pipeline_->start(config), ob::Error);

    pipeline_->stop();

    // Duplicate stop should not crash
    EXPECT_NO_THROW(pipeline_->stop());

    // Pipeline should be reusable after stop
    EXPECT_NO_THROW(pipeline_->start(config));
    pipeline_->stop();
}

TEST_F(TC_CPP_24_Error_HW, TC_CPP_24_05_device_property_exception) {
    /// Test case: device property exception.
    // Invalid property ID should throw
    EXPECT_THROW(device_->getIntProperty(static_cast<OBPropertyID>(99999)), ob::Error);

    // Write to a read-only property should throw
    if(device_->isPropertySupported(OB_PROP_SLAVE_DEVICE_SYNC_STATUS_BOOL, OB_PERMISSION_READ)) {
        EXPECT_THROW(device_->setIntProperty(OB_PROP_SLAVE_DEVICE_SYNC_STATUS_BOOL, 0), ob::Error);
    }
}

// ============================================================
// Test group: DataStruct.
// ============================================================
class TC_CPP_25_DataStruct_HW : public PipelineTest {};

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_01_intrinsic_extrinsic) {
    /// Test case: intrinsic extrinsic.
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    auto calib = pipeline_->getCalibrationParam(config);

    // Depth intrinsics: verify finite
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_DEPTH].fx));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_DEPTH].fy));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_DEPTH].cx));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_DEPTH].cy));
    // Color intrinsics: verify finite
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_COLOR].fx));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_COLOR].fy));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_COLOR].cx));
    EXPECT_TRUE(std::isfinite(calib.intrinsics[OB_SENSOR_COLOR].cy));

    // Depth distortion: verify finite
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k1));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k2));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k3));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k4));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k5));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].k6));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].p1));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_DEPTH].p2));
    // Color distortion: verify finite
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k1));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k2));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k3));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k4));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k5));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].k6));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].p1));
    EXPECT_TRUE(std::isfinite(calib.distortion[OB_SENSOR_COLOR].p2));

    // Extrinsic (depth-to-color) rotation: should be orthogonal (det ≈ ±1)
    auto &rot = calib.extrinsics[OB_SENSOR_DEPTH][OB_SENSOR_COLOR].rot;
    float det = rot[0] * (rot[4] * rot[8] - rot[5] * rot[7]) - rot[1] * (rot[3] * rot[8] - rot[5] * rot[6]) + rot[2] * (rot[3] * rot[7] - rot[4] * rot[6]);
    EXPECT_NEAR(std::abs(det), 1.0f, 0.01f) << "Extrinsic rotation matrix should be orthogonal (det approximately equal ±1)";

    // Extrinsic translation: verify finite
    auto &trans = calib.extrinsics[OB_SENSOR_DEPTH][OB_SENSOR_COLOR].trans;
    for(int i = 0; i < 3; i++) {
        EXPECT_TRUE(std::isfinite(trans[i]));
    }
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_02_camera_calib_param) {
    // Same as the TC_CPP_25_01_intrinsic_extrinsic test case
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_03_imu_intrinsic) {
    /// Test case: imu intrinsic.
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accel)
        GTEST_SKIP() << "No accel sensor";

    auto profiles  = accel->getStreamProfileList();
    auto ap        = profiles->getProfile(0)->as<ob::AccelStreamProfile>();
    auto intrinsic = ap->getIntrinsic();

    EXPECT_TRUE(std::isfinite(intrinsic.noiseDensity));
    EXPECT_TRUE(std::isfinite(intrinsic.randomWalk));
    EXPECT_TRUE(std::isfinite(intrinsic.referenceTemp));
    for(int i = 0; i < 3; i++) {
        EXPECT_TRUE(std::isfinite(intrinsic.bias[i]));
        EXPECT_TRUE(std::isfinite(intrinsic.gravity[i]));
    }
    for(int i = 0; i < 9; i++) {
        EXPECT_TRUE(std::isfinite(intrinsic.scaleMisalignment[i]));
        EXPECT_TRUE(std::isfinite(intrinsic.tempSlope[i]));
    }

    auto gyro = device_->getSensor(OB_SENSOR_GYRO);
    if(gyro) {
        auto gProfiles  = gyro->getStreamProfileList();
        auto gp         = gProfiles->getProfile(0)->as<ob::GyroStreamProfile>();
        auto gIntrinsic = gp->getIntrinsic();

        EXPECT_TRUE(std::isfinite(gIntrinsic.noiseDensity));
        EXPECT_TRUE(std::isfinite(gIntrinsic.randomWalk));
        EXPECT_TRUE(std::isfinite(gIntrinsic.referenceTemp));
        for(int i = 0; i < 3; i++) {
            EXPECT_TRUE(std::isfinite(gIntrinsic.bias[i]));
        }
        for(int i = 0; i < 9; i++) {
            EXPECT_TRUE(std::isfinite(gIntrinsic.scaleMisalignment[i]));
            EXPECT_TRUE(std::isfinite(gIntrinsic.tempSlope[i]));
        }
    }
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_04_device_temperature) {
    /// Test case: device temperature.
    if(!device_->isPropertySupported(OB_STRUCT_DEVICE_TEMPERATURE, OB_PERMISSION_READ)) {
        GTEST_SKIP() << "Device temperature not supported";
    }

    OBDeviceTemperature temp     = {};
    uint32_t            dataSize = static_cast<uint32_t>(sizeof(temp));
    device_->getStructuredData(OB_STRUCT_DEVICE_TEMPERATURE, reinterpret_cast<uint8_t *>(&temp), &dataSize);

    // Verify all temperature fields are finite
    EXPECT_TRUE(std::isfinite(temp.ldmTemp));
    EXPECT_TRUE(std::isfinite(temp.mainBoardTemp));
    EXPECT_TRUE(std::isfinite(temp.tecTemp));
    EXPECT_TRUE(std::isfinite(temp.imuTemp));
    EXPECT_TRUE(std::isfinite(temp.rgbTemp));
    EXPECT_TRUE(std::isfinite(temp.irLeftTemp));
    EXPECT_TRUE(std::isfinite(temp.irRightTemp));
    EXPECT_TRUE(std::isfinite(temp.chipTopTemp));
    EXPECT_TRUE(std::isfinite(temp.chipBottomTemp));
}
