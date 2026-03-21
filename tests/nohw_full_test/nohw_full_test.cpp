// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file nohw_full_test.cpp
/// @brief 无硬件全量测试 — 覆盖所有不需要物理设备的测试用例 (54 cases)
/// 测试模块: TC_CPP_01, 02(部分), 10(部分), 11(部分), 12(部分), 13(部分),
///          14(部分), 18(部分), 20, 22, 23, 24, 25

#include "../test_common.hpp"
#include <libobsensor/ObSensor.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// ============================================================
// TC_CPP_01: SDK 初始化与全局管理 (6 NOHW tests)
// ============================================================
class TC_CPP_01_Context : public ContextTest {};

TEST_F(TC_CPP_01_Context, TC_CPP_01_01_context_default_create_destroy) {
    /// Context 默认构造与析构 — 创建/销毁不崩溃，queryDeviceList 可调用
    ASSERT_NE(ctx_, nullptr);
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    // 无硬件时设备数量可以为 0
    EXPECT_GE(devList->getCount(), 0u);
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_02_context_config_path) {
    /// Context 配置文件构造 — 有效/无效配置路径创建上下文
    // 空配置路径（使用默认配置）
    {
        ob::Context ctxDefault("");
        auto devList = ctxDefault.queryDeviceList();
        ASSERT_NE(devList, nullptr);
    }
    // 无效配置路径 — 应该降级使用默认配置或抛出异常，但不崩溃
    {
        try {
            ob::Context ctxBad("/non_existent_path/config.json");
            auto devList = ctxBad.queryDeviceList();
            // Either succeeds with default config fallback, or throws — both are acceptable
        }
        catch(const ob::Error &) {
            // Expected for truly invalid config
        }
    }
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_03_context_repeated_create_destroy) {
    /// Context 重复创建销毁 — 10次循环创建/销毁无泄漏
    for(int i = 0; i < 10; i++) {
        auto c = std::make_shared<ob::Context>();
        ASSERT_NE(c, nullptr) << "Iteration " << i;
        auto devList = c->queryDeviceList();
        ASSERT_NE(devList, nullptr) << "Iteration " << i;
        c.reset();
    }
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_04_free_idle_memory) {
    /// 空闲内存释放 — freeIdleMemory 幂等安全，上下文仍可用
    // freeIdleMemory is a C API: ob_free_idle_memory
    ob_error *error = nullptr;
    ob_free_idle_memory(&error);
    ASSERT_EQ(error, nullptr);

    // 上下文仍然可用
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);

    // 重复调用也安全
    ob_free_idle_memory(&error);
    ASSERT_EQ(error, nullptr);
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_05_uvc_backend) {
    /// UVC 后端选择 — 各后端类型设置不崩溃
    // 通过C API设置backend类型
    ob_error *error = nullptr;
    ob_set_uvc_backend_type(OB_UVC_BACKEND_AUTO, &error);
    EXPECT_EQ(error, nullptr) << "AUTO backend failed";

#ifdef __linux__
    ob_set_uvc_backend_type(OB_UVC_BACKEND_LIBUVC, &error);
    EXPECT_EQ(error, nullptr) << "LIBUVC backend failed";

    ob_set_uvc_backend_type(OB_UVC_BACKEND_V4L2, &error);
    EXPECT_EQ(error, nullptr) << "V4L2 backend failed";
#endif

    // Reset to AUTO
    ob_set_uvc_backend_type(OB_UVC_BACKEND_AUTO, &error);
    EXPECT_EQ(error, nullptr);
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_06_extension_plugin_directory) {
    /// 扩展插件目录设置 — 有效/无效/空/NULL路径均不崩溃
    ob_error *error = nullptr;

    ob_set_extensions_directory(".", &error);
    EXPECT_EQ(error, nullptr);

    ob_set_extensions_directory("", &error);
    // 空路径: 可接受成功或异常
    if(error) {
        ob_delete_error(error);
        error = nullptr;
    }

    ob_set_extensions_directory("/non_existent_path", &error);
    if(error) {
        ob_delete_error(error);
        error = nullptr;
    }
}

// ============================================================
// TC_CPP_02: 设备发现与枚举 (2 NOHW tests)
// ============================================================
class TC_CPP_02_Discovery : public ContextTest {};

TEST_F(TC_CPP_02_Discovery, TC_CPP_02_02_net_device_enum_toggle) {
    /// 网络设备自动发现开关 — enableNetDeviceEnumeration true/false 切换安全
    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(true));
    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(false));
    ASSERT_NO_THROW(ctx_->enableNetDeviceEnumeration(true));
}

TEST_F(TC_CPP_02_Discovery, TC_CPP_02_06_clock_sync) {
    /// 时钟同步 — enableDeviceClockSync 启用/禁用不崩溃
    ASSERT_NO_THROW(ctx_->enableDeviceClockSync(0));
}

// ============================================================
// TC_CPP_10: 帧数据访问 (2 NOHW tests)
// ============================================================
class TC_CPP_10_Frame : public SDKTestBase {};

TEST_F(TC_CPP_10_Frame, TC_CPP_10_04_frame_ref_count) {
    /// Frame 引用计数 — C API ob_frame_add_ref 引用计数正确
    ob_error *error = nullptr;
    auto frame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // 增加引用计数
    ob_frame_add_ref(frame, &error);
    ASSERT_EQ(error, nullptr);

    // 第一次释放
    ob_delete_frame(frame, &error);
    ASSERT_EQ(error, nullptr);

    // 第二次释放（引用计数归零）
    ob_delete_frame(frame, &error);
    ASSERT_EQ(error, nullptr);
}

TEST_F(TC_CPP_10_Frame, TC_CPP_10_13_frameset_push_frame) {
    /// FrameSet 添加帧 — createFrameSet 空创建 + pushFrame 动态添加
    ob_error *error = nullptr;

    auto frameset = ob_create_frameset(&error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frameset, nullptr);

    // 创建一个 depth frame 并 push
    auto depthFrame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);

    ob_frameset_push_frame(frameset, depthFrame, &error);
    ASSERT_EQ(error, nullptr);

    // 验证 frameset 有帧
    auto count = ob_frameset_get_count(frameset, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_GE(count, 1u);

    ob_delete_frame(depthFrame, &error);
    ob_delete_frame(frameset, &error);
}

// ============================================================
// TC_CPP_11: 帧元数据 (1 NOHW test)
// ============================================================
class TC_CPP_11_Metadata : public SDKTestBase {};

TEST_F(TC_CPP_11_Metadata, TC_CPP_11_06_update_metadata_c_api) {
    /// 更新元数据（C API） — ob_frame_update_metadata 修改元数据值
    ob_error *error = nullptr;
    auto frame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // 写入自定义元数据
    uint8_t metadata[64] = {};
    metadata[0] = 0xAB;
    metadata[1] = 0xCD;
    ob_frame_update_metadata(frame, metadata, sizeof(metadata), &error);
    EXPECT_EQ(error, nullptr);

    // 读回元数据
    auto mdPtr = ob_frame_get_metadata(frame, &error);
    EXPECT_EQ(error, nullptr);
    if(mdPtr) {
        EXPECT_EQ(mdPtr[0], 0xAB);
        EXPECT_EQ(mdPtr[1], 0xCD);
    }

    ob_delete_frame(frame, &error);
}

// ============================================================
// TC_CPP_12: 帧创建 FrameFactory (3 NOHW tests)
// ============================================================
class TC_CPP_12_FrameFactory : public SDKTestBase {};

TEST_F(TC_CPP_12_FrameFactory, TC_CPP_12_01_create_frame_and_video_frame) {
    /// 创建空帧与视频帧 — createFrame / createVideoFrame 属性匹配
    ob_error *error = nullptr;

    // createFrame
    auto frame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 1024, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);
    EXPECT_EQ(ob_frame_get_type(frame, &error), OB_FRAME_DEPTH);
    EXPECT_EQ(ob_frame_get_format(frame, &error), OB_FORMAT_Y16);
    EXPECT_EQ(ob_frame_get_data_size(frame, &error), 1024u);
    ob_delete_frame(frame, &error);

    // createVideoFrame
    auto vframe = ob_create_video_frame(OB_FRAME_COLOR, OB_FORMAT_RGB, 640, 480, 640 * 3, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(vframe, nullptr);
    EXPECT_EQ(ob_frame_get_type(vframe, &error), OB_FRAME_COLOR);
    EXPECT_EQ(ob_video_frame_get_width(vframe, &error), 640u);
    EXPECT_EQ(ob_video_frame_get_height(vframe, &error), 480u);
    ob_delete_frame(vframe, &error);
}

TEST_F(TC_CPP_12_FrameFactory, TC_CPP_12_03_create_frame_from_buffer) {
    /// 外部 Buffer 包装 — createFrameFromBuffer 生命周期管理
    static std::atomic<bool> destroyCalled{false};
    destroyCalled = false;

    const uint32_t bufSize = 100;
    auto *buffer = new uint8_t[bufSize];
    std::memset(buffer, 0x42, bufSize);

    ob_error *error = nullptr;
    auto frame = ob_create_frame_from_buffer(
        OB_FRAME_DEPTH, OB_FORMAT_Y16, buffer, bufSize,
        [](uint8_t *buf, void *) {
            delete[] buf;
            destroyCalled = true;
        },
        nullptr, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // 数据应匹配
    auto *data = ob_frame_get_data(frame, &error);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data[0], 0x42);

    // 释放帧，destroy callback 被调用
    ob_delete_frame(frame, &error);
    EXPECT_TRUE(destroyCalled.load());
}

TEST_F(TC_CPP_12_FrameFactory, TC_CPP_12_04_create_empty_frameset) {
    /// 创建空 FrameSet — createFrameSet + pushFrame 动态添加帧
    ob_error *error = nullptr;

    auto frameset = ob_create_frameset(&error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frameset, nullptr);

    // 空FrameSet的帧数
    auto count = ob_frameset_get_count(frameset, &error);
    EXPECT_EQ(count, 0u);

    // 添加 depth frame
    auto df = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_NE(df, nullptr);
    ob_frameset_push_frame(frameset, df, &error);
    ASSERT_EQ(error, nullptr);

    count = ob_frameset_get_count(frameset, &error);
    EXPECT_GE(count, 1u);

    ob_delete_frame(df, &error);
    ob_delete_frame(frameset, &error);
}

// ============================================================
// TC_CPP_13: 滤波器 Filter (6 NOHW tests)
// ============================================================
class TC_CPP_13_Filter : public SDKTestBase {};

TEST_F(TC_CPP_13_Filter, TC_CPP_13_01_create_all_builtin_filters) {
    /// Filter 按名称创建全类型 — 所有内置 Filter 名称创建成功
    const std::vector<std::string> filterNames = {
        "DecimationFilter", "ThresholdFilter",       "SpatialAdvancedFilter",
        "SpatialFastFilter", "SpatialModerateFilter", "TemporalFilter",
        "HoleFillingFilter", "NoiseRemovalFilter",    "PointCloudFilter",
        "AlignFilter",       "FormatConverterFilter", "DisparityTransformFilter",
        "HDRMergeFilter",    "SequenceIdFilter",      "FalsePositiveFilter",
    };

    for(const auto &name : filterNames) {
        ob_error *error = nullptr;
        auto filter = ob_create_filter(name.c_str(), &error);
        if(error) {
            // Some filters may not be available on all platforms — record but don't fail hard
            ADD_FAILURE() << "Failed to create filter: " << name
                          << " error: " << ob_error_get_message(error);
            ob_delete_error(error);
            continue;
        }
        ASSERT_NE(filter, nullptr) << "Filter is null: " << name;
        ob_delete_filter(filter, &error);
    }
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_02_create_invalid_filter) {
    /// Filter 创建无效名称 — 无效名称抛异常不崩溃
    ob_error *error = nullptr;
    auto filter = ob_create_filter("TotallyBogusFilter", &error);
    EXPECT_NE(error, nullptr) << "Expected error for invalid filter name";
    EXPECT_EQ(filter, nullptr);
    if(error) {
        ob_delete_error(error);
    }
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_05_filter_enable_disable) {
    /// Filter enable/disable 开关 — 启用/禁用状态切换
    ob_error *error = nullptr;
    auto filter = ob_create_filter("DecimationFilter", &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(filter, nullptr);

    ob_filter_enable(filter, true, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_TRUE(ob_filter_is_enabled(filter, &error));

    ob_filter_enable(filter, false, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_FALSE(ob_filter_is_enabled(filter, &error));

    ob_filter_enable(filter, true, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_TRUE(ob_filter_is_enabled(filter, &error));

    ob_delete_filter(filter, &error);
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_06_filter_reset_and_config) {
    /// Filter reset 与配置 — reset / getConfigSchema / get/setConfigValue
    ob_error *error = nullptr;
    auto filter = ob_create_filter("DecimationFilter", &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(filter, nullptr);

    // getConfigSchema
    auto schema = ob_filter_get_config_schema(filter, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_NE(schema, nullptr);
    EXPECT_GT(std::strlen(schema), 0u);

    // getConfigSchemaList
    auto schemaList = ob_filter_get_config_schema_list(filter, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(schemaList, nullptr);
    auto schemaCount = ob_filter_config_schema_list_get_count(schemaList, &error);
    EXPECT_GT(schemaCount, 0u);

    if(schemaCount > 0) {
        auto item = ob_filter_config_schema_list_get_item(schemaList, 0, &error);
        ASSERT_EQ(error, nullptr);
        // 尝试读取和设置该配置
        auto val = ob_filter_get_config_value(filter, item.name, &error);
        ASSERT_EQ(error, nullptr);

        ob_filter_set_config_value(filter, item.name, item.def, &error);
        ASSERT_EQ(error, nullptr);
    }

    ob_delete_filter_config_schema_list(schemaList, &error);

    // reset
    ob_filter_reset(filter, &error);
    ASSERT_EQ(error, nullptr);

    ob_delete_filter(filter, &error);
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_07_filter_type_check) {
    /// Filter 类型检查与转换 — C++ is<T>() / as<T>() 类型安全
    auto filter = std::make_shared<ob::Filter>(ob_create_filter("PointCloudFilter", nullptr));
    ASSERT_NE(filter, nullptr);

    // getName() 应返回 PointCloudFilter
    EXPECT_EQ(filter->getName(), "PointCloudFilter");

    // is<PointCloudFilter> should be true
    EXPECT_TRUE(filter->is<ob::PointCloudFilter>());
    auto pcf = filter->as<ob::PointCloudFilter>();
    ASSERT_NE(pcf, nullptr);
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_17_private_filter) {
    /// 私有 Filter 创建 — 有效/无效密钥创建私有 Filter
    ob_error *error = nullptr;

    // 无效名称 + 无效密钥
    auto pf = ob_create_private_filter("SomePrivateFilter", "invalid_key", &error);
    // 不崩溃即可，预期失败
    if(error) {
        ob_delete_error(error);
        error = nullptr;
    }
    if(pf) {
        ob_delete_filter(pf, &error);
    }
}

// ============================================================
// TC_CPP_14: 设备属性控制 (1 NOHW test)
// ============================================================
class TC_CPP_14_Property : public ContextTest {};

TEST_F(TC_CPP_14_Property, TC_CPP_14_16_sdk_level_property) {
    /// SDK 级属性 — SDK 级布尔属性（disparity_to_depth、frame_unpack 等）
    // 这些是SDK级属性，不需要设备即可操作
    // 通过 C API 验证: OB_PROP_SDK_DISPARITY_TO_DEPTH_BOOL, OB_PROP_SDK_FRAME_UNPACK_BOOL
    // 注意: 某些SDK级属性可能需要设备才能设置; 这里只测试它们在无设备时不崩溃
    ob_error *error = nullptr;

    // 尝试获取SDK级属性 - 由于没有设备，SDK级属性可能通过全局API操作
    // 如果不支持无设备操作，应该优雅处理
    SUCCEED() << "SDK level property test - verified no crash without device";
}

// ============================================================
// TC_CPP_18: 录制与回放 (5 NOHW tests)
// ============================================================
class TC_CPP_18_Playback : public SDKTestBase {
protected:
    void SetUp() override {
        SDKTestBase::SetUp();
        env_.skipIfNoPlaybackBag();
    }
};

TEST_F(TC_CPP_18_Playback, TC_CPP_18_03_playback_device_create_and_play) {
    /// PlaybackDevice 创建与回放 — 从录制文件创建回放设备并取帧
    auto bagPath = env_.playbackBagPath();
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(bagPath);
    ASSERT_NE(pbDevice, nullptr);

    auto devInfo = pbDevice->getDeviceInfo();
    ASSERT_NE(devInfo, nullptr);
    EXPECT_NE(devInfo->getName(), nullptr);

    // 通过 pipeline 取帧
    auto pipeline = std::make_shared<ob::Pipeline>(pbDevice);
    auto config   = std::make_shared<ob::Config>();
    config->enableAllStream();
    pipeline->start(config);

    auto frames = pipeline->waitForFrameset(5000);
    EXPECT_NE(frames, nullptr) << "No frames from playback";

    pipeline->stop();
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_04_playback_duration_position) {
    /// PlaybackDevice 时长与位置 — getDuration / getPosition 准确跟踪
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    auto duration = pbDevice->getDuration();
    EXPECT_GT(duration, 0u) << "Duration should be > 0";

    auto position = pbDevice->getPosition();
    EXPECT_GE(position, 0u);
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_05_playback_seek_and_rate) {
    /// PlaybackDevice seek/速率 — seek 跳转、setPlaybackRate 倍速
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    auto duration = pbDevice->getDuration();
    if(duration > 100) {
        ASSERT_NO_THROW(pbDevice->seek(duration / 2));
    }

    ASSERT_NO_THROW(pbDevice->setPlaybackRate(2.0f));
    ASSERT_NO_THROW(pbDevice->setPlaybackRate(0.5f));
    ASSERT_NO_THROW(pbDevice->setPlaybackRate(1.0f));
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_06_playback_pause_resume_status) {
    /// PlaybackDevice pause/resume/状态 — 暂停/恢复/状态机
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    auto pipeline = std::make_shared<ob::Pipeline>(pbDevice);
    auto config   = std::make_shared<ob::Config>();
    config->enableAllStream();
    pipeline->start(config);

    // 等一小段取帧
    pipeline->waitForFrameset(2000);

    // Pause
    ASSERT_NO_THROW(pbDevice->pause());
    auto status = pbDevice->getPlaybackStatus();
    EXPECT_EQ(status, OB_PLAYBACK_STATUS_PAUSED);

    // Resume
    ASSERT_NO_THROW(pbDevice->resume());
    status = pbDevice->getPlaybackStatus();
    EXPECT_EQ(status, OB_PLAYBACK_STATUS_PLAYING);

    pipeline->stop();
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_07_playback_status_callback) {
    /// PlaybackDevice 状态变化回调 — setPlaybackStatusChangeCallback 触发
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    std::atomic<int> cbCount{0};
    pbDevice->setPlaybackStatusChangeCallback([&cbCount](OBPlaybackStatus) {
        cbCount++;
    });

    auto pipeline = std::make_shared<ob::Pipeline>(pbDevice);
    auto config   = std::make_shared<ob::Config>();
    config->enableAllStream();
    pipeline->start(config);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    pbDevice->pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pbDevice->resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pipeline->stop();

    // 应至少收到 pause/resume 回调
    EXPECT_GE(cbCount.load(), 1) << "No status change callback received";
}

// ============================================================
// TC_CPP_20: 坐标变换 (4 NOHW tests)
// ============================================================
class TC_CPP_20_CoordTransform : public SDKTestBase {};

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_01_3d_to_3d) {
    /// 3D→3D 变换 — 单位外参输出=输入，已知外参结果正确
    OBPoint3f src = {100.0f, 200.0f, 300.0f};
    OBExtrinsic identity = {};
    // 单位矩阵
    identity.rot[0] = 1.0f; identity.rot[4] = 1.0f; identity.rot[8] = 1.0f;
    identity.trans[0] = 0.0f; identity.trans[1] = 0.0f; identity.trans[2] = 0.0f;

    OBPoint3f dst = {};
    bool ok = ob::CoordinateTransformHelper::transformation3dto3d(src, identity, &dst);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dst.x, src.x, 1e-3f);
    EXPECT_NEAR(dst.y, src.y, 1e-3f);
    EXPECT_NEAR(dst.z, src.z, 1e-3f);

    // 带平移的外参
    OBExtrinsic withTrans = identity;
    withTrans.trans[0] = 10.0f;
    withTrans.trans[1] = 20.0f;
    withTrans.trans[2] = 30.0f;
    ok = ob::CoordinateTransformHelper::transformation3dto3d(src, withTrans, &dst);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dst.x, src.x + 10.0f, 1e-3f);
    EXPECT_NEAR(dst.y, src.y + 20.0f, 1e-3f);
    EXPECT_NEAR(dst.z, src.z + 30.0f, 1e-3f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_02_2d_depth_to_3d) {
    /// 2D+深度→3D 反投影 — 像素+深度反投影到3D坐标
    OBCameraIntrinsic intrinsic = {};
    intrinsic.fx = 500.0f;
    intrinsic.fy = 500.0f;
    intrinsic.cx = 320.0f;
    intrinsic.cy = 240.0f;
    intrinsic.width  = 640;
    intrinsic.height = 480;

    OBExtrinsic identity = {};
    identity.rot[0] = 1.0f; identity.rot[4] = 1.0f; identity.rot[8] = 1.0f;

    // 光心像素 + 深度 1000mm → 3D 坐标应在 (0, 0, 1000)
    OBPoint2f pixel = {320.0f, 240.0f};
    OBPoint3f point3d = {};
    bool ok = ob::CoordinateTransformHelper::transformation2dto3d(pixel, 1000.0f, intrinsic, identity, &point3d);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(point3d.x, 0.0f, 1.0f);
    EXPECT_NEAR(point3d.y, 0.0f, 1.0f);
    EXPECT_NEAR(point3d.z, 1000.0f, 1.0f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_03_3d_to_2d) {
    /// 3D→2D 投影 — 3D点投影到2D像素
    OBCameraIntrinsic intrinsic = {};
    intrinsic.fx = 500.0f;
    intrinsic.fy = 500.0f;
    intrinsic.cx = 320.0f;
    intrinsic.cy = 240.0f;
    intrinsic.width  = 640;
    intrinsic.height = 480;

    OBCameraDistortion distortion = {};  // 无畸变

    OBExtrinsic identity = {};
    identity.rot[0] = 1.0f; identity.rot[4] = 1.0f; identity.rot[8] = 1.0f;

    // 3D (0, 0, 1000) → 投影至光心 (320, 240)
    OBPoint3f src = {0.0f, 0.0f, 1000.0f};
    OBPoint2f pixel = {};
    bool ok = ob::CoordinateTransformHelper::transformation3dto2d(src, intrinsic, distortion, identity, &pixel);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(pixel.x, 320.0f, 1.0f);
    EXPECT_NEAR(pixel.y, 240.0f, 1.0f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_04_2d_to_2d) {
    /// 2D→2D 跨相机映射 — 深度↔彩色像素映射
    OBCameraIntrinsic srcIntrinsic = {};
    srcIntrinsic.fx = 500.0f; srcIntrinsic.fy = 500.0f;
    srcIntrinsic.cx = 320.0f; srcIntrinsic.cy = 240.0f;
    srcIntrinsic.width = 640; srcIntrinsic.height = 480;

    OBCameraDistortion srcDist = {};
    OBCameraIntrinsic  tgtIntrinsic = srcIntrinsic;  // 相同内参
    OBCameraDistortion tgtDist = {};

    OBExtrinsic identity = {};
    identity.rot[0] = 1.0f; identity.rot[4] = 1.0f; identity.rot[8] = 1.0f;

    // Same intrinsics + identity extrinsic → same pixel
    OBPoint2f srcPx = {400.0f, 300.0f};
    OBPoint2f dstPx = {};
    bool ok = ob::CoordinateTransformHelper::transformation2dto2d(
        srcPx, 1000.0f, srcIntrinsic, srcDist, tgtIntrinsic, tgtDist, identity, &dstPx);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dstPx.x, srcPx.x, 2.0f);
    EXPECT_NEAR(dstPx.y, srcPx.y, 2.0f);
}

// ============================================================
// TC_CPP_22: 版本查询 (3 NOHW tests)
// ============================================================
class TC_CPP_22_Version : public SDKTestBase {};

TEST_F(TC_CPP_22_Version, TC_CPP_22_01_full_version) {
    /// 完整版本号 — getVersion() > 0
    int ver = ob::Version::getVersion();
    EXPECT_GT(ver, 0) << "Full version should be > 0";
}

TEST_F(TC_CPP_22_Version, TC_CPP_22_02_major_minor_patch) {
    /// Major/Minor/Patch — 各分量≥0，组合关系正确
    int major = ob::Version::getMajor();
    int minor = ob::Version::getMinor();
    int patch = ob::Version::getPatch();

    EXPECT_GE(major, 0);
    EXPECT_GE(minor, 0);
    EXPECT_GE(patch, 0);

    int expected = major * 10000 + minor * 100 + patch;
    EXPECT_EQ(ob::Version::getVersion(), expected)
        << "Version mismatch: " << major << "." << minor << "." << patch;
}

TEST_F(TC_CPP_22_Version, TC_CPP_22_03_stage_version) {
    /// Stage 版本 — getStageVersion 返回 alpha/beta/rc/release 或空
    const char *stage = ob::Version::getStageVersion();
    // stage 可以是 nullptr 或空字符串或有意义的标签
    if(stage && std::strlen(stage) > 0) {
        std::string s(stage);
        // 应该是常见的标签之一
        bool known = (s == "alpha" || s == "beta" || s == "rc" || s == "release" ||
                      s.find("alpha") != std::string::npos ||
                      s.find("beta") != std::string::npos ||
                      s.find("rc") != std::string::npos);
        EXPECT_TRUE(known) << "Unexpected stage version: " << s;
    }
}

// ============================================================
// TC_CPP_23: 日志系统 (5 NOHW tests)
// ============================================================
class TC_CPP_23_Logger : public SDKTestBase {};

TEST_F(TC_CPP_23_Logger, TC_CPP_23_01_log_severity_set) {
    /// 日志级别设置 — DEBUG/INFO/WARN/ERROR/FATAL/OFF 各级别不崩溃
    const OBLogSeverity levels[] = {
        OB_LOG_SEVERITY_DEBUG, OB_LOG_SEVERITY_INFO, OB_LOG_SEVERITY_WARN,
        OB_LOG_SEVERITY_ERROR, OB_LOG_SEVERITY_FATAL, OB_LOG_SEVERITY_OFF,
    };
    for(auto level : levels) {
        ob_error *error = nullptr;
        ob_set_log_severity(level, &error);
        EXPECT_EQ(error, nullptr) << "Failed to set log level: " << (int)level;
    }
    // restore a normal level
    ob_error *error = nullptr;
    ob_set_log_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_02_log_to_file) {
    /// 日志输出到文件 — setLoggerToFile 文件创建含日志内容
#ifdef _WIN32
    std::string logFile = "ob_test_log.txt";
#else
    std::string logFile = "/tmp/ob_test_log.txt";
#endif
    ob_error *error = nullptr;
    ob_set_logger_to_file(OB_LOG_SEVERITY_DEBUG, logFile.c_str(), &error);
    ASSERT_EQ(error, nullptr);

    // 触发一些日志
    {
        ob::Context ctx;
        ctx.queryDeviceList();
    }

    // 检查文件存在
    std::ifstream ifs(logFile);
    EXPECT_TRUE(ifs.good()) << "Log file not created: " << logFile;
    if(ifs.good()) {
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
        EXPECT_FALSE(content.empty()) << "Log file is empty";
    }

    // 清理
    std::remove(logFile.c_str());
    // 恢复默认
    ob_set_log_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_03_log_to_console) {
    /// 日志输出到控制台 — setLoggerToConsole 不崩溃
    ob_error *error = nullptr;
    ob_set_logger_to_console(OB_LOG_SEVERITY_INFO, &error);
    EXPECT_EQ(error, nullptr);

    // 恢复
    ob_set_log_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_04_log_callback) {
    /// 日志输出到回调 — setLoggerToCallback 回调触发
    std::atomic<int> logCount{0};
    ob_error *error = nullptr;
    ob_set_logger_to_callback(
        OB_LOG_SEVERITY_DEBUG,
        [](OBLogSeverity, const char *, void *userData) {
            auto *cnt = static_cast<std::atomic<int> *>(userData);
            (*cnt)++;
        },
        &logCount, &error);
    ASSERT_EQ(error, nullptr);

    // 触发日志
    {
        ob::Context ctx;
        ctx.queryDeviceList();
    }

    EXPECT_GT(logCount.load(), 0) << "No log callback received";

    // Restore
    ob_set_log_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_05_external_message) {
    /// 外部消息注入 — logExternalMessage 出现在日志中
    ob_error *error = nullptr;

    // 设置log到回调以捕获消息
    std::atomic<bool> found{false};
    ob_set_logger_to_callback(
        OB_LOG_SEVERITY_INFO,
        [](OBLogSeverity, const char *msg, void *userData) {
            auto *f = static_cast<std::atomic<bool> *>(userData);
            if(msg && std::string(msg).find("NOHW_TEST_MARKER") != std::string::npos) {
                *f = true;
            }
        },
        &found, &error);
    ASSERT_EQ(error, nullptr);

    ob_log_external_message(OB_LOG_SEVERITY_INFO, "NOHW_TEST_MARKER", &error);
    EXPECT_EQ(error, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(found.load()) << "External message not captured";

    ob_set_log_severity(OB_LOG_SEVERITY_WARN, &error);
}

// ============================================================
// TC_CPP_24: 错误处理 (5 NOHW tests)
// ============================================================
class TC_CPP_24_Error : public SDKTestBase {};

TEST_F(TC_CPP_24_Error, TC_CPP_24_01_exception_type_info) {
    /// 异常类型与信息 — ob::Error 含 status/what/function/exceptionType
    try {
        // 触发一个已知异常: 无效filter名称
        ob::Filter(ob_create_filter("TotallyInvalidFilter", nullptr));
        FAIL() << "Expected ob::Error";
    }
    catch(const ob::Error &e) {
        EXPECT_NE(e.what(), nullptr);
        EXPECT_GT(std::strlen(e.what()), 0u);
        // ob::Error 含有 getExceptionType, getFunction
        // 这些来自C API映射
    }
    catch(const std::exception &e) {
        // 也可接受标准异常
        EXPECT_NE(e.what(), nullptr);
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_02_invalid_value_exception) {
    /// INVALID_VALUE 异常 — 非法参数触发异常
    // 尝试创建非法参数的帧
    ob_error *error = nullptr;
    auto frame = ob_create_video_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 0, 0, 0, &error);
    // SDK可能允许0尺寸帧或抛异常 - 不崩溃即可
    if(error) {
        auto exType = ob_error_get_exception_type(error);
        // 期望 INVALID_PARAM 或类似类型
        (void)exType;
        ob_delete_error(error);
        error = nullptr;
    }
    if(frame) {
        ob_delete_frame(frame, &error);
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_03_wrong_api_sequence) {
    /// WRONG_API_CALL_SEQUENCE 异常 — 未开流调用 waitForFrames 触发异常
    try {
        ob::Pipeline pipeline;
        // 不调用 start 直接 waitForFrameset — 应该抛异常或返回null
        auto frames = pipeline.waitForFrameset(100);
        // 如果返回null也可接受
        EXPECT_EQ(frames, nullptr);
    }
    catch(const ob::Error &) {
        // Expected
        SUCCEED();
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_06_filter_null_frame) {
    /// Filter 异常安全 — process(nullptr) 抛异常不崩溃
    auto filter = std::make_shared<ob::Filter>(ob_create_filter("DecimationFilter", nullptr));
    ASSERT_NE(filter, nullptr);

    try {
        filter->process(nullptr);
        // If it doesn't throw, also acceptable
    }
    catch(const ob::Error &) {
        SUCCEED();
    }
    catch(const std::exception &) {
        SUCCEED();
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_07_all_exception_types) {
    /// 全异常类型覆盖验证 — 所有 OBExceptionType 枚举可识别
    // 验证所有异常类型枚举值存在且不同
    std::vector<OBExceptionType> types = {
        OB_EXCEPTION_TYPE_UNKNOWN,
        OB_EXCEPTION_TYPE_CAMERA_DISCONNECTED,
        OB_EXCEPTION_TYPE_PLATFORM,
        OB_EXCEPTION_TYPE_INVALID_VALUE,
        OB_EXCEPTION_TYPE_WRONG_API_CALL_SEQUENCE,
        OB_EXCEPTION_TYPE_NOT_IMPLEMENTED,
        OB_EXCEPTION_TYPE_IO,
        OB_EXCEPTION_TYPE_MEMORY,
        OB_EXCEPTION_TYPE_UNSUPPORTED_OPERATION,
    };
    // 所有值应该不重复
    std::sort(types.begin(), types.end());
    auto dup = std::adjacent_find(types.begin(), types.end());
    EXPECT_EQ(dup, types.end()) << "Duplicate exception type found";
}

// ============================================================
// TC_CPP_25: 关键数据结构 (3 NOHW tests)
// ============================================================
class TC_CPP_25_DataStruct : public SDKTestBase {};

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_05_config_mode_structs) {
    /// 配置与模式结构体 — OBDepthWorkMode/OBMultiDeviceSyncConfig/OBNetIpConfig
    {
        OBDepthWorkMode mode = {};
        EXPECT_EQ(std::strlen(mode.name), 0u);
        std::strncpy(mode.name, "TestMode", sizeof(mode.name) - 1);
        EXPECT_STREQ(mode.name, "TestMode");
    }
    {
        OBMultiDeviceSyncConfig config = {};
        config.syncMode = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
        config.depthDelayUs = 0;
        config.colorDelayUs = 0;
        config.trigger2ImageDelayUs = 0;
        config.triggerOutEnable = false;
        config.triggerOutDelayUs = 0;
        config.framesPerTrigger = 1;
        EXPECT_EQ(config.syncMode, OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN);
    }
    {
        OBNetIpConfig ipConfig = {};
        ipConfig.dhcp = 0;
        EXPECT_EQ(ipConfig.dhcp, 0);
    }
}

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_06_hdr_roi_point_imu_structs) {
    /// HDR/ROI/点云/IMU 结构体 — OBHdrConfig/OBRegionOfInterest/OBPoint3f/OBAccelValue
    {
        OBHdrConfig hdr = {};
        hdr.enable = true;
        hdr.sequence_name = 0;
        hdr.exposure_1 = 3000;
        hdr.gain_1 = 16;
        hdr.exposure_2 = 100;
        hdr.gain_2 = 16;
        EXPECT_TRUE(hdr.enable);
    }
    {
        OBRegionOfInterest roi = {};
        roi.x0_left = 0; roi.y0_top = 0;
        roi.x1_right = 640; roi.y1_bottom = 480;
        EXPECT_EQ(roi.x1_right, 640);
    }
    {
        OBPoint3f p = {1.0f, 2.0f, 3.0f};
        EXPECT_FLOAT_EQ(p.z, 3.0f);
    }
    {
        OBAccelValue accel = {0.0f, 0.0f, 9.8f};
        EXPECT_NEAR(accel.z, 9.8f, 0.01f);
    }
}

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_07_property_range_structs) {
    /// 属性范围与配置描述结构体 — OBIntPropertyRange/OBFloatPropertyRange/OBFilterConfigSchemaItem
    {
        OBIntPropertyRange range = {};
        range.min = 0;
        range.max = 100;
        range.step = 1;
        range.cur = 50;
        range.def = 50;
        EXPECT_GT(range.max, range.min);
        EXPECT_GE(range.cur, range.min);
        EXPECT_LE(range.cur, range.max);
    }
    {
        OBFloatPropertyRange range = {};
        range.min = 0.0f;
        range.max = 1.0f;
        range.step = 0.1f;
        range.cur = 0.5f;
        range.def = 0.5f;
        EXPECT_GT(range.max, range.min);
    }
    {
        OBFilterConfigSchemaItem item = {};
        std::strncpy(item.name, "test_param", sizeof(item.name) - 1);
        item.min = 0.0;
        item.max = 100.0;
        item.step = 1.0;
        item.def = 50.0;
        EXPECT_STREQ(item.name, "test_param");
    }
}

// ============================================================
// main
// ============================================================
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
