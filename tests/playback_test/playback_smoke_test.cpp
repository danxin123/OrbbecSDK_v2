// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include <atomic>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr << "Usage: ob_playback_smoke_test <path-to-bag>" << std::endl;
        return 2;
    }

    const std::string bagPath = argv[1];

    try {
        auto playback = std::make_shared<ob::PlaybackDevice>(bagPath);
        auto pipeline = std::make_shared<ob::Pipeline>(playback);
        auto config   = std::make_shared<ob::Config>();

        auto sensorList = playback->getSensorList();
        if(sensorList->getCount() == 0) {
            std::cerr << "No sensors found in playback file: " << bagPath << std::endl;
            return 1;
        }

        for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
            config->enableStream(sensorList->getSensorType(i));
        }
        config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);

        std::atomic<int> frameSetCount(0);
        std::shared_ptr<ob::DepthFrame> firstDepthFrame;
        std::vector<uint64_t> depthTimestamps;

        pipeline->start(config);

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while(std::chrono::steady_clock::now() < deadline && frameSetCount.load(std::memory_order_relaxed) < 30) {
            auto frameSet = pipeline->waitForFrames(3000);
            if(frameSet == nullptr) {
                continue;
            }

            frameSetCount.fetch_add(1, std::memory_order_relaxed);
            auto depthFrame = frameSet->depthFrame();
            if(depthFrame != nullptr) {
                if(firstDepthFrame == nullptr) {
                    firstDepthFrame = depthFrame;
                }
                depthTimestamps.push_back(depthFrame->timeStampUs());
            }
        }

        pipeline->stop();

        const int received = frameSetCount.load(std::memory_order_relaxed);
        std::cout << "playback frame sets received: " << received << std::endl;

        if(received <= 0) {
            std::cerr << "Playback smoke test failed: no frame set received." << std::endl;
            return 1;
        }

        if(firstDepthFrame == nullptr) {
            std::cerr << "Playback smoke test failed: no depth frame received." << std::endl;
            return 1;
        }

        // TC_CPP_10_01 subset: basic frame property integrity.
        if(firstDepthFrame->data() == nullptr || firstDepthFrame->dataSize() == 0) {
            std::cerr << "Playback smoke test failed: invalid depth frame basic properties." << std::endl;
            return 1;
        }

        if(firstDepthFrame->timeStampUs() == 0 || firstDepthFrame->systemTimeStampUs() == 0) {
            std::cerr << "Playback smoke test failed: invalid frame timestamps." << std::endl;
            return 1;
        }

        // TC_CPP_10_05 subset: video frame dimensions.
        if(firstDepthFrame->width() == 0 || firstDepthFrame->height() == 0) {
            std::cerr << "Playback smoke test failed: invalid depth frame width/height." << std::endl;
            return 1;
        }

        // TC_CPP_10_02 subset: timestamp monotonicity.
        if(depthTimestamps.size() < 2) {
            std::cerr << "Playback smoke test failed: insufficient depth frame samples for monotonicity check." << std::endl;
            return 1;
        }

        for(size_t i = 1; i < depthTimestamps.size(); ++i) {
            if(depthTimestamps[i] < depthTimestamps[i - 1]) {
                std::cerr << "Playback smoke test failed: depth timestamp is not monotonic." << std::endl;
                return 1;
            }
        }

        // TC_CPP_11 subset: metadata availability and value access.
        const auto hasTsMeta = firstDepthFrame->hasMetadata(OB_FRAME_METADATA_TYPE_TIMESTAMP);
        const auto hasSensorTsMeta = firstDepthFrame->hasMetadata(OB_FRAME_METADATA_TYPE_SENSOR_TIMESTAMP);
        const auto hasFrameNumMeta = firstDepthFrame->hasMetadata(OB_FRAME_METADATA_TYPE_FRAME_NUMBER);
        const auto metadataSize = firstDepthFrame->metadataSize();

        bool hasAnyMetadata = false;
        if(hasTsMeta) {
            hasAnyMetadata = true;
            const auto ts = firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_TIMESTAMP);
            if(ts <= 0) {
                std::cerr << "Playback smoke test failed: invalid TIMESTAMP metadata value." << std::endl;
                return 1;
            }
        }
        if(hasSensorTsMeta) {
            hasAnyMetadata = true;
            const auto sts = firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_SENSOR_TIMESTAMP);
            if(sts <= 0) {
                std::cerr << "Playback smoke test failed: invalid SENSOR_TIMESTAMP metadata value." << std::endl;
                return 1;
            }
        }
        if(hasFrameNumMeta) {
            hasAnyMetadata = true;
            (void)firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_FRAME_NUMBER);
        }

        if(!hasAnyMetadata && metadataSize == 0) {
            std::cerr << "Playback smoke test failed: no metadata found on first depth frame." << std::endl;
            return 1;
        }

        // TC_CPP_11_04 subset: raw metadata buffer readability and consistency.
        if(metadataSize > 0) {
            auto rawMetaBefore = firstDepthFrame->metadata();
            if(rawMetaBefore == nullptr) {
                std::cerr << "Playback smoke test failed: metadataSize > 0 but metadata buffer is null." << std::endl;
                return 1;
            }

            std::vector<uint8_t> snapshotBefore(rawMetaBefore, rawMetaBefore + metadataSize);

            // Read APIs should not mutate raw metadata contents.
            if(hasTsMeta) {
                (void)firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_TIMESTAMP);
            }
            if(hasSensorTsMeta) {
                (void)firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_SENSOR_TIMESTAMP);
            }
            if(hasFrameNumMeta) {
                (void)firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_FRAME_NUMBER);
            }

            if(firstDepthFrame->metadataSize() != metadataSize) {
                std::cerr << "Playback smoke test failed: metadata size changed after metadata reads." << std::endl;
                return 1;
            }

            auto rawMetaAfter = firstDepthFrame->metadata();
            if(rawMetaAfter == nullptr) {
                std::cerr << "Playback smoke test failed: metadata buffer becomes null after reads." << std::endl;
                return 1;
            }

            if(std::memcmp(snapshotBefore.data(), rawMetaAfter, metadataSize) != 0) {
                std::cerr << "Playback smoke test failed: raw metadata buffer changed unexpectedly after reads." << std::endl;
                return 1;
            }
        }

        // TC_CPP_12_01/12_02/12_04 subsets: FrameFactory create/clone/frameset operations.
        auto createdFrame = ob::FrameFactory::createFrame(firstDepthFrame->type(), firstDepthFrame->format(), firstDepthFrame->dataSize());
        if(createdFrame == nullptr || createdFrame->type() != firstDepthFrame->type() || createdFrame->format() != firstDepthFrame->format()
           || createdFrame->dataSize() != firstDepthFrame->dataSize()) {
            std::cerr << "Playback smoke test failed: createFrame returned invalid frame." << std::endl;
            return 1;
        }

        auto createdVideoFrame = ob::FrameFactory::createVideoFrame(firstDepthFrame->type(), firstDepthFrame->format(), firstDepthFrame->width(),
                                                                     firstDepthFrame->height(), 0);
        if(createdVideoFrame == nullptr || createdVideoFrame->width() != firstDepthFrame->width()
           || createdVideoFrame->height() != firstDepthFrame->height()) {
            std::cerr << "Playback smoke test failed: createVideoFrame returned invalid video frame." << std::endl;
            return 1;
        }

        auto clonedFrame = ob::FrameFactory::createFrameFromOtherFrame(firstDepthFrame, true);
        if(clonedFrame == nullptr || clonedFrame->type() != firstDepthFrame->type() || clonedFrame->format() != firstDepthFrame->format()
           || clonedFrame->dataSize() != firstDepthFrame->dataSize()) {
            std::cerr << "Playback smoke test failed: createFrameFromOtherFrame returned invalid clone." << std::endl;
            return 1;
        }

        if(clonedFrame->data() == nullptr || firstDepthFrame->data() == nullptr) {
            std::cerr << "Playback smoke test failed: clone data buffer is null." << std::endl;
            return 1;
        }

        if(clonedFrame->data() == firstDepthFrame->data()) {
            std::cerr << "Playback smoke test failed: clone data buffer aliases source buffer unexpectedly." << std::endl;
            return 1;
        }

        if(std::memcmp(clonedFrame->data(), firstDepthFrame->data(), firstDepthFrame->dataSize()) != 0) {
            std::cerr << "Playback smoke test failed: clone data does not match source data." << std::endl;
            return 1;
        }

        auto createdFrameSet = ob::FrameFactory::createFrameSet();
        if(createdFrameSet == nullptr || createdFrameSet->frameCount() != 0) {
            std::cerr << "Playback smoke test failed: createFrameSet initial state is invalid." << std::endl;
            return 1;
        }
        createdFrameSet->pushFrame(createdFrame);
        if(createdFrameSet->frameCount() != 1) {
            std::cerr << "Playback smoke test failed: createFrameSet pushFrame did not increase frame count." << std::endl;
            return 1;
        }

        // TC_CPP_12_02 subset: create frame from stream profile of playback frame.
        auto sourceProfile = firstDepthFrame->getStreamProfile();
        if(sourceProfile == nullptr) {
            std::cerr << "Playback smoke test failed: first depth frame stream profile is null." << std::endl;
            return 1;
        }

        auto frameFromProfile = ob::FrameFactory::createFrameFromStreamProfile(sourceProfile);
        if(frameFromProfile == nullptr || frameFromProfile->format() != sourceProfile->format()) {
            std::cerr << "Playback smoke test failed: createFrameFromStreamProfile returned invalid frame." << std::endl;
            return 1;
        }

        if(sourceProfile->is<ob::VideoStreamProfile>()) {
            auto sourceVideoProfile = sourceProfile->as<ob::VideoStreamProfile>();
            auto createdVideo = frameFromProfile->as<ob::VideoFrame>();
            if(createdVideo == nullptr || sourceVideoProfile == nullptr || createdVideo->width() != sourceVideoProfile->width()
               || createdVideo->height() != sourceVideoProfile->height()) {
                std::cerr << "Playback smoke test failed: frame created from stream profile has mismatched dimensions." << std::endl;
                return 1;
            }
        }

        // TC_CPP_12_03 subset: wrap external buffers and verify destroy callbacks.
        std::atomic<int> externalDestroyCount(0);
        {
            const auto externalSize = firstDepthFrame->dataSize();
            auto *externalBuffer = new uint8_t[externalSize];
            std::memcpy(externalBuffer, firstDepthFrame->data(), externalSize);

            auto wrappedFrame = ob::FrameFactory::createFrameFromBuffer(
                firstDepthFrame->type(), firstDepthFrame->format(), externalBuffer,
                [&externalDestroyCount](uint8_t *buffer) {
                    delete[] buffer;
                    externalDestroyCount.fetch_add(1, std::memory_order_relaxed);
                },
                externalSize);

            if(wrappedFrame == nullptr || wrappedFrame->data() == nullptr || wrappedFrame->dataSize() != externalSize
               || wrappedFrame->data() != externalBuffer) {
                std::cerr << "Playback smoke test failed: createFrameFromBuffer returned invalid frame." << std::endl;
                return 1;
            }
        }

        if(externalDestroyCount.load(std::memory_order_relaxed) != 1) {
            std::cerr << "Playback smoke test failed: frame buffer destroy callback was not called exactly once." << std::endl;
            return 1;
        }

        std::atomic<int> externalVideoDestroyCount(0);
        {
            const auto externalVideoSize = firstDepthFrame->dataSize();
            auto *externalVideoBuffer = new uint8_t[externalVideoSize];
            std::memcpy(externalVideoBuffer, firstDepthFrame->data(), externalVideoSize);

            auto wrappedVideoFrame = ob::FrameFactory::createVideoFrameFromBuffer(
                firstDepthFrame->type(), firstDepthFrame->format(), firstDepthFrame->width(), firstDepthFrame->height(), externalVideoBuffer,
                [&externalVideoDestroyCount](uint8_t *buffer) {
                    delete[] buffer;
                    externalVideoDestroyCount.fetch_add(1, std::memory_order_relaxed);
                },
                externalVideoSize, 0);

            if(wrappedVideoFrame == nullptr || wrappedVideoFrame->data() == nullptr || wrappedVideoFrame->dataSize() != externalVideoSize
               || wrappedVideoFrame->data() != externalVideoBuffer || wrappedVideoFrame->width() != firstDepthFrame->width()
               || wrappedVideoFrame->height() != firstDepthFrame->height()) {
                std::cerr << "Playback smoke test failed: createVideoFrameFromBuffer returned invalid frame." << std::endl;
                return 1;
            }
        }

        if(externalVideoDestroyCount.load(std::memory_order_relaxed) != 1) {
            std::cerr << "Playback smoke test failed: video frame buffer destroy callback was not called exactly once." << std::endl;
            return 1;
        }

        // TC_CPP_11_05 subset: unsupported metadata field should be handled safely.
        const auto hasGpioInputMeta = firstDepthFrame->hasMetadata(OB_FRAME_METADATA_TYPE_GPIO_INPUT_DATA);
        if(!hasGpioInputMeta) {
            bool handledSafely = false;
            try {
                (void)firstDepthFrame->getMetadataValue(OB_FRAME_METADATA_TYPE_GPIO_INPUT_DATA);
                handledSafely = true;
            }
            catch(const ob::Error &) {
                handledSafely = true;
            }
            catch(...) {
                handledSafely = true;
            }

            if(!handledSafely) {
                std::cerr << "Playback smoke test failed: unsupported metadata access is not handled safely." << std::endl;
                return 1;
            }
        }

        // TC_CPP_08 config switch fallback in current SDK: stop/start with a new config.
        auto config2 = std::make_shared<ob::Config>();
        auto depthProfiles = pipeline->getStreamProfileList(OB_SENSOR_DEPTH);
        if(depthProfiles != nullptr && depthProfiles->count() > 0) {
            config2->enableStream(depthProfiles->getProfile(0));
        }
        else {
            // Fallback: re-enable all available sensors if depth profile list is unavailable.
            for(uint32_t i = 0; i < sensorList->getCount(); ++i) {
                config2->enableStream(sensorList->getSensorType(i));
            }
        }
        config2->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);

        pipeline->start(config2);
        int switchedFrameCount = 0;
        for(int i = 0; i < 10; ++i) {
            auto fs = pipeline->waitForFrames(3000);
            if(fs != nullptr) {
                ++switchedFrameCount;
            }
        }
        pipeline->stop();

        if(switchedFrameCount <= 0) {
            std::cerr << "Playback smoke test failed: no frame after config switch fallback." << std::endl;
            return 1;
        }

        return 0;
    }
    catch(const ob::Error &e) {
        std::cerr << "Function: " << e.getFunction() << "\n"
                  << "Args: " << e.getArgs() << "\n"
                  << "Message: " << e.what() << "\n"
                  << "Type: " << e.getExceptionType() << std::endl;
        return 1;
    }
}
