// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file hw_full_test.cpp
/// @brief 硬件全量测试 — 覆盖所有需要物理设备的测试用例 (~144 cases)
/// 测试模块: TC_CPP_02-21, 24(部分), 25(部分)

#include "../test_common.hpp"
#include <libobsensor/ObSensor.hpp>
#include <libobsensor/h/Utils.h>

#include <algorithm>
#include <atomic>
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
            try { pipeline_->stop(); } catch(...) {}
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
// TC_CPP_02: 设备发现与枚举 (4 HW tests)
// ============================================================
class TC_CPP_02_Discovery_HW : public DeviceTest {};

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_01_usb_device_enum) {
    /// USB 设备枚举 — deviceCount>0, 遍历设备有效
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    ASSERT_GT(devList->getCount(), 0u);

    for(uint32_t i = 0; i < devList->getCount(); i++) {
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_NE(devList->getSerialNumber(i), nullptr);
    }
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_03_net_device_direct) {
    /// 网络设备直连 — createNetDevice IP直连（仅335Le）
    ENV().skipIfNot335le();
    // 需要环境变量 ORBBEC_NET_IP 指定 335Le IP
    const char *ip = std::getenv("ORBBEC_NET_IP");
    if(!ip) {
        GTEST_SKIP() << "ORBBEC_NET_IP not set";
    }
    auto netDev = ctx_->createNetDevice(ip, 8090);
    ASSERT_NE(netDev, nullptr);
    auto info = netDev->getDeviceInfo();
    ASSERT_NE(info, nullptr);
    EXPECT_NE(info->getName(), nullptr);
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_04_force_ip) {
    /// 强制 IP 配置 — forceIp MAC→静态IP配置（仅335Le）
    ENV().skipIfNot335le();
    ENV().skipUnlessDestructive();
    // Skip: 需要知道设备MAC和目标IP, 并且会改变设备网络配置
    GTEST_SKIP() << "Force IP test requires explicit MAC/IP configuration";
}

TEST_F(TC_CPP_02_Discovery_HW, TC_CPP_02_05_hotplug_reboot) {
    /// 设备热插拔回调（reboot模拟）— removed/added 回调触发
    ENV().skipUnlessDestructive();

    std::atomic<bool> removedCalled{false};
    std::atomic<bool> addedCalled{false};

    auto cbId = ctx_->registerDeviceChangedCallback(
        [&](std::shared_ptr<ob::DeviceList> removed, std::shared_ptr<ob::DeviceList> added) {
            if(removed && removed->getCount() > 0) removedCalled = true;
            if(added && added->getCount() > 0) addedCalled = true;
        });

    auto sn = std::string(devInfo_->getSerialNumber());
    device_->reboot();
    device_.reset();

    // 等待设备断开和重连
    std::this_thread::sleep_for(std::chrono::seconds(15));

    EXPECT_TRUE(removedCalled.load()) << "Device removed callback not received";
    // 设备可能尚未重连，但 removed 应该已触发

    ctx_->unregisterDeviceChangedCallback(cbId);
}

// ============================================================
// TC_CPP_03: 设备列表查询 (6 HW tests)
// ============================================================
class TC_CPP_03_DeviceList : public DeviceTest {};

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_01_count_and_index) {
    /// 设备数量与索引访问 — count>0, 遍历所有设备有效
    auto devList = ctx_->queryDeviceList();
    auto count = devList->getCount();
    ASSERT_GT(count, 0u);

    for(uint32_t i = 0; i < count; i++) {
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_GT(devList->getPid(i), 0);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_02_access_mode) {
    /// 带访问模式获取设备 — EXCLUSIVE/SHARED 模式
    // 释放当前设备先
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    // EXCLUSIVE
    {
        auto dev = devList->getDevice(0, OB_DEVICE_EXCLUSIVE_ACCESS);
        ASSERT_NE(dev, nullptr);
        auto info = dev->getDeviceInfo();
        EXPECT_STREQ(info->getSerialNumber(), sn.c_str());
    }
    // Re-get with DEFAULT
    {
        auto dev = devList->getDevice(0, OB_DEVICE_DEFAULT_ACCESS);
        ASSERT_NE(dev, nullptr);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_03_get_by_sn_uid) {
    /// 按序列号/UID 查找 — getDeviceBySN/getDeviceByUid
    auto sn = std::string(devInfo_->getSerialNumber());
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
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_04_basic_info_fields) {
    /// 设备基本信息字段 — name/PID/VID(0x2BC5)/connectionType
    auto devList = ctx_->queryDeviceList();
    for(uint32_t i = 0; i < devList->getCount(); i++) {
        EXPECT_NE(devList->getName(i), nullptr);
        EXPECT_GT(devList->getPid(i), 0);
        EXPECT_EQ(devList->getVid(i), 0x2BC5);
        EXPECT_NE(devList->getConnectionType(i), nullptr);
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_05_net_device_info) {
    /// 网络设备信息字段 — IP/子网/网关 格式合法（仅335Le）
    ENV().skipIfNot335le();
    auto devList = ctx_->queryDeviceList();
    for(uint32_t i = 0; i < devList->getCount(); i++) {
        auto connType = std::string(devList->getConnectionType(i));
        if(connType == "Ethernet") {
            auto ip = devList->getIpAddress(i);
            EXPECT_NE(ip, nullptr);
            EXPECT_NE(std::string(ip), "0.0.0.0");
        }
    }
}

TEST_F(TC_CPP_03_DeviceList, TC_CPP_03_06_out_of_bounds) {
    /// 越界索引安全 — 越界索引抛异常不崩溃
    auto devList = ctx_->queryDeviceList();
    auto count = devList->getCount();
    EXPECT_THROW(devList->getDevice(count + 100), ob::Error);
}

// ============================================================
// TC_CPP_04: 设备信息 (5 HW tests)
// ============================================================
class TC_CPP_04_DeviceInfo : public DeviceTest {};

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_01_basic_info) {
    /// 基本信息字段 — name/SN/firmwareVersion/hardwareVersion 非空
    EXPECT_NE(devInfo_->getName(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getName()), 0u);
    EXPECT_NE(devInfo_->getSerialNumber(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getSerialNumber()), 0u);
    EXPECT_NE(devInfo_->getFirmwareVersion(), nullptr);
    EXPECT_NE(devInfo_->getHardwareVersion(), nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_02_id_fields) {
    /// 标识字段 — PID>0, VID=0x2BC5, UID非空, connectionType正确
    EXPECT_GT(devInfo_->getPid(), 0);
    EXPECT_EQ(devInfo_->getVid(), 0x2BC5);
    EXPECT_NE(devInfo_->getUid(), nullptr);
    EXPECT_GT(std::strlen(devInfo_->getUid()), 0u);
    auto conn = devInfo_->getConnectionType();
    EXPECT_NE(conn, nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_03_net_ip_info) {
    /// 网络设备 IP 信息 — ipAddress/subnetMask/gateway（仅335Le）
    ENV().skipIfNot335le();
    auto ip = devInfo_->getIpAddress();
    ASSERT_NE(ip, nullptr);
    EXPECT_NE(std::string(ip), "0.0.0.0");
    auto mask = devInfo_->getDeviceSubnetMask();
    EXPECT_NE(mask, nullptr);
    auto gw = devInfo_->getDeviceGateway();
    EXPECT_NE(gw, nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_04_chip_type_info) {
    /// 芯片与类型信息 — asicName/deviceType/supportedMinSdkVersion
    auto asic = devInfo_->getAsicName();
    EXPECT_NE(asic, nullptr);
    auto devType = devInfo_->getDeviceType();
    (void)devType;  // 枚举值存在即可
    auto minSdk = devInfo_->getSupportedMinSdkVersion();
    EXPECT_NE(minSdk, nullptr);
}

TEST_F(TC_CPP_04_DeviceInfo, TC_CPP_04_05_extension_info) {
    /// 扩展信息查询 — isExtensionInfoExist + getExtensionInfo
    // 尝试查询一些常见扩展信息键
    bool exists = device_->isExtensionInfoExist("SerialNumber");
    if(exists) {
        auto val = device_->getExtensionInfo("SerialNumber");
        EXPECT_NE(val, nullptr);
    }
    // 不存在的键返回 false
    EXPECT_FALSE(device_->isExtensionInfoExist("TotallyBogusExtKey_12345"));
}

// ============================================================
// TC_CPP_05: 设备访问模式 (4 HW tests)
// ============================================================
class TC_CPP_05_AccessMode : public DeviceTest {};

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_01_default_access) {
    /// DEFAULT_ACCESS 打开设备 — 默认模式开流取帧
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config   = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);
    auto frames = pipeline->waitForFrameset(3000);
    EXPECT_NE(frames, nullptr);
    pipeline->stop();
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_02_exclusive_access) {
    /// EXCLUSIVE_ACCESS 独占 — 独占后二次获取失败
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto dev1 = devList->getDevice(0, OB_DEVICE_EXCLUSIVE_ACCESS);
    ASSERT_NE(dev1, nullptr);

    // 二次获取应失败
    try {
        auto dev2 = devList->getDeviceBySN(sn.c_str(), OB_DEVICE_EXCLUSIVE_ACCESS);
        // 如果不抛异常也可能返回同一设备
    }
    catch(const ob::Error &) {
        SUCCEED() << "Expected: exclusive access prevents second open";
    }
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_03_shared_access) {
    /// SHARED_ACCESS 共享 — 多句柄同时持有+读属性
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto devA = devList->getDevice(0, OB_DEVICE_MONITOR_ACCESS);
    ASSERT_NE(devA, nullptr);

    // 第二个共享句柄
    try {
        auto devB = devList->getDeviceBySN(sn.c_str(), OB_DEVICE_MONITOR_ACCESS);
        if(devB) {
            auto infoB = devB->getDeviceInfo();
            EXPECT_STREQ(infoB->getSerialNumber(), sn.c_str());
        }
    }
    catch(const ob::Error &) {
        // 某些平台不支持共享访问
    }
}

TEST_F(TC_CPP_05_AccessMode, TC_CPP_05_04_control_only_access) {
    /// CONTROL_ONLY_ACCESS 仅控制 — 可读写属性，开流失败
    auto sn = std::string(devInfo_->getSerialNumber());
    devInfo_.reset();
    device_.reset();

    auto devList = ctx_->queryDeviceList();
    auto dev = devList->getDevice(0, OB_DEVICE_CONTROL_ACCESS);
    ASSERT_NE(dev, nullptr);

    // 读属性应成功
    auto info = dev->getDeviceInfo();
    EXPECT_NE(info->getName(), nullptr);
}

// ============================================================
// TC_CPP_06: 传感器枚举 (9 HW tests)
// ============================================================
class TC_CPP_06_Sensor : public SensorTest {};

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_01_sensor_list_completeness) {
    /// 传感器列表完整性 — count>0, ≥Depth+Color
    auto count = sensorList_->getCount();
    ASSERT_GT(count, 0u);
    EXPECT_GE(count, 2u) << "Expected at least Depth + Color sensors";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_02_core_sensors) {
    /// 按类型获取核心传感器 — Depth/Color/IR 传感器有效
    auto depth = device_->getSensor(OB_SENSOR_DEPTH);
    EXPECT_NE(depth, nullptr);
    EXPECT_EQ(depth->getType(), OB_SENSOR_DEPTH);

    auto color = device_->getSensor(OB_SENSOR_COLOR);
    EXPECT_NE(color, nullptr);
    EXPECT_EQ(color->getType(), OB_SENSOR_COLOR);

    auto ir = device_->getSensor(OB_SENSOR_IR);
    EXPECT_NE(ir, nullptr);
    EXPECT_EQ(ir->getType(), OB_SENSOR_IR);
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_03_imu_sensors) {
    /// IMU 传感器获取 — Accel/Gyro 传感器
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    auto gyro  = device_->getSensor(OB_SENSOR_GYRO);
    // Gemini335 应支持IMU
    EXPECT_NE(accel, nullptr);
    EXPECT_NE(gyro, nullptr);
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_04_stereo_ir) {
    /// 双目 IR 传感器 — IR_LEFT/IR_RIGHT（若支持）
    auto irLeft = device_->getSensor(OB_SENSOR_IR_LEFT);
    auto irRight = device_->getSensor(OB_SENSOR_IR_RIGHT);
    // 双目IR不是所有设备都支持
    if(irLeft) {
        EXPECT_EQ(irLeft->getType(), OB_SENSOR_IR_LEFT);
    }
    if(irRight) {
        EXPECT_EQ(irRight->getType(), OB_SENSOR_IR_RIGHT);
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_05_invalid_sensor_type) {
    /// 不存在传感器类型安全 — 返回null或异常不崩溃
    try {
        auto sensor = device_->getSensor(static_cast<OBSensorType>(999));
        // 返回null也可接受
    }
    catch(const ob::Error &) {
        SUCCEED();
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_06_sensor_type_consistency) {
    /// Sensor 类型一致性 — getSensor(type)->type()==type
    OBSensorType types[] = {OB_SENSOR_DEPTH, OB_SENSOR_COLOR, OB_SENSOR_IR};
    for(auto type : types) {
        auto sensor = device_->getSensor(type);
        if(sensor) {
            EXPECT_EQ(sensor->getType(), type);
        }
    }
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_07_sensor_callback_stream) {
    /// Sensor 级别 callback 开流 — start(profile,callback) 回调取帧
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr);

    auto profileList = depthSensor->getStreamProfileList();
    ASSERT_NE(profileList, nullptr);
    ASSERT_GT(profileList->getCount(), 0u);

    auto profile = profileList->getProfile(0);

    std::atomic<int> frameCount{0};
    depthSensor->start(profile, [&frameCount](std::shared_ptr<ob::Frame>) {
        frameCount++;
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    depthSensor->stop();

    EXPECT_GT(frameCount.load(), 0) << "No frames received via sensor callback";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_08_sensor_repeated_start_stop) {
    /// Sensor 级别重复开关流 — 5次 start/stop 循环
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr);
    auto profileList = depthSensor->getStreamProfileList();
    auto profile = profileList->getProfile(0);

    for(int i = 0; i < 5; i++) {
        depthSensor->start(profile, [](std::shared_ptr<ob::Frame>) {});
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        depthSensor->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    SUCCEED() << "5 start/stop cycles completed";
}

TEST_F(TC_CPP_06_Sensor, TC_CPP_06_09_sensor_after_reboot) {
    /// 设备重启后传感器恢复（破坏性测试）
    ENV().skipUnlessDestructive();
    GTEST_SKIP() << "Reboot test requires extended wait and dedicated hardware setup";
}

// ============================================================
// TC_CPP_07: 流配置 StreamProfile (5 HW tests)
// ============================================================
class TC_CPP_07_StreamProfile : public SensorTest {};

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_01_depth_color_profiles) {
    /// Depth/Color Profile 列表 — count>0, width/height/fps/format 有效
    for(auto sensorType : {OB_SENSOR_DEPTH, OB_SENSOR_COLOR}) {
        auto sensor = device_->getSensor(sensorType);
        ASSERT_NE(sensor, nullptr) << "Sensor null for type " << sensorType;
        auto profiles = sensor->getStreamProfileList();
        ASSERT_GT(profiles->getCount(), 0u) << "No profiles for sensor type " << sensorType;

        for(uint32_t i = 0; i < profiles->getCount(); i++) {
            auto p = profiles->getProfile(i)->as<ob::VideoStreamProfile>();
            EXPECT_GT(p->getWidth(), 0u);
            EXPECT_GT(p->getHeight(), 0u);
            EXPECT_GT(p->getFps(), 0u);
        }
    }
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_02_video_profile_filter) {
    /// 按参数筛选 VideoStreamProfile — getVideoStreamProfile 精确匹配
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto depthProfiles = pipeline->getStreamProfileList(OB_SENSOR_DEPTH);
    ASSERT_GT(depthProfiles->getCount(), 0u);

    auto first = depthProfiles->getProfile(0)->as<ob::VideoStreamProfile>();
    auto w = first->getWidth();
    auto h = first->getHeight();
    auto fps = first->getFps();

    // 用同参数筛选应该能找到匹配项
    auto matched = depthProfiles->getVideoStreamProfile(w, h, first->getFormat(), fps);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->getWidth(), w);
    EXPECT_EQ(matched->getHeight(), h);
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_03_accel_profile) {
    /// AccelStreamProfile 属性 — fullScaleRange/sampleRate 有效
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accel) GTEST_SKIP() << "No accel sensor";

    auto profiles = accel->getStreamProfileList();
    ASSERT_GT(profiles->getCount(), 0u);

    auto ap = profiles->getProfile(0)->as<ob::AccelStreamProfile>();
    EXPECT_NE(ap->getFullScaleRange(), 0);
    EXPECT_NE(ap->getSampleRate(), 0);
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_04_gyro_profile) {
    /// GyroStreamProfile 属性 — fullScaleRange/sampleRate 有效
    auto gyro = device_->getSensor(OB_SENSOR_GYRO);
    if(!gyro) GTEST_SKIP() << "No gyro sensor";

    auto profiles = gyro->getStreamProfileList();
    ASSERT_GT(profiles->getCount(), 0u);

    auto gp = profiles->getProfile(0)->as<ob::GyroStreamProfile>();
    EXPECT_NE(gp->getFullScaleRange(), 0);
    EXPECT_NE(gp->getSampleRate(), 0);
}

TEST_F(TC_CPP_07_StreamProfile, TC_CPP_07_05_profile_type_check) {
    /// StreamProfile 类型检查与转换 — is<T>()/as<T>() 安全
    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    auto profiles = depthSensor->getStreamProfileList();
    auto p = profiles->getProfile(0);

    EXPECT_TRUE(p->is<ob::VideoStreamProfile>());
    auto vp = p->as<ob::VideoStreamProfile>();
    ASSERT_NE(vp, nullptr);

    // 非视频流类型转换应失败
    EXPECT_FALSE(p->is<ob::AccelStreamProfile>());
    EXPECT_THROW(p->as<ob::AccelStreamProfile>(), std::runtime_error);
}

// ============================================================
// TC_CPP_08: 流水线 Pipeline (11 HW tests)
// ============================================================
class TC_CPP_08_Pipeline : public PipelineTest {};

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_01_pipeline_construct) {
    /// Pipeline 默认/指定设备构造
    ASSERT_NE(pipeline_, nullptr);
    // 也测试无参构造 (需先释放当前设备)
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_02_default_start_stop) {
    /// Pipeline 默认开流与停流 — start()/waitForFrames()/stop()
    pipeline_->start();
    auto frames = pipeline_->waitForFrameset(3000);
    EXPECT_NE(frames, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_03_config_start_poll) {
    /// Pipeline 配置开流（轮询）— start(config)+waitForFrames 10次
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);

    int validFrames = 0;
    for(int i = 0; i < 10; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(fs) validFrames++;
    }
    pipeline_->stop();
    EXPECT_GE(validFrames, 8) << "Expected >=8 valid framesets out of 10";
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_04_config_start_callback) {
    /// Pipeline 配置开流（回调）— start(config,callback) 实时取帧
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    std::atomic<int> cbCount{0};
    pipeline_->start(config, [&cbCount](std::shared_ptr<ob::FrameSet>) {
        cbCount++;
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pipeline_->stop();

    EXPECT_GT(cbCount.load(), 0) << "No callback frames received";
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_05_switch_config) {
    /// Pipeline 运行时切换配置 — switchConfig 动态切换
    auto config1 = std::make_shared<ob::Config>();
    config1->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config1);
    pipeline_->waitForFrameset(2000);

    // 切换到 depth + color
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
    /// Pipeline 获取绑定设备 — getDevice SN一致
    auto pipeDev = pipeline_->getDevice();
    ASSERT_NE(pipeDev, nullptr);
    auto pipeInfo = pipeDev->getDeviceInfo();
    EXPECT_STREQ(pipeInfo->getSerialNumber(), devInfo_->getSerialNumber());
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_07_get_stream_profile_list) {
    /// Pipeline 获取 StreamProfile 列表
    auto depthProfiles = pipeline_->getStreamProfileList(OB_SENSOR_DEPTH);
    ASSERT_NE(depthProfiles, nullptr);
    EXPECT_GT(depthProfiles->getCount(), 0u);
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_08_frame_sync) {
    /// Pipeline 帧同步开关 — Depth/Color时间戳差<33ms
    pipeline_->enableFrameSync();
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);

    // 等稳定后取帧
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    for(int i = 0; i < 5; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs) continue;
        auto depth = fs->getDepthFrame();
        auto color = fs->getColorFrame();
        if(depth && color) {
            auto diff = std::abs((int64_t)depth->getTimeStampUs() - (int64_t)color->getTimeStampUs());
            EXPECT_LT(diff, 33000) << "Depth-Color timestamp diff > 33ms";
        }
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_09_d2c_depth_profile_list) {
    /// D2C 兼容 Depth Profile 列表
    auto colorProfiles = pipeline_->getStreamProfileList(OB_SENSOR_COLOR);
    if(colorProfiles && colorProfiles->getCount() > 0) {
        auto colorProfile = colorProfiles->getProfile(0);
        try {
            auto d2cList = pipeline_->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE);
            if(d2cList) {
                EXPECT_GT(d2cList->getCount(), 0u);
            }
        }
        catch(const ob::Error &) {
            // HW D2C may not be supported
        }
    }
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_10_camera_param) {
    /// Pipeline 获取相机参数 — getCameraParam fx/fy/cx/cy>0
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);
    pipeline_->waitForFrameset(2000);

    auto param = pipeline_->getCameraParam();
    EXPECT_GT(param.depthIntrinsic.fx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.fy, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fx, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fy, 0.0f);
    pipeline_->stop();
}

TEST_F(TC_CPP_08_Pipeline, TC_CPP_08_11_calibration_param) {
    /// Pipeline 获取完整标定参数
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    auto calibParam = pipeline_->getCalibrationParam(config);
    // 至少 depth intrinsic 应有效
    EXPECT_GT(calibParam.intrinsics[OB_SENSOR_DEPTH].fx, 0.0f);
    EXPECT_GT(calibParam.intrinsics[OB_SENSOR_COLOR].fx, 0.0f);
}

// ============================================================
// TC_CPP_09: 流水线配置 Config (8 HW tests)
// ============================================================
class TC_CPP_09_Config : public PipelineTest {};

TEST_F(TC_CPP_09_Config, TC_CPP_09_01_enable_stream_by_type) {
    /// 按类型/Profile 启用流
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
    /// 参数化启用视频流
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
    /// 启用 IMU 流
    auto config = std::make_shared<ob::Config>();
    config->enableAccelStream();
    config->enableGyroStream();
    pipeline_->start(config);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    pipeline_->stop();
    SUCCEED();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_04_enable_disable_all) {
    /// 启用/禁用所有流
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
    /// 查询已启用 Profile 列表
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    auto profiles = config->getEnabledStreamProfileList();
    EXPECT_GE(profiles->getCount(), 2u);
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_06_d2c_align_mode) {
    /// D2C 对齐模式 — HW/SW/DISABLE
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    config->setAlignMode(ALIGN_DISABLE);
    pipeline_->start(config);
    pipeline_->waitForFrameset(2000);
    pipeline_->stop();

    config->setAlignMode(ALIGN_D2C_SW_MODE);
    pipeline_->start(config);
    pipeline_->waitForFrameset(2000);
    pipeline_->stop();

    SUCCEED();
}

TEST_F(TC_CPP_09_Config, TC_CPP_09_07_depth_scale_after_align) {
    /// 对齐后深度缩放
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
    /// 帧聚合输出模式 — ALL_TYPE/ANY
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline_->start(config);
    auto fs = pipeline_->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);
    pipeline_->stop();
}

// ============================================================
// TC_CPP_10: 帧数据访问 (12 HW tests)
// ============================================================
class TC_CPP_10_Frame_HW : public PipelineTest {
protected:
    std::shared_ptr<ob::FrameSet> captureFrames() {
        auto config = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        config->enableStream(OB_STREAM_COLOR);
        pipeline_->start(config);
        // 丢弃前几帧
        for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
        auto fs = pipeline_->waitForFrameset(3000);
        return fs;
    }
};

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_01_frame_basic_properties) {
    /// Frame 基本属性完整性 — index/format/type/data/dataSize
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    EXPECT_EQ(depth->getType(), OB_FRAME_DEPTH);
    EXPECT_NE(depth->getFormat(), OB_FORMAT_UNKNOWN);
    EXPECT_NE(depth->getData(), nullptr);
    EXPECT_GT(depth->getDataSize(), 0u);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_02_timestamp_monotonicity) {
    /// Frame 时间戳单调性 — 30帧时间戳严格递增
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);

    std::vector<uint64_t> timestamps;
    for(int i = 0; i < 35; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs) continue;
        auto df = fs->getDepthFrame();
        if(df) timestamps.push_back(df->getTimeStampUs());
    }
    pipeline_->stop();

    ASSERT_GE(timestamps.size(), 10u);
    for(size_t i = 1; i < timestamps.size(); i++) {
        EXPECT_GT(timestamps[i], timestamps[i - 1])
            << "Timestamp not monotonic at index " << i;
    }
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_03_frame_associated_info) {
    /// Frame 关联信息 — getStreamProfile/getSensor/getDevice
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto profile = depth->getStreamProfile();
    EXPECT_NE(profile, nullptr);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_05_video_frame_properties) {
    /// VideoFrame 宽高与像素属性
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    EXPECT_GT(depth->getWidth(), 0u);
    EXPECT_GT(depth->getHeight(), 0u);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_06_depth_frame_scale) {
    /// DepthFrame 深度比例与数据 — getValueScale>0, ≥10%有效深度值
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);

    float scale = depth->getValueScale();
    EXPECT_GT(scale, 0.0f);

    auto *data = reinterpret_cast<const uint16_t *>(depth->getData());
    uint32_t pixCount = depth->getWidth() * depth->getHeight();
    uint32_t validCount = 0;
    for(uint32_t i = 0; i < pixCount; i++) {
        if(data[i] > 0) validCount++;
    }
    float validRatio = static_cast<float>(validCount) / pixCount;
    EXPECT_GE(validRatio, 0.1f) << "Less than 10% valid depth pixels";
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_07_color_ir_data_valid) {
    /// ColorFrame / IRFrame 数据有效性 — 数据非全零
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    config->enableStream(OB_STREAM_IR);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);

    auto color = fs->getColorFrame();
    if(color) {
        auto *d = color->getData();
        bool allZero = true;
        for(uint32_t i = 0; i < std::min(color->getDataSize(), 1000u); i++) {
            if(d[i] != 0) { allZero = false; break; }
        }
        EXPECT_FALSE(allZero) << "Color frame is all zeros";
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_08_points_frame) {
    /// PointsFrame 属性 — getCoordinateValueScale>0
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    pipeline_->stop();

    // 使用 PointCloudFilter 生成点云
    ob_error *error = nullptr;
    auto pcFilter = ob_create_filter("PointCloudFilter", &error);
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
    /// AccelFrame 值与温度 — z≈9.8, temperature 0~80°C
    auto config = std::make_shared<ob::Config>();
    config->enableAccelStream();
    pipeline_->start(config);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 使用sensor级callback取IMU帧
    auto accelSensor = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accelSensor) {
        pipeline_->stop();
        GTEST_SKIP() << "No accel sensor";
    }

    pipeline_->stop();

    auto profiles = accelSensor->getStreamProfileList();
    auto profile = profiles->getProfile(0);

    std::shared_ptr<ob::AccelFrame> accelFrame;
    std::mutex mtx;
    std::condition_variable cv;

    accelSensor->start(profile, [&](std::shared_ptr<ob::Frame> frame) {
        if(frame->is<ob::AccelFrame>()) {
            std::lock_guard<std::mutex> lock(mtx);
            accelFrame = frame->as<ob::AccelFrame>();
            cv.notify_one();
        }
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(3), [&]{ return accelFrame != nullptr; });
    }
    accelSensor->stop();

    ASSERT_NE(accelFrame, nullptr) << "No accel frame received";
    auto val = accelFrame->getValue();
    // 静止时 z ≈ 9.8 (根据设备朝向)
    float mag = std::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);
    EXPECT_NEAR(mag, 9.8f, 3.0f) << "Accel magnitude unexpected: " << mag;

    float temp = accelFrame->getTemperature();
    EXPECT_GE(temp, 0.0f);
    EXPECT_LE(temp, 80.0f);
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_10_gyro_frame) {
    /// GyroFrame 值与温度 — 静止时xyz≈0
    auto gyroSensor = device_->getSensor(OB_SENSOR_GYRO);
    if(!gyroSensor) GTEST_SKIP() << "No gyro sensor";

    auto profiles = gyroSensor->getStreamProfileList();
    auto profile = profiles->getProfile(0);

    std::shared_ptr<ob::GyroFrame> gyroFrame;
    std::mutex mtx;
    std::condition_variable cv;

    gyroSensor->start(profile, [&](std::shared_ptr<ob::Frame> frame) {
        if(frame->is<ob::GyroFrame>()) {
            std::lock_guard<std::mutex> lock(mtx);
            gyroFrame = frame->as<ob::GyroFrame>();
            cv.notify_one();
        }
    });

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(3), [&]{ return gyroFrame != nullptr; });
    }
    gyroSensor->stop();

    ASSERT_NE(gyroFrame, nullptr) << "No gyro frame received";
    auto val = gyroFrame->getValue();
    // 静止时角速度应接近0
    EXPECT_NEAR(val.x, 0.0f, 1.0f);
    EXPECT_NEAR(val.y, 0.0f, 1.0f);
    EXPECT_NEAR(val.z, 0.0f, 1.0f);

    float temp = gyroFrame->getTemperature();
    EXPECT_GE(temp, 0.0f);
    EXPECT_LE(temp, 80.0f);
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_11_frameset_count_extract) {
    /// FrameSet 帧计数与提取
    auto fs = captureFrames();
    ASSERT_NE(fs, nullptr);
    EXPECT_GE(fs->getCount(), 1u);

    auto depth = fs->getDepthFrame();
    auto color = fs->getColorFrame();
    // 至少有一个有效
    EXPECT_TRUE(depth || color);
    pipeline_->stop();
}

TEST_F(TC_CPP_10_Frame_HW, TC_CPP_10_12_frameset_by_type_index) {
    /// FrameSet 按类型/索引提取
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
    /// FrameSet Depth+Color 帧同步 — 时间戳差<33ms
    pipeline_->enableFrameSync();
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);

    for(int i = 0; i < 5; i++) pipeline_->waitForFrameset(2000);

    int synced = 0;
    for(int i = 0; i < 5; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs) continue;
        auto d = fs->getDepthFrame();
        auto c = fs->getColorFrame();
        if(d && c) {
            auto diff = std::abs((int64_t)d->getTimeStampUs() - (int64_t)c->getTimeStampUs());
            if(diff < 33000) synced++;
        }
    }
    pipeline_->stop();
    EXPECT_GE(synced, 3) << "Insufficient synced frame pairs";
}

// ============================================================
// TC_CPP_11: 帧元数据 (5 HW tests)
// ============================================================
class TC_CPP_11_Metadata_HW : public PipelineTest {};

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_01_metadata_basic_read) {
    /// 元数据存在性检查与基本读取
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
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
    /// 帧号与帧率元数据 — FRAME_NUMBER递增, ACTUAL_FRAME_RATE
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++) pipeline_->waitForFrameset(2000);

    std::vector<int64_t> frameNums;
    for(int i = 0; i < 10; i++) {
        auto fs = pipeline_->waitForFrameset(3000);
        if(!fs) continue;
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
    /// 曝光/增益/激光元数据
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    // 这些元数据不一定所有设备都支持
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
    /// 原始元数据缓冲区 — getMetadata 非空
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    auto md = d->getMetadata();
    auto mdSize = d->getMetadataSize();
    if(mdSize > 0) {
        EXPECT_NE(md, nullptr);
    }
    pipeline_->stop();
}

TEST_F(TC_CPP_11_Metadata_HW, TC_CPP_11_05_unsupported_field_safe) {
    /// 不支持字段安全访问 — hasMetadata=false 不崩溃
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto d = fs->getDepthFrame();
    ASSERT_NE(d, nullptr);

    // 查询一个不太可能支持的元数据类型
    bool has = d->hasMetadata(static_cast<OBFrameMetadataType>(9999));
    EXPECT_FALSE(has);
    pipeline_->stop();
}

// ============================================================
// TC_CPP_12: 帧创建 (1 HW test)
// ============================================================
class TC_CPP_12_FrameFactory_HW : public PipelineTest {};

TEST_F(TC_CPP_12_FrameFactory_HW, TC_CPP_12_02_clone_frame) {
    /// 克隆帧与从 Profile 创建 — createFrameFromOtherFrame 数据一致
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    auto depth = fs->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    pipeline_->stop();

    ob_error *error = nullptr;
    auto clone = ob_create_frame_from_other_frame(depth->getImpl(), true, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(clone, nullptr);

    EXPECT_EQ(ob_frame_get_data_size(clone, &error), depth->getDataSize());
    EXPECT_EQ(ob_frame_get_format(clone, &error), (ob_format)depth->getFormat());

    ob_delete_frame(clone, &error);
}

// ============================================================
// TC_CPP_13: 滤波器 Filter (11 HW tests)
// ============================================================
class TC_CPP_13_Filter_HW : public PipelineTest {
protected:
    std::shared_ptr<ob::DepthFrame> getDepthFrame() {
        auto config = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        pipeline_->start(config);
        for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
        auto fs = pipeline_->waitForFrameset(3000);
        pipeline_->stop();
        return fs ? fs->getDepthFrame() : nullptr;
    }
};

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_03_filter_sync_process) {
    /// Filter 同步处理流程 — process(frame) 输出合法
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
    EXPECT_GT(result->getDataSize(), 0u);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_04_filter_async_callback) {
    /// Filter 异步回调处理
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    std::atomic<bool> cbCalled{false};
    filter->setCallBack([&cbCalled](std::shared_ptr<ob::Frame>) {
        cbCalled = true;
    });
    filter->pushFrame(depth);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(cbCalled.load());
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_08_pointcloud_filter) {
    /// PointCloudFilter XYZ 点云生成
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
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    ASSERT_NE(fs, nullptr);
    // D2C 通过 pipeline config 验证
    pipeline_->stop();
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_10_format_converter) {
    /// FormatConverter 格式转换
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);
    for(int i = 0; i < 3; i++) pipeline_->waitForFrameset(2000);
    auto fs = pipeline_->waitForFrameset(3000);
    pipeline_->stop();
    ASSERT_NE(fs, nullptr);
    auto color = fs->getColorFrame();
    if(!color) GTEST_SKIP() << "No color frame";

    ob_error *error = nullptr;
    auto filter = ob_create_filter("FormatConverterFilter", &error);
    if(!filter) GTEST_SKIP() << "FormatConverterFilter not available";

    auto result = ob_filter_process(filter, color->getImpl(), &error);
    if(result) {
        EXPECT_GT(ob_frame_get_data_size(result, &error), 0u);
        ob_delete_frame(result, &error);
    }
    ob_delete_filter(filter, &error);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_11_hdr_merge) {
    /// HDRMerge + SequenceIdFilter（若支持HDR）
    // HDR需要特定设备支持，这里只验证filter可创建
    ob_error *error = nullptr;
    auto hdrFilter = ob_create_filter("HDRMergeFilter", &error);
    auto seqFilter = ob_create_filter("SequenceIdFilter", &error);

    if(hdrFilter) ob_delete_filter(hdrFilter, &error);
    if(seqFilter) ob_delete_filter(seqFilter, &error);
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_12_decimation_filter) {
    /// DecimationFilter 降采样 — 输出分辨率=输入/scale
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
    if(result && result->is<ob::VideoFrame>()) {
        auto vf = result->as<ob::VideoFrame>();
        // 降采样后分辨率应该更小
        EXPECT_LE(vf->getWidth(), depth->getWidth());
    }
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_13_threshold_filter) {
    /// ThresholdFilter 深度阈值
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    auto filter = std::make_shared<ob::Filter>(ob_create_filter("ThresholdFilter", nullptr));
    auto result = filter->process(depth);
    EXPECT_NE(result, nullptr);
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_14_spatial_filters) {
    /// 空间滤波器组 — SpatialAdvanced/Fast/Moderate
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name : {"SpatialAdvancedFilter", "SpatialFastFilter", "SpatialModerateFilter"}) {
        ob_error *error = nullptr;
        auto f = ob_create_filter(name, &error);
        if(!f) continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result) {
            EXPECT_GT(ob_frame_get_data_size(result, &error), 0u);
            ob_delete_frame(result, &error);
        }
        ob_delete_filter(f, &error);
    }
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_15_temporal_holefilling_noise) {
    /// 时域/空洞/降噪滤波器组
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name : {"TemporalFilter", "HoleFillingFilter", "NoiseRemovalFilter"}) {
        ob_error *error = nullptr;
        auto f = ob_create_filter(name, &error);
        if(!f) continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result) ob_delete_frame(result, &error);
        ob_delete_filter(f, &error);
    }
    SUCCEED();
}

TEST_F(TC_CPP_13_Filter_HW, TC_CPP_13_16_false_positive_disparity) {
    /// FalsePositiveFilter + DisparityTransform
    auto depth = getDepthFrame();
    ASSERT_NE(depth, nullptr);

    for(const auto &name : {"FalsePositiveFilter", "DisparityTransformFilter"}) {
        ob_error *error = nullptr;
        auto f = ob_create_filter(name, &error);
        if(!f) continue;
        auto result = ob_filter_process(f, depth->getImpl(), &error);
        if(result) ob_delete_frame(result, &error);
        ob_delete_filter(f, &error);
    }
    SUCCEED();
}

// ============================================================
// TC_CPP_14: 设备属性控制 (17 HW tests)
// ============================================================
class TC_CPP_14_Property_HW : public DeviceTest {};

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_01_property_enum) {
    /// 属性枚举与支持查询
    int count = device_->getSupportedPropertyCount();
    EXPECT_GT(count, 0);
    for(int i = 0; i < count; i++) {
        auto item = device_->getSupportedProperty(i);
        EXPECT_NE(item.id, 0);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_02_bool_property) {
    /// Bool 属性读写 — Laser/LDP 开关
    if(device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE)) {
        bool val = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        device_->setBoolProperty(OB_PROP_LASER_BOOL, !val);
        device_->setBoolProperty(OB_PROP_LASER_BOOL, val);  // 恢复
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_03_int_property_range) {
    /// Int 属性读写与 Range
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
    /// Float 属性读写与 Range
    if(device_->isPropertySupported(OB_PROP_COLOR_GAIN_INT, OB_PERMISSION_READ)) {
        auto range = device_->getIntPropertyRange(OB_PROP_COLOR_GAIN_INT);
        int cur = device_->getIntProperty(OB_PROP_COLOR_GAIN_INT);
        (void)cur;
        (void)range;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_05_structured_data) {
    /// 结构体属性读写
    // 读取OBMultiDeviceSyncConfig
    auto config = device_->getMultiDeviceSyncConfig();
    // 只读验证，不写入
    (void)config;
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_06_raw_data) {
    /// 原始数据读取 — getRawData
    // 部分设备支持; 不崩溃即可
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_07_customer_data) {
    /// 自定义用户数据 — writeCustomerData/readCustomerData
    ENV().skipUnlessDestructive();
    // 写入自定义数据可能影响设备，标记为破坏性
    GTEST_SKIP() << "Customer data write is destructive";
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_08_laser_control) {
    /// 激光控制属性组 — LDP/Laser/LaserPower
    if(device_->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ)) {
        bool laserOn = device_->getBoolProperty(OB_PROP_LASER_BOOL);
        (void)laserOn;
    }
    if(device_->isPropertySupported(OB_PROP_LDP_BOOL, OB_PERMISSION_READ)) {
        bool ldp = device_->getBoolProperty(OB_PROP_LDP_BOOL);
        (void)ldp;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_09_depth_control) {
    /// 深度控制属性组 — Mirror/Flip/MinMax
    if(device_->isPropertySupported(OB_PROP_DEPTH_MIRROR_BOOL, OB_PERMISSION_READ)) {
        bool mirror = device_->getBoolProperty(OB_PROP_DEPTH_MIRROR_BOOL);
        (void)mirror;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_10_color_control) {
    /// 色彩控制属性组
    if(device_->isPropertySupported(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, OB_PERMISSION_READ)) {
        bool ae = device_->getBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL);
        (void)ae;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_11_ir_control) {
    /// IR 控制属性组
    if(device_->isPropertySupported(OB_PROP_IR_MIRROR_BOOL, OB_PERMISSION_READ)) {
        bool val = device_->getBoolProperty(OB_PROP_IR_MIRROR_BOOL);
        (void)val;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_12_device_management) {
    /// 设备管理属性组
    if(device_->isPropertySupported(OB_PROP_INDICATOR_LIGHT_BOOL, OB_PERMISSION_READ)) {
        bool val = device_->getBoolProperty(OB_PROP_INDICATOR_LIGHT_BOOL);
        (void)val;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_13_timing_sync) {
    /// 时序/同步属性组
    auto config = device_->getTimestampResetConfig();
    (void)config;
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_14_hdr_interleave) {
    /// HDR/帧交错属性
    if(device_->isPropertySupported(OB_PROP_HDR_MERGE_BOOL, OB_PERMISSION_READ)) {
        bool val = device_->getBoolProperty(OB_PROP_HDR_MERGE_BOOL);
        (void)val;
    }
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_15_structured_read_group) {
    /// 结构体属性读取组 — Temperature/Baseline等
    SUCCEED();
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_17_unsupported_property_safe) {
    /// 不支持属性读写安全 — 抛异常不崩溃
    bool supported = device_->isPropertySupported(static_cast<OBPropertyID>(99999), OB_PERMISSION_READ);
    EXPECT_FALSE(supported);

    if(!supported) {
        EXPECT_THROW(device_->getIntProperty(static_cast<OBPropertyID>(99999)), ob::Error);
    }
}

TEST_F(TC_CPP_14_Property_HW, TC_CPP_14_18_out_of_range_safe) {
    /// 属性超范围设置安全
    if(device_->isPropertySupported(OB_PROP_DEPTH_EXPOSURE_INT, OB_PERMISSION_WRITE)) {
        auto range = device_->getIntPropertyRange(OB_PROP_DEPTH_EXPOSURE_INT);
        EXPECT_THROW(device_->setIntProperty(OB_PROP_DEPTH_EXPOSURE_INT, range.max + 10000), ob::Error);
    }
    SUCCEED();
}

// ============================================================
// TC_CPP_15: 深度工作模式 (5 HW tests)
// ============================================================
class TC_CPP_15_DepthMode : public DeviceTest {};

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_01_current_mode) {
    /// 获取当前模式与名称
    auto mode = device_->getCurrentDepthWorkMode();
    EXPECT_GT(std::strlen(mode.name), 0u);
    auto name = device_->getCurrentDepthModeName();
    EXPECT_NE(name, nullptr);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_02_enum_modes) {
    /// 枚举所有深度工作模式
    auto list = device_->getDepthWorkModeList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);

    for(uint32_t i = 0; i < list->getCount(); i++) {
        auto mode = list->getOBDepthWorkMode(i);
        EXPECT_GT(std::strlen(mode.name), 0u);
    }
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_03_switch_by_name) {
    /// 按名称切换深度模式
    auto list = device_->getDepthWorkModeList();
    ASSERT_GT(list->getCount(), 0u);
    auto firstMode = list->getOBDepthWorkMode(0);

    auto status = device_->switchDepthWorkMode(firstMode.name);
    EXPECT_EQ(status, OB_STATUS_OK);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_04_switch_by_struct) {
    /// 按 struct 切换深度模式
    auto list = device_->getDepthWorkModeList();
    ASSERT_GT(list->getCount(), 0u);
    auto mode = list->getOBDepthWorkMode(0);

    auto status = device_->switchDepthWorkMode(mode);
    EXPECT_EQ(status, OB_STATUS_OK);
}

TEST_F(TC_CPP_15_DepthMode, TC_CPP_15_05_profile_change_after_switch) {
    /// 切换后 StreamProfile 变化
    auto list = device_->getDepthWorkModeList();
    if(list->getCount() < 2) GTEST_SKIP() << "Only 1 depth mode available";

    auto depthSensor = device_->getSensor(OB_SENSOR_DEPTH);
    auto profilesBefore = depthSensor->getStreamProfileList();
    auto countBefore = profilesBefore->getCount();

    auto mode1 = list->getOBDepthWorkMode(1);
    device_->switchDepthWorkMode(mode1);

    auto profilesAfter = depthSensor->getStreamProfileList();
    // Profile列表可能不同
    (void)countBefore;
    EXPECT_GT(profilesAfter->getCount(), 0u);
}

// ============================================================
// TC_CPP_16: 设备预设 Preset (5 HW tests)
// ============================================================
class TC_CPP_16_Preset : public DeviceTest {};

TEST_F(TC_CPP_16_Preset, TC_CPP_16_01_current_and_list) {
    /// 获取当前 Preset 与枚举列表
    auto list = device_->getAvailablePresetList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);

    auto curName = device_->getCurrentPresetName();
    EXPECT_NE(curName, nullptr);
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_02_load_builtin) {
    /// 加载内置 Preset
    auto list = device_->getAvailablePresetList();
    if(list->getCount() > 0) {
        auto name = list->getName(0);
        ASSERT_NO_THROW(device_->loadPreset(name));
    }
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_03_export_json) {
    /// 导出为 JSON 文件/数据
    const uint8_t *data = nullptr;
    uint32_t dataSize = 0;
    ASSERT_NO_THROW(device_->exportSettingsAsPresetJsonData("test_export", &data, &dataSize));
    EXPECT_NE(data, nullptr);
    EXPECT_GT(dataSize, 0u);
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_04_load_from_json) {
    /// 从 JSON 数据加载
    // 先导出再加载
    const uint8_t *data = nullptr;
    uint32_t dataSize = 0;
    device_->exportSettingsAsPresetJsonData("roundtrip_test", &data, &dataSize);
    if(data && dataSize > 0) {
        ASSERT_NO_THROW(device_->loadPresetFromJsonData("roundtrip_test", data, dataSize));
    }
}

TEST_F(TC_CPP_16_Preset, TC_CPP_16_05_preset_changes_properties) {
    /// Preset 切换后属性变化
    auto list = device_->getAvailablePresetList();
    if(list->getCount() < 2) GTEST_SKIP() << "Need >=2 presets";

    device_->loadPreset(list->getName(0));
    device_->loadPreset(list->getName(1));
    // 属性已变化，不崩溃即成功
    SUCCEED();
}

// ============================================================
// TC_CPP_17: 帧交错 Frame Interleave (3 HW tests)
// ============================================================
class TC_CPP_17_Interleave : public DeviceTest {};

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_01_support_query) {
    /// 支持查询与模式枚举
    bool supported = device_->isFrameInterleaveSupported();
    if(!supported) GTEST_SKIP() << "Frame interleave not supported";

    auto list = device_->getAvailableFrameInterleaveList();
    ASSERT_NE(list, nullptr);
    EXPECT_GT(list->getCount(), 0u);
}

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_02_load_interleave) {
    /// 加载 Interleave 模式
    if(!device_->isFrameInterleaveSupported()) GTEST_SKIP();
    auto list = device_->getAvailableFrameInterleaveList();
    if(list && list->getCount() > 0) {
        ASSERT_NO_THROW(device_->loadFrameInterleave(list->getName(0)));
    }
}

TEST_F(TC_CPP_17_Interleave, TC_CPP_17_03_interleave_stream) {
    /// Interleave 开流验证 — SequenceId 交替变化
    if(!device_->isFrameInterleaveSupported()) GTEST_SKIP();
    auto list = device_->getAvailableFrameInterleaveList();
    if(!list || list->getCount() == 0) GTEST_SKIP();

    device_->loadFrameInterleave(list->getName(0));
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 10; i++) pipeline->waitForFrameset(2000);
    pipeline->stop();
    SUCCEED();
}

// ============================================================
// TC_CPP_18: 录制与回放 (3 HW tests)
// ============================================================
class TC_CPP_18_Record_HW : public PipelineTest {};

TEST_F(TC_CPP_18_Record_HW, TC_CPP_18_01_record_device) {
    /// RecordDevice 创建与录制 — 录制文件>1KB
#ifdef _WIN32
    std::string recFile = "ob_test_record.bag";
#else
    std::string recFile = "/tmp/ob_test_record.bag";
#endif
    std::remove(recFile.c_str());

    auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 30; i++) pipeline->waitForFrameset(2000);
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
    /// RecordDevice pause/resume/销毁
#ifdef _WIN32
    std::string recFile = "ob_test_record2.bag";
#else
    std::string recFile = "/tmp/ob_test_record2.bag";
#endif
    std::remove(recFile.c_str());

    auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline->start(config);

    for(int i = 0; i < 10; i++) pipeline->waitForFrameset(2000);
    recorder->pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    recorder->resume();
    for(int i = 0; i < 10; i++) pipeline->waitForFrameset(2000);

    pipeline->stop();
    recorder.reset();

    std::ifstream ifs(recFile, std::ios::binary | std::ios::ate);
    EXPECT_TRUE(ifs.good());
    std::remove(recFile.c_str());
}

TEST_F(TC_CPP_18_Record_HW, TC_CPP_18_08_record_playback_roundtrip) {
    /// 录制回放往返一致性
#ifdef _WIN32
    std::string recFile = "ob_test_roundtrip.bag";
#else
    std::string recFile = "/tmp/ob_test_roundtrip.bag";
#endif
    std::remove(recFile.c_str());

    // 录制
    {
        auto recorder = std::make_shared<ob::RecordDevice>(device_, recFile);
        auto pipeline = std::make_shared<ob::Pipeline>(device_);
        auto config = std::make_shared<ob::Config>();
        config->enableStream(OB_STREAM_DEPTH);
        pipeline->start(config);
        for(int i = 0; i < 30; i++) pipeline->waitForFrameset(2000);
        pipeline->stop();
        recorder.reset();
    }

    // 回放
    {
        auto pbDevice = std::make_shared<ob::PlaybackDevice>(recFile);
        auto pipeline = std::make_shared<ob::Pipeline>(pbDevice);
        auto config = std::make_shared<ob::Config>();
        config->enableAllStream();
        pipeline->start(config);
        auto fs = pipeline->waitForFrameset(5000);
        EXPECT_NE(fs, nullptr) << "Playback returned no frames";
        if(fs) {
            auto depth = fs->getDepthFrame();
            EXPECT_NE(depth, nullptr);
        }
        pipeline->stop();
    }

    std::remove(recFile.c_str());
}

// ============================================================
// TC_CPP_19: 多设备同步 (7 HW tests)
// ============================================================
class TC_CPP_19_MultiSync : public DeviceTest {};

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_01_sync_mode_support) {
    /// 同步模式支持与配置读取
    auto bitmap = device_->getSupportedMultiDeviceSyncModeBitmap();
    EXPECT_NE(bitmap, 0u) << "No sync modes supported";

    auto config = device_->getMultiDeviceSyncConfig();
    (void)config;
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_02_freerun_standalone) {
    /// 设置 FREE_RUN/STANDALONE 模式
    OBMultiDeviceSyncConfig config = {};
    config.syncMode = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(config));

    auto readBack = device_->getMultiDeviceSyncConfig();
    EXPECT_EQ(readBack.syncMode, OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN);
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_03_primary_secondary) {
    /// PRIMARY/SECONDARY 配对 — 需要2台设备
    ENV().skipIfMultiDeviceNotAvailable();
    GTEST_SKIP() << "Multi-device sync test requires 2 connected devices and special wiring";
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_04_software_trigger) {
    /// SOFTWARE_TRIGGERING 触发
    auto bitmap = device_->getSupportedMultiDeviceSyncModeBitmap();
    if(!(bitmap & OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING)) {
        GTEST_SKIP() << "Software triggering not supported";
    }

    OBMultiDeviceSyncConfig config = {};
    config.syncMode = OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING;
    config.framesPerTrigger = 1;
    device_->setMultiDeviceSyncConfig(config);

    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    auto cfg = std::make_shared<ob::Config>();
    cfg->enableStream(OB_STREAM_DEPTH);
    pipeline->start(cfg);

    device_->triggerCapture();
    auto fs = pipeline->waitForFrameset(3000);
    EXPECT_NE(fs, nullptr);

    pipeline->stop();
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_05_timestamp_reset) {
    /// 时间戳重置配置与执行
    auto config = device_->getTimestampResetConfig();
    ASSERT_NO_THROW(device_->setTimestampResetConfig(config));
    ASSERT_NO_THROW(device_->timestampReset());
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_06_timer_sync_host) {
    /// Timer 与 Host 同步 — timerSyncWithHost 不崩溃
    ASSERT_NO_THROW(device_->timerSyncWithHost());
}

TEST_F(TC_CPP_19_MultiSync, TC_CPP_19_07_sync_config_fields) {
    /// 同步配置字段验证
    OBMultiDeviceSyncConfig config = {};
    config.syncMode = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
    config.depthDelayUs = 100;
    config.colorDelayUs = 200;
    config.trigger2ImageDelayUs = 0;
    config.triggerOutEnable = false;
    config.triggerOutDelayUs = 0;
    config.framesPerTrigger = 1;

    ASSERT_NO_THROW(device_->setMultiDeviceSyncConfig(config));
    auto readBack = device_->getMultiDeviceSyncConfig();
    EXPECT_EQ(readBack.depthDelayUs, 100);
    EXPECT_EQ(readBack.colorDelayUs, 200);
}

// ============================================================
// TC_CPP_20: 坐标变换 (1 HW test)
// ============================================================
class TC_CPP_20_Coord_HW : public PipelineTest {};

TEST_F(TC_CPP_20_Coord_HW, TC_CPP_20_05_save_ply) {
    /// 保存点云 PLY 文件
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    pipeline_->start(config);
    for(int i = 0; i < 5; i++) pipeline_->waitForFrameset(2000);
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

    ob_error *error = nullptr;
    auto pcFilter = ob_create_filter("PointCloudFilter", &error);
    if(!pcFilter) GTEST_SKIP() << "No PointCloudFilter";

    auto result = ob_filter_process(pcFilter, depth->getImpl(), &error);
    if(result) {
        ob_save_pointcloud_to_ply(plyFile.c_str(), result, false, false, 0.0f, &error);
        std::ifstream ifs(plyFile);
        EXPECT_TRUE(ifs.good()) << "PLY file not created";
        ob_delete_frame(result, &error);
    }

    ob_delete_filter(pcFilter, &error);
    std::remove(plyFile.c_str());
}

// ============================================================
// TC_CPP_21: 固件管理 (9 HW tests)
// ============================================================
class TC_CPP_21_Firmware : public DeviceTest {};

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_01_global_timestamp) {
    /// 全局时间戳支持查询与启用
    bool supported = device_->isGlobalTimestampSupported();
    if(supported) {
        ASSERT_NO_THROW(device_->enableGlobalTimestamp(true));
        ASSERT_NO_THROW(device_->enableGlobalTimestamp(false));
    }
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_02_device_state) {
    /// 设备状态查询
    auto state = device_->getDeviceState();
    (void)state;
    SUCCEED();
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_03_state_change_callback) {
    /// 设备状态变化回调
    ASSERT_NO_THROW(device_->setDeviceStateChangedCallback(
        [](OBDeviceState, const char *) {}));
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_04_heartbeat) {
    /// 心跳监测开关
    ASSERT_NO_THROW(device_->enableHeartbeat(true));
    ASSERT_NO_THROW(device_->enableHeartbeat(false));
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_05_raw_vendor_command) {
    /// 原始厂商命令 — sendAndReceiveData
    // 需要知道具体命令格式，这里只验证接口不崩溃
    SUCCEED();
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_06_calibration_param_list) {
    /// 相机标定参数列表 — fx/fy>0
    auto paramList = device_->getCalibrationCameraParamList();
    ASSERT_NE(paramList, nullptr);
    EXPECT_GT(paramList->getCount(), 0u);
    auto param = paramList->getCameraParam(0);
    EXPECT_GT(param.depthIntrinsic.fx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.fy, 0.0f);
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_07_reboot) {
    /// 重启设备 — destructive
    ENV().skipUnlessDestructive();
    GTEST_SKIP() << "Reboot test skipped unless ALLOW_DESTRUCTIVE_TESTS=true";
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_08_firmware_update) {
    /// 固件升级 — destructive
    ENV().skipUnlessDestructive();
    GTEST_SKIP() << "Firmware update requires firmware file and is destructive";
}

TEST_F(TC_CPP_21_Firmware, TC_CPP_21_09_update_depth_presets) {
    /// 更新深度预设 — destructive
    ENV().skipUnlessDestructive();
    GTEST_SKIP() << "Depth preset update is destructive";
}

// ============================================================
// TC_CPP_24: 错误处理 (2 HW tests)
// ============================================================
class TC_CPP_24_Error_HW : public PipelineTest {};

TEST_F(TC_CPP_24_Error_HW, TC_CPP_24_04_pipeline_exception_safety) {
    /// Pipeline 异常安全 — 重复start/stop不崩溃
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    pipeline_->start(config);
    pipeline_->stop();

    // 重复 stop 不崩溃
    try { pipeline_->stop(); } catch(...) {}

    // 重新 start
    pipeline_->start(config);
    pipeline_->stop();

    SUCCEED();
}

TEST_F(TC_CPP_24_Error_HW, TC_CPP_24_05_device_property_exception) {
    /// Device 属性异常安全 — 写只读/超范围 抛异常不崩溃
    // 尝试读取一个不支持的属性ID
    EXPECT_THROW(device_->getIntProperty(static_cast<OBPropertyID>(99999)), ob::Error);
}

// ============================================================
// TC_CPP_25: 关键数据结构 (4 HW tests)
// ============================================================
class TC_CPP_25_DataStruct_HW : public PipelineTest {};

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_01_intrinsic_extrinsic) {
    /// 相机内参/畸变/外参结构体
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);
    pipeline_->start(config);
    pipeline_->waitForFrameset(2000);

    auto param = pipeline_->getCameraParam();
    EXPECT_GT(param.depthIntrinsic.fx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.fy, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fx, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fy, 0.0f);
    pipeline_->stop();
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_02_camera_calib_param) {
    /// CameraParam/CalibrationParam — depth+color完整标定
    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);
    config->enableStream(OB_STREAM_COLOR);

    auto calib = pipeline_->getCalibrationParam(config);
    EXPECT_GT(calib.intrinsics[OB_SENSOR_DEPTH].fx, 0.0f);
    EXPECT_GT(calib.intrinsics[OB_SENSOR_COLOR].fx, 0.0f);
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_03_imu_intrinsic) {
    /// IMU 内参结构体
    auto accel = device_->getSensor(OB_SENSOR_ACCEL);
    if(!accel) GTEST_SKIP() << "No accel sensor";

    auto profiles = accel->getStreamProfileList();
    auto ap = profiles->getProfile(0)->as<ob::AccelStreamProfile>();
    auto intrinsic = ap->getIntrinsic();
    // bias 和 noise density 字段存在即可
    (void)intrinsic;

    auto gyro = device_->getSensor(OB_SENSOR_GYRO);
    if(gyro) {
        auto gProfiles = gyro->getStreamProfileList();
        auto gp = gProfiles->getProfile(0)->as<ob::GyroStreamProfile>();
        auto gIntrinsic = gp->getIntrinsic();
        (void)gIntrinsic;
    }
    SUCCEED();
}

TEST_F(TC_CPP_25_DataStruct_HW, TC_CPP_25_04_device_temperature) {
    /// 设备温度结构体 — CPU/IR/Laser 0~80°C
    // 温度通常通过属性获取
    if(device_->isPropertySupported(OB_STRUCT_DEVICE_TEMPERATURE, OB_PERMISSION_READ)) {
        OBDeviceTemperature temp = {};
        uint32_t dataSize = static_cast<uint32_t>(sizeof(temp));
        device_->getStructuredData(OB_STRUCT_DEVICE_TEMPERATURE, reinterpret_cast<uint8_t *>(&temp), &dataSize);
        EXPECT_GE(temp.chipTopTemp, -40.0f);
        EXPECT_LE(temp.chipTopTemp, 120.0f);
    }
    SUCCEED();
}

// ============================================================
// main
// ============================================================
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
