// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file nohw_full_test.cpp
/// @brief Full no-hardware test suite covering scenarios without physical devices (54 cases).
/// Coverage: TC_CPP_01, selected items in 02/10/11/12/13/14/18, and TC_CPP_20/22/23/24/25.
/// These tests validate API robustness, data structures, and error-handling in offline conditions.

#include "../test_common.hpp"
#include <libobsensor/ObSensor.hpp>
#include <libobsensor/hpp/Utils.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <future>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

// ============================================================
// Test group: Context.
// ============================================================
class TC_CPP_01_Context : public ContextTest {};

TEST_F(TC_CPP_01_Context, TC_CPP_01_01_context_default_create_destroy) {
    /// Test case: context default create destroy.
    ASSERT_NE(ctx_, nullptr);
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);
    // Validate expected conditions for this step.
    EXPECT_GE(devList->getCount(), 0u);
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_02_context_config_path) {
    /// Test case: context config path.
    // Prepare local state for the next check.
    {
        ob::Context ctxDefault("");
        auto        devList = ctxDefault.queryDeviceList();
        ASSERT_NE(devList, nullptr);
    }
    // Prepare local state for the next check.
    {
        try {
            ob::Context ctxBad("/non_existent_path/config.json");
            auto        devList = ctxBad.queryDeviceList();
            // Either succeeds with default config fallback, or throws — both are acceptable
        }
        catch(const ob::Error &) {
            // Expected for truly invalid config
        }
    }
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_03_context_repeated_create_destroy) {
    /// Test case: context repeated create destroy.
    for(int i = 0; i < 10; i++) {
        auto c = std::make_shared<ob::Context>();
        ASSERT_NE(c, nullptr) << "Iteration " << i;
        auto devList = c->queryDeviceList();
        ASSERT_NE(devList, nullptr) << "Iteration " << i;
        c.reset();
    }
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_04_free_idle_memory) {
    /// Test case: free idle memory.
    ctx_->freeIdleMemory();

    // Query the current device list for verification.
    auto devList = ctx_->queryDeviceList();
    ASSERT_NE(devList, nullptr);

    // Prepare local state for the next check.
    ctx_->freeIdleMemory();
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_05_uvc_backend) {
    /// Test case: uvc backend.
#if !defined(BUILD_USB_PAL)
    GTEST_SKIP() << "SDK built without USB PAL (BUILD_USB_PAL=OFF)";
#else
    bool        usbPalMissing = false;
    bool        hasUnexpected = false;
    std::string errMsg;

    auto setBackendType = [&](OBUvcBackendType backendType) {
        if(usbPalMissing || hasUnexpected) {
            return;
        }

        try {
            ctx_->setUvcBackendType(backendType);
        }
        catch(const ob::Error &e) {
            errMsg = e.what() ? e.what() : "";
            if(errMsg.find("Usb pal is not exist") != std::string::npos || errMsg.find("BUILD_USB_PAL") != std::string::npos) {
                usbPalMissing = true;
                return;
            }
            hasUnexpected = true;
        }
        catch(const std::exception &e) {
            errMsg        = e.what() ? e.what() : "std::exception";
            hasUnexpected = true;
        }
        catch(...) {
            errMsg        = "unknown exception";
            hasUnexpected = true;
        }
    };

    setBackendType(OB_UVC_BACKEND_TYPE_AUTO);
    if(usbPalMissing) {
        GTEST_SKIP() << "USB PAL backend unavailable at runtime: " << errMsg;
    }
    ASSERT_FALSE(hasUnexpected) << "setUvcBackendType(AUTO) failed: " << errMsg;

#ifdef __linux__
    setBackendType(OB_UVC_BACKEND_TYPE_LIBUVC);
    if(usbPalMissing) {
        GTEST_SKIP() << "USB PAL backend unavailable at runtime: " << errMsg;
    }
    ASSERT_FALSE(hasUnexpected) << "setUvcBackendType(LIBUVC) failed: " << errMsg;

    setBackendType(OB_UVC_BACKEND_TYPE_V4L2);
    if(usbPalMissing) {
        GTEST_SKIP() << "USB PAL backend unavailable at runtime: " << errMsg;
    }
    ASSERT_FALSE(hasUnexpected) << "setUvcBackendType(V4L2) failed: " << errMsg;
#endif

    // Reset to AUTO
    setBackendType(OB_UVC_BACKEND_TYPE_AUTO);
    if(usbPalMissing) {
        GTEST_SKIP() << "USB PAL backend unavailable at runtime: " << errMsg;
    }
    ASSERT_FALSE(hasUnexpected) << "setUvcBackendType(AUTO reset) failed: " << errMsg;
#endif
}

TEST_F(TC_CPP_01_Context, TC_CPP_01_06_extension_plugin_directory) {
    /// Test case: extension plugin directory.
    ob_error *error = nullptr;

    ob_set_extensions_directory(".", &error);
    EXPECT_EQ(error, nullptr);

    ob_set_extensions_directory("", &error);
    // Prepare local state for the next check.
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
// Test group: Discovery.
// ============================================================
class TC_CPP_02_Discovery : public ContextTest {};

TEST_F(TC_CPP_02_Discovery, TC_CPP_02_06_clock_sync) {
    /// Test case: clock sync.
    ASSERT_NO_THROW(ctx_->enableDeviceClockSync(0));
}

// ============================================================
// Test group: Frame.
// ============================================================
class TC_CPP_10_Frame : public SDKTestBase {};

TEST_F(TC_CPP_10_Frame, TC_CPP_10_04_frame_ref_count) {
    /// Test case: frame ref count.
    ob_error *error = nullptr;
    auto      frame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // Prepare local state for the next check.
    ob_frame_add_ref(frame, &error);
    ASSERT_EQ(error, nullptr);

    // Prepare local state for the next check.
    ob_delete_frame(frame, &error);
    ASSERT_EQ(error, nullptr);

    // Prepare local state for the next check.
    ob_delete_frame(frame, &error);
    ASSERT_EQ(error, nullptr);
}

TEST_F(TC_CPP_10_Frame, TC_CPP_10_13_frameset_push_frame) {
    /// Test case: frameset push frame.
    ob_error *error = nullptr;

    auto frameset = ob_create_frameset(&error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frameset, nullptr);

    // Prepare local state for the next check.
    auto depthFrame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);

    ob_frameset_push_frame(frameset, depthFrame, &error);
    ASSERT_EQ(error, nullptr);

    // Prepare local state for the next check.
    auto count = ob_frameset_get_count(frameset, &error);
    ASSERT_EQ(error, nullptr);
    EXPECT_GE(count, 1u);

    ob_delete_frame(depthFrame, &error);
    ob_delete_frame(frameset, &error);
}

// ============================================================
// Test group: Metadata.
// ============================================================
class TC_CPP_11_Metadata : public SDKTestBase {};

TEST_F(TC_CPP_11_Metadata, TC_CPP_11_06_update_metadata_c_api) {
    /// Test case: update metadata c api.
    ob_error *error = nullptr;
    auto      frame = ob_create_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640 * 480 * 2, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // Prepare local state for the next check.
    uint8_t metadata[64] = {};
    metadata[0]          = 0xAB;
    metadata[1]          = 0xCD;
    ob_frame_update_metadata(frame, metadata, sizeof(metadata), &error);
    EXPECT_EQ(error, nullptr);

    // Prepare local state for the next check.
    auto mdPtr = ob_frame_get_metadata(frame, &error);
    EXPECT_EQ(error, nullptr);
    if(mdPtr) {
        EXPECT_EQ(mdPtr[0], 0xAB);
        EXPECT_EQ(mdPtr[1], 0xCD);
    }

    ob_delete_frame(frame, &error);
}

// ============================================================
// Test group: FrameFactory.
// ============================================================
class TC_CPP_12_FrameFactory : public SDKTestBase {};

TEST_F(TC_CPP_12_FrameFactory, TC_CPP_12_01_create_frame_and_video_frame) {
    /// Test case: create frame and video frame.
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
    /// Test case: create frame from buffer.
    static std::atomic<bool> destroyCalled{ false };
    destroyCalled = false;

    const uint32_t bufSize = 100;
    auto          *buffer  = new uint8_t[bufSize];
    std::memset(buffer, 0x42, bufSize);

    ob_error *error = nullptr;
    auto      frame = ob_create_frame_from_buffer(
        OB_FRAME_DEPTH, OB_FORMAT_Y16, buffer, bufSize,
        [](uint8_t *buf, void *) {
            delete[] buf;
            destroyCalled = true;
        },
        nullptr, &error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frame, nullptr);

    // Prepare local state for the next check.
    auto *data = ob_frame_get_data(frame, &error);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data[0], 0x42);

    // Prepare local state for the next check.
    ob_delete_frame(frame, &error);
    EXPECT_TRUE(destroyCalled.load());
}

TEST_F(TC_CPP_12_FrameFactory, TC_CPP_12_04_create_empty_frameset) {
    /// Test case: create empty frameset.
    ob_error *error = nullptr;

    auto frameset = ob_create_frameset(&error);
    ASSERT_EQ(error, nullptr);
    ASSERT_NE(frameset, nullptr);

    // Prepare local state for the next check.
    auto count = ob_frameset_get_count(frameset, &error);
    EXPECT_EQ(count, 0u);

    // Prepare local state for the next check.
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
// Test group: Filter.
// ============================================================
class TC_CPP_13_Filter : public SDKTestBase {};

TEST_F(TC_CPP_13_Filter, TC_CPP_13_01_create_all_builtin_filters) {
    /// Test case: create all builtin filters.
    const std::vector<std::string> requiredFilterNames = {
        "DecimationFilter", "ThresholdFilter", "Align", "FormatConverter", "HDRMerge", "PointCloudFilter", "SequenceIdFilter",
    };

    const std::vector<std::string> optionalPrivateFilterNames = {
        "SpatialAdvancedFilter", "SpatialFastFilter",  "SpatialModerateFilter", "TemporalFilter",
        "HoleFillingFilter",     "NoiseRemovalFilter", "DisparityTransform",    "FalsePositiveFilter",
    };

    auto createFilterAndCheck = [](const std::string &name, bool required) {
        ob_error *error  = nullptr;
        auto      filter = ob_create_filter(name.c_str(), &error);
        if(error) {
            std::string errMsg = ob_error_get_message(error);
            ob_delete_error(error);

            if(required) {
                ADD_FAILURE() << "Failed to create required filter: " << name << " error: " << errMsg;
                return;
            }

            if(errMsg.find("Private filter library not activated") != std::string::npos || errMsg.find("Invalid filter name") != std::string::npos) {
                GTEST_LOG_(INFO) << "Skip optional filter in current environment: " << name << " error: " << errMsg;
                return;
            }

            ADD_FAILURE() << "Failed to create optional filter: " << name << " error: " << errMsg;
            return;
        }
        ASSERT_NE(filter, nullptr) << "Filter is null: " << name;
        ob_delete_filter(filter, &error);
    };

    for(const auto &name: requiredFilterNames) {
        createFilterAndCheck(name, true);
    }

    for(const auto &name: optionalPrivateFilterNames) {
        createFilterAndCheck(name, false);
    }
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_02_create_invalid_filter) {
    /// Test case: create invalid filter.
    ob_error *error  = nullptr;
    auto      filter = ob_create_filter("TotallyBogusFilter", &error);
    EXPECT_NE(error, nullptr) << "Expected error for invalid filter name";
    EXPECT_EQ(filter, nullptr);
    if(error) {
        ob_delete_error(error);
    }
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_05_filter_enable_disable) {
    /// Test case: filter enable disable.
    ob_error *error  = nullptr;
    auto      filter = ob_create_filter("DecimationFilter", &error);
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
    /// Test case: filter reset and config.
    ob_error *error  = nullptr;
    auto      filter = ob_create_filter("DecimationFilter", &error);
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
        // Prepare local state for the next check.
        ob_filter_get_config_value(filter, item.name, &error);
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
    /// Test case: filter type check.
    auto filter = std::make_shared<ob::Filter>(ob_create_filter("PointCloudFilter", nullptr));
    ASSERT_NE(filter, nullptr);

    // Validate expected conditions for this step.
    EXPECT_EQ(filter->getName(), "PointCloudFilter");

    // is<PointCloudFilter> should be true
    EXPECT_TRUE(filter->is<ob::PointCloudFilter>());
    auto pcf = filter->as<ob::PointCloudFilter>();
    ASSERT_NE(pcf, nullptr);
}

TEST_F(TC_CPP_13_Filter, TC_CPP_13_17_private_filter) {
    /// Test case: private filter.
    ob_error *error = nullptr;

    // Prepare local state for the next check.
    auto pf = ob_create_private_filter("SomePrivateFilter", "invalid_key", &error);
    // Prepare local state for the next check.
    if(error) {
        ob_delete_error(error);
        error = nullptr;
    }
    if(pf) {
        ob_delete_filter(pf, &error);
    }
}

// ============================================================
// Test group: Playback.
// ============================================================
namespace {

static bool hasSensorType(const std::shared_ptr<ob::SensorList> &sensorList, OBSensorType targetType) {
    if(!sensorList) {
        return false;
    }
    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        if(sensorList->getSensorType(i) == targetType) {
            return true;
        }
    }
    return false;
}

static std::shared_ptr<ob::VideoStreamProfile> getFirstVideoProfile(const std::shared_ptr<ob::SensorList> &sensorList, OBSensorType targetType) {
    if(!sensorList) {
        return nullptr;
    }
    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        if(sensorList->getSensorType(i) != targetType) {
            continue;
        }
        auto sensor = sensorList->getSensor(i);
        if(!sensor) {
            continue;
        }
        auto profiles = sensor->getStreamProfileList();
        if(!profiles || profiles->getCount() == 0) {
            continue;
        }
        for(uint32_t j = 0; j < profiles->getCount(); ++j) {
            auto profile = profiles->getProfile(j);
            if(profile && profile->is<ob::VideoStreamProfile>()) {
                return profile->as<ob::VideoStreamProfile>();
            }
        }
    }
    return nullptr;
}

template <typename T> static void expectRangeValid(const T &range) {
    EXPECT_LE(range.min, range.max);
}

}  // namespace

class TC_CPP_18_Playback : public SDKTestBase {
protected:
    void SetUp() override {
        SDKTestBase::SetUp();
        if(env_.playbackBagPath().empty()) {
            GTEST_SKIP() << "PLAYBACK_BAG_PATH not set and no local .bag found — skipping playback test";
        }
    }
};

TEST_F(TC_CPP_18_Playback, TC_CPP_18_03_playback_device_create_and_play) {
    /// Test case: playback device create and play.
    auto bagPath  = env_.playbackBagPath();
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(bagPath);
    ASSERT_NE(pbDevice, nullptr);

    auto devInfo = pbDevice->getDeviceInfo();
    ASSERT_NE(devInfo, nullptr);
    EXPECT_NE(devInfo->getName(), nullptr);

    // Prepare local state for the next check.
    auto pipeline   = std::make_shared<ob::Pipeline>(pbDevice);
    auto config     = std::make_shared<ob::Config>();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    ASSERT_GT(sensorList->getCount(), 0u) << "No sensors found in playback file";
    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        config->enableStream(sensorList->getSensorType(i));
    }
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline->start(config);

    auto frames = pipeline->waitForFrameset(5000);
    EXPECT_NE(frames, nullptr) << "No frames from playback";

    pipeline->stop();
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_04_playback_duration_position) {
    /// Test case: playback duration position.
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    auto duration = pbDevice->getDuration();
    EXPECT_GT(duration, 0u) << "Duration should be > 0";

    auto position = pbDevice->getPosition();
    EXPECT_GE(position, 0u);
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_05_playback_seek_and_rate) {
    /// Test case: playback seek and rate.
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
    /// Test case: playback pause resume status.
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    auto pipeline   = std::make_shared<ob::Pipeline>(pbDevice);
    auto config     = std::make_shared<ob::Config>();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    ASSERT_GT(sensorList->getCount(), 0u) << "No sensors found in playback file";
    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        config->enableStream(sensorList->getSensorType(i));
    }
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline->start(config);

    // Wait for frames to validate runtime behavior.
    pipeline->waitForFrameset(2000);

    // Pause
    ASSERT_NO_THROW(pbDevice->pause());
    auto status = pbDevice->getPlaybackStatus();
    EXPECT_EQ(status, OB_PLAYBACK_PAUSED);

    // Resume
    ASSERT_NO_THROW(pbDevice->resume());
    status = pbDevice->getPlaybackStatus();
    EXPECT_EQ(status, OB_PLAYBACK_PLAYING);

    pipeline->stop();
}

TEST_F(TC_CPP_18_Playback, TC_CPP_18_07_playback_status_callback) {
    /// Test case: playback status callback.
    auto pbDevice = std::make_shared<ob::PlaybackDevice>(env_.playbackBagPath());
    ASSERT_NE(pbDevice, nullptr);

    std::atomic<int> cbCount{ 0 };
    pbDevice->setPlaybackStatusChangeCallback([&cbCount](OBPlaybackStatus) { cbCount++; });

    auto pipeline   = std::make_shared<ob::Pipeline>(pbDevice);
    auto config     = std::make_shared<ob::Config>();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    ASSERT_GT(sensorList->getCount(), 0u) << "No sensors found in playback file";
    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        config->enableStream(sensorList->getSensorType(i));
    }
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);
    pipeline->start(config);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    pbDevice->pause();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pbDevice->resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    pipeline->stop();

    // Validate expected conditions for this step.
    EXPECT_GE(cbCount.load(), 1) << "No status change callback received";
}

class TC_CPP_18_PlaybackParam : public SDKTestBase, public ::testing::WithParamInterface<std::string> {
protected:
    void SetUp() override {
        SDKTestBase::SetUp();
        if(GetParam().empty()) {
            GTEST_SKIP() << "No playback bag path provided for parameterized test";
        }
    }

    std::shared_ptr<ob::PlaybackDevice> createPlaybackDevice() const {
        return std::make_shared<ob::PlaybackDevice>(GetParam());
    }

    std::shared_ptr<ob::Pipeline> createStartedPipeline(const std::shared_ptr<ob::PlaybackDevice> &pbDevice) const {
        auto pipeline   = std::make_shared<ob::Pipeline>(pbDevice);
        auto config     = std::make_shared<ob::Config>();
        auto sensorList = pbDevice->getSensorList();
        EXPECT_NE(sensorList, nullptr);
        if(!sensorList || sensorList->getCount() == 0) {
            return nullptr;
        }
        for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
            config->enableStream(sensorList->getSensorType(i));
        }
        config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);
        pipeline->start(config);
        return pipeline;
    }

    std::shared_ptr<ob::DepthFrame> getDepthFrameFromPlayback(const std::shared_ptr<ob::PlaybackDevice> &pbDevice) const {
        auto pipeline = createStartedPipeline(pbDevice);
        if(!pipeline) {
            return nullptr;
        }
        auto frames = pipeline->waitForFrameset(5000);
        pipeline->stop();
        if(!frames) {
            return nullptr;
        }
        return frames->getDepthFrame();
    }

    template <typename TFilter, typename Fn> void runPrivateFilterCase(const Fn &fn) {
        try {
            auto filter = std::make_shared<TFilter>("");
            ASSERT_NE(filter, nullptr);
            fn(filter);
        }
        catch(const ob::Error &e) {
            std::string msg = e.what() ? e.what() : "";
            if(msg.find("Private filter") != std::string::npos || msg.find("activated") != std::string::npos) {
                GTEST_SKIP() << "Private filter not activated: " << msg;
            }
            throw;
        }
    }
};

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_10_playback_device_info) {
    auto pbDevice = createPlaybackDevice();
    ASSERT_NE(pbDevice, nullptr);

    auto info = pbDevice->getDeviceInfo();
    ASSERT_NE(info, nullptr);
    ASSERT_NE(info->getName(), nullptr);
    EXPECT_GT(std::strlen(info->getName()), 0u);
    EXPECT_NE(info->getSerialNumber(), nullptr);
    EXPECT_NE(info->getFirmwareVersion(), nullptr);
    EXPECT_NE(info->getConnectionType(), nullptr);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_11_playback_sensor_list) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    ASSERT_GT(sensorList->getCount(), 0u);

    EXPECT_TRUE(hasSensorType(sensorList, OB_SENSOR_DEPTH));
    EXPECT_TRUE(hasSensorType(sensorList, OB_SENSOR_COLOR));
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_12_playback_stream_profile_list) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    ASSERT_GT(sensorList->getCount(), 0u);

    for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
        auto sensor = sensorList->getSensor(i);
        ASSERT_NE(sensor, nullptr);

        auto profileList = sensor->getStreamProfileList();
        ASSERT_NE(profileList, nullptr);
        ASSERT_GT(profileList->getCount(), 0u);

        for(uint32_t j = 0; j < profileList->getCount(); ++j) {
            auto profile = profileList->getProfile(j);
            ASSERT_NE(profile, nullptr);
            if(profile->is<ob::VideoStreamProfile>()) {
                auto video = profile->as<ob::VideoStreamProfile>();
                EXPECT_GT(video->getWidth(), 0u);
                EXPECT_GT(video->getHeight(), 0u);
                EXPECT_GT(video->getFps(), 0u);
            }
        }
    }
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_20_playback_depth_frame) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);

    auto depth = frames->getDepthFrame();
    ASSERT_NE(depth, nullptr);
    EXPECT_EQ(depth->getType(), OB_FRAME_DEPTH);
    EXPECT_GT(depth->getWidth(), 0u);
    EXPECT_GT(depth->getHeight(), 0u);
    EXPECT_NE(depth->getData(), nullptr);
    EXPECT_GT(depth->getDataSize(), 0u);
    EXPECT_GT(depth->getTimeStampUs(), 0u);

    uint64_t pixelCount = static_cast<uint64_t>(depth->getWidth()) * depth->getHeight();
    ASSERT_GT(pixelCount, 0u);
    EXPECT_EQ(depth->getDataSize() % pixelCount, 0u);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_21_playback_color_frame) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);

    auto color = frames->getColorFrame();
    if(!color) {
        GTEST_SKIP() << "No color frame in playback bag";
    }

    EXPECT_EQ(color->getType(), OB_FRAME_COLOR);
    EXPECT_GT(color->getWidth(), 0u);
    EXPECT_GT(color->getHeight(), 0u);
    EXPECT_NE(color->getData(), nullptr);
    EXPECT_GT(color->getDataSize(), 0u);

    const std::set<OBFormat> knownColorFormats = {
        OB_FORMAT_RGB,  OB_FORMAT_BGR,  OB_FORMAT_RGBA, OB_FORMAT_BGRA, OB_FORMAT_YUYV,
        OB_FORMAT_UYVY, OB_FORMAT_MJPG, OB_FORMAT_NV12, OB_FORMAT_NV21, OB_FORMAT_I420,
    };
    EXPECT_TRUE(knownColorFormats.count(color->getFormat()) > 0 || color->getFormat() != OB_FORMAT_UNKNOWN);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_22_playback_ir_frame) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);

    auto ir      = frames->getFrame(OB_FRAME_IR);
    auto irLeft  = frames->getFrame(OB_FRAME_IR_LEFT);
    auto irRight = frames->getFrame(OB_FRAME_IR_RIGHT);
    if(!ir && !irLeft && !irRight) {
        GTEST_SKIP() << "No IR/IR_LEFT/IR_RIGHT frame in playback bag";
    }

    auto validateIr = [](const std::shared_ptr<ob::Frame> &f) {
        if(!f) {
            return;
        }
        EXPECT_NE(f->getData(), nullptr);
        EXPECT_GT(f->getDataSize(), 0u);
    };

    validateIr(ir);
    validateIr(irLeft);
    validateIr(irRight);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_23_playback_frame_metadata) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);

    std::shared_ptr<ob::Frame> frame = frames->getDepthFrame();
    if(!frame) {
        frame = frames->getColorFrame();
    }
    if(!frame) {
        GTEST_SKIP() << "No depth/color frame for metadata test";
    }

    EXPECT_TRUE(frame->hasMetadata(OB_FRAME_METADATA_TYPE_TIMESTAMP));
    if(frame->hasMetadata(OB_FRAME_METADATA_TYPE_TIMESTAMP)) {
        EXPECT_GT(frame->getMetadataValue(OB_FRAME_METADATA_TYPE_TIMESTAMP), 0);
    }

    int supportedCount = 0;
    for(int i = 0; i < static_cast<int>(OB_FRAME_METADATA_TYPE_COUNT); ++i) {
        auto type = static_cast<OBFrameMetadataType>(i);
        if(frame->hasMetadata(type)) {
            (void)frame->getMetadataValue(type);
            ++supportedCount;
        }
    }
    EXPECT_GT(supportedCount, 0);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_24_playback_frame_timestamps) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    std::shared_ptr<ob::DepthFrame> d1;
    std::shared_ptr<ob::DepthFrame> d2;
    for(int i = 0; i < 10 && (!d1 || !d2); ++i) {
        auto frames = pipeline->waitForFrameset(5000);
        if(!frames) {
            continue;
        }

        auto depth = frames->getDepthFrame();
        if(!depth) {
            continue;
        }

        if(!d1) {
            d1 = depth;
        }
        else {
            d2 = depth;
        }
    }
    pipeline->stop();

    if(!d1 || !d2) {
        GTEST_SKIP() << "No two depth frames for timestamp monotonic check after retries";
    }

    EXPECT_GT(d1->getTimeStampUs(), 0u);
    EXPECT_GT(d2->getTimeStampUs(), 0u);
    EXPECT_GE(d2->getTimeStampUs(), d1->getTimeStampUs());
    EXPECT_GE(d1->getSystemTimeStampUs(), 0u);
    EXPECT_GE(d1->getGlobalTimeStampUs(), 0u);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_25_playback_frameset_multi_stream) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);
    ASSERT_GE(frames->getCount(), 2u);

    std::set<int> typeSet;
    for(uint32_t i = 0; i < frames->getCount(); ++i) {
        auto frame = frames->getFrameByIndex(i);
        ASSERT_NE(frame, nullptr);
        int t = static_cast<int>(frame->getType());
        EXPECT_TRUE(typeSet.insert(t).second) << "Duplicate frame type in frameset: " << t;
    }
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_30_playback_depth_intrinsic) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    auto profile    = getFirstVideoProfile(sensorList, OB_SENSOR_DEPTH);
    if(!profile) {
        GTEST_SKIP() << "No depth video profile";
    }

    auto in = profile->getIntrinsic();
    EXPECT_GT(in.fx, 0.0f);
    EXPECT_GT(in.fy, 0.0f);
    EXPECT_GT(in.width, 0);
    EXPECT_GT(in.height, 0);
    EXPECT_GT(in.cx, 0.0f);
    EXPECT_GT(in.cy, 0.0f);
    EXPECT_LT(in.cx, static_cast<float>(in.width));
    EXPECT_LT(in.cy, static_cast<float>(in.height));
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_31_playback_color_intrinsic) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    auto profile    = getFirstVideoProfile(sensorList, OB_SENSOR_COLOR);
    if(!profile) {
        GTEST_SKIP() << "No color video profile";
    }

    auto in = profile->getIntrinsic();
    EXPECT_GT(in.fx, 0.0f);
    EXPECT_GT(in.fy, 0.0f);
    EXPECT_GT(in.width, 0);
    EXPECT_GT(in.height, 0);
    EXPECT_GT(in.cx, 0.0f);
    EXPECT_GT(in.cy, 0.0f);
    EXPECT_LT(in.cx, static_cast<float>(in.width));
    EXPECT_LT(in.cy, static_cast<float>(in.height));
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_32_playback_depth_distortion) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    auto profile    = getFirstVideoProfile(sensorList, OB_SENSOR_DEPTH);
    if(!profile) {
        GTEST_SKIP() << "No depth video profile";
    }

    auto d = profile->getDistortion();
    EXPECT_GE(static_cast<int>(d.model), static_cast<int>(OB_DISTORTION_NONE));
    EXPECT_LE(static_cast<int>(d.model), static_cast<int>(OB_DISTORTION_KANNALA_BRANDT4));
    EXPECT_TRUE(std::isfinite(d.k1));
    EXPECT_TRUE(std::isfinite(d.k2));
    EXPECT_TRUE(std::isfinite(d.k3));
    EXPECT_TRUE(std::isfinite(d.k4));
    EXPECT_TRUE(std::isfinite(d.k5));
    EXPECT_TRUE(std::isfinite(d.k6));
    EXPECT_TRUE(std::isfinite(d.p1));
    EXPECT_TRUE(std::isfinite(d.p2));
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_33_playback_depth_to_color_extrinsic) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    auto depthPrf   = getFirstVideoProfile(sensorList, OB_SENSOR_DEPTH);
    auto colorPrf   = getFirstVideoProfile(sensorList, OB_SENSOR_COLOR);
    if(!depthPrf || !colorPrf) {
        GTEST_SKIP() << "Depth or color profile missing for extrinsic test";
    }

    auto ex = depthPrf->getExtrinsicTo(colorPrf);
    for(int i = 0; i < 3; ++i) {
        EXPECT_TRUE(std::isfinite(ex.trans[i]));
    }

    auto dot = [&](int r1, int r2) {
        return ex.rot[r1 * 3 + 0] * ex.rot[r2 * 3 + 0] + ex.rot[r1 * 3 + 1] * ex.rot[r2 * 3 + 1] + ex.rot[r1 * 3 + 2] * ex.rot[r2 * 3 + 2];
    };
    EXPECT_NEAR(dot(0, 0), 1.0f, 0.1f);
    EXPECT_NEAR(dot(1, 1), 1.0f, 0.1f);
    EXPECT_NEAR(dot(2, 2), 1.0f, 0.1f);
    EXPECT_NEAR(dot(0, 1), 0.0f, 0.1f);
    EXPECT_NEAR(dot(0, 2), 0.0f, 0.1f);
    EXPECT_NEAR(dot(1, 2), 0.0f, 0.1f);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_34_playback_calibration_camera_param_list) {
    auto pbDevice = createPlaybackDevice();
    auto list     = pbDevice->getCalibrationCameraParamList();
    ASSERT_NE(list, nullptr);
    ASSERT_GT(list->getCount(), 0u);

    auto param = list->getCameraParam(0);
    EXPECT_GT(param.depthIntrinsic.fx, 0.0f);
    EXPECT_GT(param.depthIntrinsic.fy, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fx, 0.0f);
    EXPECT_GT(param.rgbIntrinsic.fy, 0.0f);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_40_playback_decimation_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame";
    }

    ob::DecimationFilter filter;
    EXPECT_EQ(filter.getName(), "DecimationFilter");
    auto range = filter.getScaleRange();
    expectRangeValid(range);

    uint32_t inW = depthFrame->getWidth();
    uint32_t inH = depthFrame->getHeight();

    filter.setScaleValue(2);
    EXPECT_EQ(filter.getScaleValue(), 2);
    EXPECT_NEAR(filter.getConfigValue("decimate"), 2.0, 1e-6);

    auto out = filter.process(depthFrame);
    ASSERT_NE(out, nullptr);
    ASSERT_TRUE(out->is<ob::VideoFrame>());
    auto outV = out->as<ob::VideoFrame>();
    EXPECT_EQ(outV->getWidth(), inW / 2);
    EXPECT_EQ(outV->getHeight(), inH / 2);

    filter.enable(false);
    auto passthrough = filter.process(depthFrame);
    ASSERT_NE(passthrough, nullptr);
    auto passV = passthrough->as<ob::VideoFrame>();
    EXPECT_GT(passV->getWidth(), 0u);
    EXPECT_GT(passV->getHeight(), 0u);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_41_playback_threshold_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame";
    }

    ob::ThresholdFilter filter;
    EXPECT_EQ(filter.getName(), "ThresholdFilter");
    auto minR = filter.getMinRange();
    auto maxR = filter.getMaxRange();
    expectRangeValid(minR);
    expectRangeValid(maxR);
    EXPECT_TRUE(filter.setValueRange(100, 5000));
    EXPECT_FALSE(filter.setValueRange(5000, 100));
    EXPECT_FALSE(filter.setValueRange(3000, 3000));
    EXPECT_NEAR(filter.getConfigValue("min"), 100.0, 1e-6);
    EXPECT_NEAR(filter.getConfigValue("max"), 5000.0, 1e-6);

    auto out = filter.process(depthFrame);
    ASSERT_NE(out, nullptr);
    EXPECT_NE(out->getData(), nullptr);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_42_playback_format_converter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame";
    }

    ob::FormatConvertFilter filter;
    EXPECT_EQ(filter.getName(), "FormatConverter");

    if(depthFrame->getFormat() == OB_FORMAT_Y16) {
        filter.setFormatConvertType(FORMAT_Y16_TO_RGB);
    }
    else if(depthFrame->getFormat() == OB_FORMAT_YUYV) {
        filter.setFormatConvertType(FORMAT_YUYV_TO_RGB);
    }
    else {
        GTEST_SKIP() << "No known conversion mapping for depth format " << static_cast<int>(depthFrame->getFormat());
    }

    auto out = filter.process(depthFrame);
    ASSERT_NE(out, nullptr);
    EXPECT_NE(out->getFormat(), depthFrame->getFormat());
    EXPECT_NE(out->getData(), nullptr);
    EXPECT_GT(out->getDataSize(), 0u);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_43_playback_point_cloud_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame";
    }

    ob::PointCloudFilter filter;
    EXPECT_EQ(filter.getName(), "PointCloudFilter");

    auto decRange = filter.getDecimationFactorRange();
    expectRangeValid(decRange);
    filter.setCreatePointFormat(OB_FORMAT_POINT);
    filter.setCoordinateDataScaled(0.001f);
    filter.setCoordinateSystem(OB_LEFT_HAND_COORDINATE_SYSTEM);
    filter.setDecimationFactor(2);

    auto out = filter.process(depthFrame);
    ASSERT_NE(out, nullptr);
    ASSERT_TRUE(out->is<ob::PointsFrame>());
    auto points = out->as<ob::PointsFrame>();
    EXPECT_NE(points->getData(), nullptr);
    EXPECT_GT(points->getDataSize(), 0u);
    EXPECT_GT(points->getCoordinateValueScale(), 0.0f);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_44_playback_align_filter) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    ASSERT_NE(pipeline, nullptr);

    auto frames = pipeline->waitForFrameset(5000);
    pipeline->stop();
    ASSERT_NE(frames, nullptr);

    ob::Align alignColor(OB_STREAM_COLOR);
    EXPECT_EQ(alignColor.getName(), "Align");
    EXPECT_EQ(alignColor.getAlignToStreamType(), OB_STREAM_COLOR);
    alignColor.setMatchTargetResolution(true);
    alignColor.setMatchTargetResolution(false);
    auto aligned = alignColor.process(frames);
    ASSERT_NE(aligned, nullptr);

    auto sensorList = pbDevice->getSensorList();
    auto colorPrf   = getFirstVideoProfile(sensorList, OB_SENSOR_COLOR);
    if(colorPrf) {
        EXPECT_NO_THROW(alignColor.setAlignToStreamProfile(colorPrf));
    }

    ob::Align alignDepth(OB_STREAM_DEPTH);
    EXPECT_EQ(alignDepth.getAlignToStreamType(), OB_STREAM_DEPTH);
    EXPECT_NO_THROW(alignDepth.process(frames));
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_45_playback_recommended_filters) {
    auto pbDevice   = createPlaybackDevice();
    auto sensorList = pbDevice->getSensorList();
    ASSERT_NE(sensorList, nullptr);
    if(!hasSensorType(sensorList, OB_SENSOR_DEPTH)) {
        GTEST_SKIP() << "No depth sensor for recommended filter test";
    }

    auto depthSensor = sensorList->getSensor(OB_SENSOR_DEPTH);
    ASSERT_NE(depthSensor, nullptr);
    auto filters = depthSensor->createRecommendedFilters();
    ASSERT_GT(filters.size(), 0u);

    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame for recommended filter process";
    }

    bool sawDecimation = false;
    for(const auto &f: filters) {
        ASSERT_NE(f, nullptr);
        EXPECT_FALSE(f->getName().empty());

        auto schema = f->getConfigSchemaVec();
        for(const auto &item: schema) {
            double v = f->getConfigValue(item.name);
            EXPECT_GE(v, item.min);
            EXPECT_LE(v, item.max);
        }

        f->enable(true);
        EXPECT_NO_THROW(f->process(depthFrame));
        EXPECT_NO_THROW(f->reset());

        if(f->getName() == "DecimationFilter") {
            sawDecimation = true;
            EXPECT_TRUE(f->is<ob::DecimationFilter>());
        }
    }
    EXPECT_TRUE(sawDecimation || !filters.empty());
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_46_playback_filter_chain) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame";
    }

    ob::ThresholdFilter threshold;
    ASSERT_TRUE(threshold.setValueRange(100, 10000));
    ob::DecimationFilter decimation;
    decimation.setScaleValue(2);

    auto stage1 = threshold.process(depthFrame);
    ASSERT_NE(stage1, nullptr);
    auto stage2 = decimation.process(stage1);
    ASSERT_NE(stage2, nullptr);

    ASSERT_TRUE(stage1->is<ob::VideoFrame>());
    ASSERT_TRUE(stage2->is<ob::VideoFrame>());
    auto s1 = stage1->as<ob::VideoFrame>();
    auto s2 = stage2->as<ob::VideoFrame>();
    EXPECT_EQ(s2->getWidth(), s1->getWidth() / 2);
    EXPECT_EQ(s2->getHeight(), s1->getHeight() / 2);

    ob::FormatConvertFilter convert;
    if(stage2->getFormat() == OB_FORMAT_Y16) {
        convert.setFormatConvertType(FORMAT_Y16_TO_RGB);
        auto stage3 = convert.process(stage2);
        ASSERT_NE(stage3, nullptr);
        EXPECT_NE(stage3->getFormat(), OB_FORMAT_Y16);
    }

    std::promise<bool> gotAsync;
    auto               asyncResult = gotAsync.get_future();
    decimation.setCallBack([&gotAsync](std::shared_ptr<ob::Frame> frame) {
        if(frame) {
            gotAsync.set_value(true);
        }
    });
    decimation.pushFrame(depthFrame);
    EXPECT_EQ(asyncResult.wait_for(std::chrono::seconds(3)), std::future_status::ready);

    auto fromFactory = ob::FilterFactory::createFilter("DecimationFilter");
    ASSERT_NE(fromFactory, nullptr);
    EXPECT_EQ(fromFactory->getName(), "DecimationFilter");
    EXPECT_TRUE(fromFactory->is<ob::DecimationFilter>());
    EXPECT_NE(fromFactory->as<ob::DecimationFilter>(), nullptr);

    auto csv = fromFactory->getConfigSchema();
    EXPECT_FALSE(csv.empty());
    EXPECT_NE(csv.find("decimate"), std::string::npos);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_47_playback_hdr_merge) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    auto filter = std::make_shared<ob::HdrMerge>();
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->getName(), "HDRMerge");
    EXPECT_TRUE(filter->is<ob::HdrMerge>());
    EXPECT_NE(filter->as<ob::HdrMerge>(), nullptr);
    EXPECT_NO_THROW(filter->reset());

    std::shared_ptr<ob::Frame> out;
    EXPECT_NO_THROW(out = filter->process(depthFrame));
    if(out) {
        EXPECT_NE(out->getData(), nullptr);
    }
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_48_playback_sequence_id_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    auto filter = std::make_shared<ob::SequenceIdFilter>();
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->getName(), "SequenceIdFilter");
    EXPECT_TRUE(filter->is<ob::SequenceIdFilter>());
    EXPECT_NE(filter->as<ob::SequenceIdFilter>(), nullptr);
    EXPECT_GE(filter->getSequenceIdListSize(), 2);
    EXPECT_NE(filter->getSequenceIdList(), nullptr);

    filter->selectSequenceId(0);
    EXPECT_EQ(filter->getSelectSequenceId(), 0);
    filter->selectSequenceId(1);
    EXPECT_EQ(filter->getSelectSequenceId(), 1);
    EXPECT_NEAR(filter->getConfigValue("sequenceid"), 1.0, 1e-6);

    auto out = filter->process(depthFrame);
    if(out) {
        EXPECT_EQ(out->getType(), OB_FRAME_DEPTH);
    }
    EXPECT_NO_THROW(filter->reset());
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_49_playback_spatial_advanced_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::SpatialAdvancedFilter>([&](const std::shared_ptr<ob::SpatialAdvancedFilter> &filter) {
        EXPECT_EQ(filter->getName(), "SpatialAdvancedFilter");
        expectRangeValid(filter->getAlphaRange());
        expectRangeValid(filter->getDispDiffRange());
        expectRangeValid(filter->getRadiusRange());
        expectRangeValid(filter->getMagnitudeRange());

        auto params      = filter->getFilterParams();
        params.alpha     = filter->getAlphaRange().def;
        params.disp_diff = filter->getDispDiffRange().def;
        params.magnitude = static_cast<uint8_t>(filter->getMagnitudeRange().def);
        params.radius    = filter->getRadiusRange().def;
        filter->setFilterParams(params);
        auto got = filter->getFilterParams();
        EXPECT_NEAR(got.alpha, params.alpha, 1e-6f);
        EXPECT_EQ(got.disp_diff, params.disp_diff);
        EXPECT_EQ(got.magnitude, params.magnitude);
        EXPECT_EQ(got.radius, params.radius);

        auto out = filter->process(depthFrame);
        if(out) {
            EXPECT_NE(out->getData(), nullptr);
        }
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_50_playback_spatial_fast_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::SpatialFastFilter>([&](const std::shared_ptr<ob::SpatialFastFilter> &filter) {
        EXPECT_EQ(filter->getName(), "SpatialFastFilter");
        auto radiusRange = filter->getRadiusRange();
        expectRangeValid(radiusRange);

        auto params   = filter->getFilterParams();
        params.radius = static_cast<uint8_t>(radiusRange.def);
        filter->setFilterParams(params);
        auto got = filter->getFilterParams();
        EXPECT_EQ(got.radius, params.radius);
        EXPECT_NEAR(filter->getConfigValue("radius"), static_cast<double>(params.radius), 1e-6);

        auto out = filter->process(depthFrame);
        if(out) {
            EXPECT_NE(out->getData(), nullptr);
        }
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_51_playback_spatial_moderate_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::SpatialModerateFilter>([&](const std::shared_ptr<ob::SpatialModerateFilter> &filter) {
        EXPECT_EQ(filter->getName(), "SpatialModerateFilter");
        auto magRange  = filter->getMagnitudeRange();
        auto radRange  = filter->getRadiusRange();
        auto diffRange = filter->getDispDiffRange();
        expectRangeValid(magRange);
        expectRangeValid(radRange);
        expectRangeValid(diffRange);

        auto params      = filter->getFilterParams();
        params.magnitude = static_cast<uint8_t>(magRange.def);
        params.radius    = static_cast<uint8_t>(radRange.def);
        params.disp_diff = static_cast<uint16_t>(diffRange.def);
        filter->setFilterParams(params);
        auto got = filter->getFilterParams();
        EXPECT_EQ(got.magnitude, params.magnitude);
        EXPECT_EQ(got.radius, params.radius);
        EXPECT_EQ(got.disp_diff, params.disp_diff);

        EXPECT_NO_THROW(filter->process(depthFrame));
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_52_playback_hole_filling_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::HoleFillingFilter>([&](const std::shared_ptr<ob::HoleFillingFilter> &filter) {
        EXPECT_EQ(filter->getName(), "HoleFillingFilter");

        filter->setFilterMode(OB_HOLE_FILL_TOP);
        EXPECT_EQ(filter->getFilterMode(), OB_HOLE_FILL_TOP);
        filter->setFilterMode(OB_HOLE_FILL_NEAREST);
        EXPECT_EQ(filter->getFilterMode(), OB_HOLE_FILL_NEAREST);
        filter->setFilterMode(OB_HOLE_FILL_FAREST);
        EXPECT_EQ(filter->getFilterMode(), OB_HOLE_FILL_FAREST);

        EXPECT_NO_THROW(filter->process(depthFrame));
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_53_playback_noise_removal_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::NoiseRemovalFilter>([&](const std::shared_ptr<ob::NoiseRemovalFilter> &filter) {
        EXPECT_EQ(filter->getName(), "NoiseRemovalFilter");
        auto dispRange = filter->getDispDiffRange();
        auto sizeRange = filter->getMaxSizeRange();
        expectRangeValid(dispRange);
        expectRangeValid(sizeRange);

        auto params      = filter->getFilterParams();
        params.disp_diff = dispRange.def;
        params.max_size  = sizeRange.def;
        filter->setFilterParams(params);
        auto got = filter->getFilterParams();
        EXPECT_EQ(got.disp_diff, params.disp_diff);
        EXPECT_EQ(got.max_size, params.max_size);
        EXPECT_NEAR(filter->getConfigValue("min_diff"), static_cast<double>(params.disp_diff), 1e-6);
        EXPECT_NEAR(filter->getConfigValue("max_size"), static_cast<double>(params.max_size), 1e-6);

        EXPECT_NO_THROW(filter->process(depthFrame));
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_54_playback_temporal_filter) {
    auto pbDevice = createPlaybackDevice();
    auto pipeline = createStartedPipeline(pbDevice);
    if(!pipeline) {
        GTEST_SKIP() << "Failed to start pipeline for temporal filter";
    }

    std::vector<std::shared_ptr<ob::DepthFrame>> depthFrames;
    for(int i = 0; i < 3; ++i) {
        auto frames = pipeline->waitForFrameset(5000);
        if(!frames) {
            continue;
        }
        auto depth = frames->getDepthFrame();
        if(depth) {
            depthFrames.push_back(depth);
        }
    }
    pipeline->stop();

    if(depthFrames.empty()) {
        GTEST_SKIP() << "No depth frames for temporal filter";
    }

    runPrivateFilterCase<ob::TemporalFilter>([&](const std::shared_ptr<ob::TemporalFilter> &filter) {
        EXPECT_EQ(filter->getName(), "TemporalFilter");
        auto diffRange = filter->getDiffScaleRange();
        auto wRange    = filter->getWeightRange();
        expectRangeValid(diffRange);
        expectRangeValid(wRange);

        filter->setDiffScale(diffRange.def);
        filter->setWeight(wRange.def);
        EXPECT_NEAR(filter->getConfigValue("diff_scale"), static_cast<double>(diffRange.def), 1e-6);
        EXPECT_NEAR(filter->getConfigValue("weight"), static_cast<double>(wRange.def), 1e-6);

        for(const auto &depth: depthFrames) {
            EXPECT_NO_THROW(filter->process(depth));
        }
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_55_playback_false_positive_filter) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::FalsePositiveFilter>([&](const std::shared_ptr<ob::FalsePositiveFilter> &filter) {
        EXPECT_EQ(filter->getName(), "FalsePositiveFilter");
        expectRangeValid(filter->getfpEdgeBleedFilterEnableRange());
        expectRangeValid(filter->getfpebfROIMinXRatioRange());
        expectRangeValid(filter->getfpebfROIMaxXRatioRange());
        expectRangeValid(filter->getfpebfROIMinYRatioRange());
        expectRangeValid(filter->getfpebfROIMaxYRatioRange());
        expectRangeValid(filter->getfpTextureSparsityFilterEnableRange());
        expectRangeValid(filter->getfptsfROIMinXRatioRange());
        expectRangeValid(filter->getfptsfROIMaxXRatioRange());
        expectRangeValid(filter->getfptsfROIMinYRatioRange());
        expectRangeValid(filter->getfptsfROIMaxYRatioRange());
        expectRangeValid(filter->getfptsfMaxNoiseLevelRange());
        expectRangeValid(filter->getfptsfMaxSpeckleSizeRange());
        expectRangeValid(filter->getfpPatternAmbiguityFilterEnableRange());
        expectRangeValid(filter->getfppafROIMinXRatioRange());
        expectRangeValid(filter->getfppafROIMaxXRatioRange());
        expectRangeValid(filter->getfppafROIMinYRatioRange());
        expectRangeValid(filter->getfppafROIMaxYRatioRange());
        expectRangeValid(filter->getfppafMaxNoiseLevelRange());
        expectRangeValid(filter->getfppafMaxSpeckleSizeRange());

        EXPECT_NO_THROW(filter->process(depthFrame));
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_56_playback_disparity_transform) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    runPrivateFilterCase<ob::DisparityTransform>([&](const std::shared_ptr<ob::DisparityTransform> &filter) {
        EXPECT_EQ(filter->getName(), "DisparityTransform");
        EXPECT_TRUE(filter->is<ob::DisparityTransform>());
        EXPECT_NE(filter->as<ob::DisparityTransform>(), nullptr);

        EXPECT_NO_THROW(filter->process(depthFrame));
        EXPECT_NO_THROW(filter->reset());
    });
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_60_playback_coord_transform_2d_to_3d) {
    auto pbDevice   = createPlaybackDevice();
    auto depthFrame = getDepthFrameFromPlayback(pbDevice);
    if(!depthFrame) {
        GTEST_SKIP() << "No depth frame in playback bag";
    }

    auto sensorList        = pbDevice->getSensorList();
    auto depthVideoProfile = getFirstVideoProfile(sensorList, OB_SENSOR_DEPTH);
    if(!depthVideoProfile) {
        GTEST_SKIP() << "No depth video stream profile in playback bag";
    }

    auto width  = depthFrame->getWidth();
    auto height = depthFrame->getHeight();
    ASSERT_GT(width, 0u);
    ASSERT_GT(height, 0u);
    ASSERT_EQ(depthFrame->getFormat(), OB_FORMAT_Y16);

    auto *data = reinterpret_cast<const uint16_t *>(depthFrame->getData());
    ASSERT_NE(data, nullptr);

    uint32_t cx       = width / 2;
    uint32_t cy       = height / 2;
    uint32_t idx      = cy * width + cx;
    uint16_t rawDepth = data[idx];
    if(rawDepth == 0) {
        for(uint32_t i = 0; i < width * height; ++i) {
            if(data[i] != 0) {
                rawDepth = data[i];
                idx      = i;
                cx       = idx % width;
                cy       = idx / width;
                break;
            }
        }
    }
    if(rawDepth == 0) {
        GTEST_SKIP() << "Depth frame contains only zero values";
    }

    float depthMm = rawDepth * depthFrame->getValueScale();
    ASSERT_GT(depthMm, 0.0f);

    auto        intrinsic = depthVideoProfile->getIntrinsic();
    OBExtrinsic identity  = {};
    identity.rot[0]       = 1.0f;
    identity.rot[4]       = 1.0f;
    identity.rot[8]       = 1.0f;

    OBPoint2f pixel   = { static_cast<float>(cx), static_cast<float>(cy) };
    OBPoint3f point3d = {};
    bool      ok      = ob::CoordinateTransformHelper::transformation2dto3d(pixel, depthMm, intrinsic, identity, &point3d);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(point3d.z, depthMm, depthMm * 0.05f + 1.0f);
}

TEST_P(TC_CPP_18_PlaybackParam, TC_CPP_18_61_playback_coord_transform_3d_to_2d) {
    auto pbDevice          = createPlaybackDevice();
    auto sensorList        = pbDevice->getSensorList();
    auto depthVideoProfile = getFirstVideoProfile(sensorList, OB_SENSOR_DEPTH);
    if(!depthVideoProfile) {
        GTEST_SKIP() << "No depth video stream profile in playback bag";
    }

    auto        intrinsic  = depthVideoProfile->getIntrinsic();
    auto        distortion = depthVideoProfile->getDistortion();
    OBExtrinsic identity   = {};
    identity.rot[0]        = 1.0f;
    identity.rot[4]        = 1.0f;
    identity.rot[8]        = 1.0f;

    OBPoint3f src   = { 0.0f, 0.0f, 1000.0f };
    OBPoint2f pixel = {};
    bool      ok    = ob::CoordinateTransformHelper::transformation3dto2d(src, intrinsic, distortion, identity, &pixel);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(pixel.x, intrinsic.cx, 2.0f);
    EXPECT_NEAR(pixel.y, intrinsic.cy, 2.0f);
}

INSTANTIATE_TEST_SUITE_P(AllPlaybackBags, TC_CPP_18_PlaybackParam, ::testing::ValuesIn(TestEnvironment::instance().allPlaybackBagPaths()));

// ============================================================
// Test group: CoordTransform.
// ============================================================
class TC_CPP_20_CoordTransform : public SDKTestBase {};

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_01_3d_to_3d) {
    /// Test case: 3d to 3d.
    OBPoint3f   src      = { 100.0f, 200.0f, 300.0f };
    OBExtrinsic identity = {};
    // Prepare local state for the next check.
    identity.rot[0]   = 1.0f;
    identity.rot[4]   = 1.0f;
    identity.rot[8]   = 1.0f;
    identity.trans[0] = 0.0f;
    identity.trans[1] = 0.0f;
    identity.trans[2] = 0.0f;

    OBPoint3f dst = {};
    bool      ok  = ob::CoordinateTransformHelper::transformation3dto3d(src, identity, &dst);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dst.x, src.x, 1e-3f);
    EXPECT_NEAR(dst.y, src.y, 1e-3f);
    EXPECT_NEAR(dst.z, src.z, 1e-3f);

    // Prepare local state for the next check.
    OBExtrinsic withTrans = identity;
    withTrans.trans[0]    = 10.0f;
    withTrans.trans[1]    = 20.0f;
    withTrans.trans[2]    = 30.0f;
    ok                    = ob::CoordinateTransformHelper::transformation3dto3d(src, withTrans, &dst);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dst.x, src.x + 10.0f, 1e-3f);
    EXPECT_NEAR(dst.y, src.y + 20.0f, 1e-3f);
    EXPECT_NEAR(dst.z, src.z + 30.0f, 1e-3f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_02_2d_depth_to_3d) {
    /// Test case: 2d depth to 3d.
    OBCameraIntrinsic intrinsic = {};
    intrinsic.fx                = 500.0f;
    intrinsic.fy                = 500.0f;
    intrinsic.cx                = 320.0f;
    intrinsic.cy                = 240.0f;
    intrinsic.width             = 640;
    intrinsic.height            = 480;

    OBExtrinsic identity = {};
    identity.rot[0]      = 1.0f;
    identity.rot[4]      = 1.0f;
    identity.rot[8]      = 1.0f;

    // TODO
    OBPoint2f pixel   = { 320.0f, 240.0f };
    OBPoint3f point3d = {};
    bool      ok      = ob::CoordinateTransformHelper::transformation2dto3d(pixel, 1000.0f, intrinsic, identity, &point3d);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(point3d.x, 0.0f, 1.0f);
    EXPECT_NEAR(point3d.y, 0.0f, 1.0f);
    EXPECT_NEAR(point3d.z, 1000.0f, 1.0f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_03_3d_to_2d) {
    /// Test case: 3d to 2d.
    OBCameraIntrinsic intrinsic = {};
    intrinsic.fx                = 500.0f;
    intrinsic.fy                = 500.0f;
    intrinsic.cx                = 320.0f;
    intrinsic.cy                = 240.0f;
    intrinsic.width             = 640;
    intrinsic.height            = 480;

    OBCameraDistortion distortion = {};  // No distortion.

    OBExtrinsic identity = {};
    identity.rot[0]      = 1.0f;
    identity.rot[4]      = 1.0f;
    identity.rot[8]      = 1.0f;

    // TODO - add more cases with different intrinsics, distortion, and extrinsics
    OBPoint3f src   = { 0.0f, 0.0f, 1000.0f };
    OBPoint2f pixel = {};
    bool      ok    = ob::CoordinateTransformHelper::transformation3dto2d(src, intrinsic, distortion, identity, &pixel);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(pixel.x, 320.0f, 1.0f);
    EXPECT_NEAR(pixel.y, 240.0f, 1.0f);
}

TEST_F(TC_CPP_20_CoordTransform, TC_CPP_20_04_2d_to_2d) {
    /// Test case: 2d to 2d.
    OBCameraIntrinsic srcIntrinsic = {};
    srcIntrinsic.fx                = 500.0f;
    srcIntrinsic.fy                = 500.0f;
    srcIntrinsic.cx                = 320.0f;
    srcIntrinsic.cy                = 240.0f;
    srcIntrinsic.width             = 640;
    srcIntrinsic.height            = 480;

    OBCameraDistortion srcDist      = {};
    OBCameraIntrinsic  tgtIntrinsic = srcIntrinsic;  // Same intrinsic parameters.
    OBCameraDistortion tgtDist      = {};

    OBExtrinsic identity = {};
    identity.rot[0]      = 1.0f;
    identity.rot[4]      = 1.0f;
    identity.rot[8]      = 1.0f;

    // Same intrinsics + identity extrinsic → same pixel
    OBPoint2f srcPx = { 400.0f, 300.0f };
    OBPoint2f dstPx = {};
    bool      ok    = ob::CoordinateTransformHelper::transformation2dto2d(srcPx, 1000.0f, srcIntrinsic, srcDist, tgtIntrinsic, tgtDist, identity, &dstPx);
    ASSERT_TRUE(ok);
    EXPECT_NEAR(dstPx.x, srcPx.x, 2.0f);
    EXPECT_NEAR(dstPx.y, srcPx.y, 2.0f);
}

// ============================================================
// Test group: Version.
// ============================================================
class TC_CPP_22_Version : public SDKTestBase {};

TEST_F(TC_CPP_22_Version, TC_CPP_22_01_full_version) {
    /// Test case: full version.
    int ver = ob::Version::getVersion();
    EXPECT_GT(ver, 0) << "Full version should be > 0";
}

TEST_F(TC_CPP_22_Version, TC_CPP_22_02_major_minor_patch) {
    /// Test case: major minor patch.
    int major = ob::Version::getMajor();
    int minor = ob::Version::getMinor();
    int patch = ob::Version::getPatch();

    EXPECT_GE(major, 0);
    EXPECT_GE(minor, 0);
    EXPECT_GE(patch, 0);

    int expected = major * 10000 + minor * 100 + patch;
    EXPECT_EQ(ob::Version::getVersion(), expected) << "Version mismatch: " << major << "." << minor << "." << patch;
}

TEST_F(TC_CPP_22_Version, TC_CPP_22_03_stage_version) {
    /// Test case: stage version.
    const char *stage = ob::Version::getStageVersion();
    // Prepare local state for the next check.
    if(stage && std::strlen(stage) > 0) {
        std::string s(stage);
        // Prepare local state for the next check.
        bool known = (s == "alpha" || s == "beta" || s == "rc" || s == "release" || s.find("alpha") != std::string::npos || s.find("beta") != std::string::npos
                      || s.find("rc") != std::string::npos);
        EXPECT_TRUE(known) << "Unexpected stage version: " << s;
    }
}

// ============================================================
// Test group: Logger.
// ============================================================
class TC_CPP_23_Logger : public SDKTestBase {};

TEST_F(TC_CPP_23_Logger, TC_CPP_23_01_log_severity_set) {
    /// Test case: log severity set.
    const OBLogSeverity levels[] = {
        OB_LOG_SEVERITY_DEBUG, OB_LOG_SEVERITY_INFO, OB_LOG_SEVERITY_WARN, OB_LOG_SEVERITY_ERROR, OB_LOG_SEVERITY_FATAL, OB_LOG_SEVERITY_OFF,
    };
    for(auto level: levels) {
        ob_error *error = nullptr;
        ob_set_logger_severity(level, &error);
        EXPECT_EQ(error, nullptr) << "Failed to set log level: " << (int)level;
    }
    // restore a normal level
    ob_error *error = nullptr;
    ob_set_logger_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_02_log_to_file) {
    /// Test case: log to file.
#ifdef _WIN32
    std::string logDir  = ".";
    std::string logFile = ".\\OrbbecSDK.log.txt";
#else
    std::string logDir  = "/tmp";
    std::string logFile = "/tmp/OrbbecSDK.log.txt";
#endif

    // Remove temporary artifacts created by the test.
    std::remove(logFile.c_str());

    ob_error *error = nullptr;
    ob_set_logger_to_file(OB_LOG_SEVERITY_DEBUG, logDir.c_str(), &error);
    ASSERT_EQ(error, nullptr);

    // Prepare local state for the next check.
    {
        ob::Context ctx;
        ctx.queryDeviceList();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Prepare local state for the next check.
    std::ifstream ifs(logFile);
    EXPECT_TRUE(ifs.good()) << "Log file not created: " << logFile;
    if(ifs.good()) {
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        EXPECT_FALSE(content.empty()) << "Log file is empty";
    }

    // Remove temporary artifacts created by the test.
    std::remove(logFile.c_str());
    // Prepare local state for the next check.
    ob_set_logger_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_03_log_to_console) {
    /// Test case: log to console.
    ob_error *error = nullptr;
    ob_set_logger_to_console(OB_LOG_SEVERITY_INFO, &error);
    EXPECT_EQ(error, nullptr);

    // Prepare local state for the next check.
    ob_set_logger_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_04_log_callback) {
    /// Test case: log callback.
    std::atomic<int> logCount{ 0 };
    ob_error        *error = nullptr;
    ob_set_logger_to_callback(
        OB_LOG_SEVERITY_DEBUG,
        [](OBLogSeverity, const char *, void *userData) {
            auto *cnt = static_cast<std::atomic<int> *>(userData);
            (*cnt)++;
        },
        &logCount, &error);
    ASSERT_EQ(error, nullptr);

    // Prepare local state for the next check.
    {
        ob::Context ctx;
        ctx.queryDeviceList();
    }

    EXPECT_GT(logCount.load(), 0) << "No log callback received";

    // Restore
    ob_set_logger_severity(OB_LOG_SEVERITY_WARN, &error);
}

TEST_F(TC_CPP_23_Logger, TC_CPP_23_05_external_message) {
    /// Test case: external message.
    ob_error *error = nullptr;

    // Prepare local state for the next check.
    std::atomic<bool> found{ false };
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

    ob_log_external_message(OB_LOG_SEVERITY_INFO, "NOHW", "NOHW_TEST_MARKER", __FILE__, __func__, __LINE__, &error);
    EXPECT_EQ(error, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(found.load()) << "External message not captured";

    ob_set_logger_severity(OB_LOG_SEVERITY_WARN, &error);
}

// ============================================================
// Test group: Error.
// ============================================================
class TC_CPP_24_Error : public SDKTestBase {};

TEST_F(TC_CPP_24_Error, TC_CPP_24_01_exception_type_info) {
    /// Test case: exception type info.
    try {
        // Prepare local state for the next check.
        ob::Filter(ob_create_filter("TotallyInvalidFilter", nullptr));
        FAIL() << "Expected ob::Error";
    }
    catch(const ob::Error &e) {
        EXPECT_NE(e.what(), nullptr);
        EXPECT_GT(std::strlen(e.what()), 0u);
        // Prepare local state for the next check.
        // Prepare local state for the next check.
    }
    catch(const std::exception &e) {
        // Validate expected conditions for this step.
        EXPECT_NE(e.what(), nullptr);
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_02_invalid_value_exception) {
    /// Test case: invalid value exception.
    // Prepare local state for the next check.
    ob_error *error = nullptr;
    auto      frame = ob_create_video_frame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 0, 0, 0, &error);
    // Prepare local state for the next check.
    if(error) {
        auto exType = ob_error_get_exception_type(error);
        // Prepare local state for the next check.
        (void)exType;
        ob_delete_error(error);
        error = nullptr;
    }
    if(frame) {
        ob_delete_frame(frame, &error);
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_03_wrong_api_sequence) {
    /// Test case: wrong api sequence.
    try {
        ob::Pipeline pipeline;
        // Wait for frames to validate runtime behavior.
        auto frames = pipeline.waitForFrameset(100);
        // Validate expected conditions for this step.
        EXPECT_EQ(frames, nullptr);
    }
    catch(const ob::Error &) {
        // Expected
        SUCCEED();
    }
}

TEST_F(TC_CPP_24_Error, TC_CPP_24_06_filter_null_frame) {
    /// Test case: filter null frame.
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
    /// Test case: all exception types.
    // Prepare local state for the next check.
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
    // Prepare local state for the next check.
    std::sort(types.begin(), types.end());
    auto dup = std::adjacent_find(types.begin(), types.end());
    EXPECT_EQ(dup, types.end()) << "Duplicate exception type found";
}

// ============================================================
// Test group: DataStruct.
// ============================================================
class TC_CPP_25_DataStruct : public SDKTestBase {};

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_05_config_mode_structs) {
    /// Test case: config mode structs.
    {
        OBDepthWorkMode mode = {};
        EXPECT_EQ(std::strlen(mode.name), 0u);
        std::strncpy(mode.name, "TestMode", sizeof(mode.name) - 1);
        EXPECT_STREQ(mode.name, "TestMode");
    }
    {
        OBMultiDeviceSyncConfig config = {};
        config.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
        config.depthDelayUs            = 0;
        config.colorDelayUs            = 0;
        config.trigger2ImageDelayUs    = 0;
        config.triggerOutEnable        = false;
        config.triggerOutDelayUs       = 0;
        config.framesPerTrigger        = 1;
        EXPECT_EQ(config.syncMode, OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN);
    }
    {
        OBNetIpConfig ipConfig = {};
        ipConfig.dhcp          = 0;
        EXPECT_EQ(ipConfig.dhcp, 0);
    }
}

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_06_hdr_roi_point_imu_structs) {
    /// Test case: hdr roi point imu structs.
    {
        OBHdrConfig hdr   = {};
        hdr.enable        = true;
        hdr.sequence_name = 0;
        hdr.exposure_1    = 3000;
        hdr.gain_1        = 16;
        hdr.exposure_2    = 100;
        hdr.gain_2        = 16;
        EXPECT_TRUE(hdr.enable);
    }
    {
        OBRegionOfInterest roi = {};
        roi.x0_left            = 0;
        roi.y0_top             = 0;
        roi.x1_right           = 640;
        roi.y1_bottom          = 480;
        EXPECT_EQ(roi.x1_right, 640);
    }
    {
        OBPoint3f p = { 1.0f, 2.0f, 3.0f };
        EXPECT_FLOAT_EQ(p.z, 3.0f);
    }
    {
        OBAccelValue accel = { 0.0f, 0.0f, 9.8f };
        EXPECT_NEAR(accel.z, 9.8f, 0.01f);
    }
}

TEST_F(TC_CPP_25_DataStruct, TC_CPP_25_07_property_range_structs) {
    /// Test case: property range structs.
    {
        OBIntPropertyRange range = {};
        range.min                = 0;
        range.max                = 100;
        range.step               = 1;
        range.cur                = 50;
        range.def                = 50;
        EXPECT_GT(range.max, range.min);
        EXPECT_GE(range.cur, range.min);
        EXPECT_LE(range.cur, range.max);
    }
    {
        OBFloatPropertyRange range = {};
        range.min                  = 0.0f;
        range.max                  = 1.0f;
        range.step                 = 0.1f;
        range.cur                  = 0.5f;
        range.def                  = 0.5f;
        EXPECT_GT(range.max, range.min);
    }
    {
        OBFilterConfigSchemaItem item = {};
        item.name                     = "test_param";
        item.min                      = 0.0;
        item.max                      = 100.0;
        item.step                     = 1.0;
        item.def                      = 50.0;
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
