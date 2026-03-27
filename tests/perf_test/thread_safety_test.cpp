// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/// @file thread_safety_test.cpp
/// Thread-safety tests: verify that concurrent multi-threaded access to
/// OrbbecSDK APIs does not cause crashes, deadlocks, or data corruption.
///
/// These tests are intentionally separate from the frame-drop / timestamp
/// continuity tests in perf_test.cpp because they target a different concern
/// (robustness under concurrency rather than streaming performance metrics).
///
/// Run selectively:  ob_perf_test --gtest_filter=ThreadSafetyTest.*

#include "../test_common.hpp"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// ===================================================================
// Configuration
// ===================================================================
static int tsGetEnvInt(const char *name, int defaultVal) {
    const char *val = std::getenv(name);
    if(val) {
        int v = std::atoi(val);
        if(v > 0)
            return v;
    }
    return defaultVal;
}

static int threadSafetyDurationSec() {
    return tsGetEnvInt("TS_DURATION_SECONDS", 10);
}

static int threadSafetyThreadCount() {
    return tsGetEnvInt("TS_THREAD_COUNT", 4);
}

// ===================================================================
// Fixture: ThreadSafetyTest
// ===================================================================
class ThreadSafetyTest : public DeviceTest {
protected:
    int durationSec_ = 10;
    int threadCount_ = 4;

    void SetUp() override {
        DeviceTest::SetUp();
        durationSec_ = threadSafetyDurationSec();
        threadCount_ = threadSafetyThreadCount();
        std::cout << "[ThreadSafety] duration=" << durationSec_ << "s, threads=" << threadCount_ << std::endl;
    }
};

// -------------------------------------------------------------------
// TC_TS_01: Multiple threads concurrently start/stop the same Pipeline
// -------------------------------------------------------------------
// This is the most common misuse pattern: several threads racing to
// control a single Pipeline instance.  The SDK must not crash or
// deadlock regardless of the interleaving.
TEST_F(ThreadSafetyTest, TC_TS_01_concurrent_pipeline_start_stop) {
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    ASSERT_NE(pipeline, nullptr);

    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    std::atomic<bool> running{ true };
    std::atomic<int>  errors{ 0 };

    auto worker = [&](int /*id*/) {
        while(running.load()) {
            try {
                pipeline->start(config);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                pipeline->stop();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            catch(const ob::Error &e) {
                // SDK may legitimately reject concurrent start/stop with an
                // exception — that is acceptable as long as it does not crash.
                std::cout << "[TC_TS_01] ob::Error in thread: " << e.what() << std::endl;
            }
            catch(const std::exception &e) {
                std::cerr << "[TC_TS_01] std::exception in thread: " << e.what() << std::endl;
                errors.fetch_add(1);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(threadCount_);
    for(int i = 0; i < threadCount_; ++i) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(durationSec_));
    running.store(false);

    for(auto &t: threads) {
        t.join();
    }

    // Ensure the pipeline can still be used normally after the stress.
    try {
        pipeline->stop();
    }
    catch(...) {
        // May already be stopped — ignore.
    }

    EXPECT_EQ(errors.load(), 0) << "Unexpected std::exception caught during concurrent start/stop";
    std::cout << "[TC_TS_01] Completed without crash or deadlock." << std::endl;
}

// -------------------------------------------------------------------
// TC_TS_02: One thread streams frames, another reads/writes device
//           properties concurrently.
// -------------------------------------------------------------------
TEST_F(ThreadSafetyTest, TC_TS_02_stream_while_property_access) {
    auto pipeline = std::make_shared<ob::Pipeline>(device_);
    ASSERT_NE(pipeline, nullptr);

    auto config = std::make_shared<ob::Config>();
    config->enableStream(OB_STREAM_DEPTH);

    // Collect supported read-write int properties for the stress loop.
    struct PropEntry {
        OBPropertyID id;
        int32_t      minVal;
        int32_t      maxVal;
    };
    std::vector<PropEntry> rwProps;

    int propCount = device_->getSupportedPropertyCount();
    for(int i = 0; i < propCount; ++i) {
        auto item = device_->getSupportedProperty(static_cast<uint32_t>(i));
        if(item.permission == OB_PERMISSION_READ_WRITE && item.type == OB_INT_PROPERTY) {
            try {
                auto range = device_->getIntPropertyRange(item.id);
                rwProps.push_back({ item.id, range.min, range.max });
            }
            catch(...) {
                // Skip properties whose range can't be queried.
            }
        }
    }

    // Start streaming.
    std::atomic<bool>     running{ true };
    std::atomic<uint64_t> frameCount{ 0 };
    std::atomic<int>      streamErrors{ 0 };
    std::atomic<int>      propErrors{ 0 };

    pipeline->start(config);

    // Thread A: continuously pull frames.
    std::thread streamThread([&]() {
        while(running.load()) {
            try {
                auto frameset = pipeline->waitForFrameset(200);
                if(frameset) {
                    frameCount.fetch_add(1);
                }
            }
            catch(const ob::Error &) {
                // Acceptable SDK error during concurrent property writes.
            }
            catch(const std::exception &e) {
                std::cerr << "[TC_TS_02-stream] " << e.what() << std::endl;
                streamErrors.fetch_add(1);
            }
        }
    });

    // Thread B: repeatedly get/set device properties.
    std::thread propThread([&]() {
        while(running.load()) {
            for(const auto &p: rwProps) {
                if(!running.load())
                    break;
                try {
                    int32_t cur = device_->getIntProperty(p.id);
                    // Write back the same value to avoid changing device state.
                    device_->setIntProperty(p.id, cur);
                }
                catch(const ob::Error &) {
                    // Property access may fail under concurrent streaming — acceptable.
                }
                catch(const std::exception &e) {
                    std::cerr << "[TC_TS_02-prop] " << e.what() << std::endl;
                    propErrors.fetch_add(1);
                }
            }
            if(rwProps.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(durationSec_));
    running.store(false);

    streamThread.join();
    propThread.join();

    pipeline->stop();

    std::cout << "[TC_TS_02] frames=" << frameCount.load() << ", rw_properties=" << rwProps.size() << std::endl;

    EXPECT_EQ(streamErrors.load(), 0) << "Unexpected std::exception in stream thread";
    EXPECT_EQ(propErrors.load(), 0) << "Unexpected std::exception in property thread";
    EXPECT_GT(frameCount.load(), 0u) << "No frames received during the test";
    std::cout << "[TC_TS_02] Completed without crash or deadlock." << std::endl;
}

// -------------------------------------------------------------------
// TC_TS_03: Multiple threads each create, use, and destroy their own
//           Pipeline instance simultaneously (resource contention on
//           the shared device / USB bus).
// -------------------------------------------------------------------
TEST_F(ThreadSafetyTest, TC_TS_03_concurrent_pipeline_create_destroy) {
    std::atomic<bool> running{ true };
    std::atomic<int>  errors{ 0 };
    std::atomic<int>  iterations{ 0 };

    auto worker = [&](int /*id*/) {
        while(running.load()) {
            try {
                auto pipeline = std::make_shared<ob::Pipeline>(device_);
                auto config   = std::make_shared<ob::Config>();
                config->enableStream(OB_STREAM_DEPTH);
                pipeline->start(config);

                // Pull a few frames, then tear down.
                for(int f = 0; f < 5 && running.load(); ++f) {
                    auto frameset = pipeline->waitForFrameset(500);
                    (void)frameset;
                }

                pipeline->stop();
                iterations.fetch_add(1);
            }
            catch(const ob::Error &e) {
                // SDK may reject overlapping pipeline instances on the same
                // device — this is acceptable as long as it doesn't crash.
                std::cout << "[TC_TS_03] ob::Error: " << e.what() << std::endl;
            }
            catch(const std::exception &e) {
                std::cerr << "[TC_TS_03] std::exception: " << e.what() << std::endl;
                errors.fetch_add(1);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(threadCount_);
    for(int i = 0; i < threadCount_; ++i) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(durationSec_));
    running.store(false);

    for(auto &t: threads) {
        t.join();
    }

    std::cout << "[TC_TS_03] iterations=" << iterations.load() << std::endl;

    EXPECT_EQ(errors.load(), 0) << "Unexpected std::exception during concurrent create/destroy";
    std::cout << "[TC_TS_03] Completed without crash or deadlock." << std::endl;
}
