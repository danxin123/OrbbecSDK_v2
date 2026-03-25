// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Record & Playback — Record sensor streams to .bag and play them back.
// Demonstrates: RecordDevice, PlaybackDevice, Pipeline callback, Config enableStream.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <atomic>
#include <map>
#include <condition_variable>

// ============================================================
// Record mode
// ============================================================
void doRecord() {
    std::cout << "\n=== RECORD MODE ===" << std::endl;
    std::cout << "Enter output filename (.bag): ";
    std::string filePath;
    std::getline(std::cin, filePath);
    if(filePath.empty())
        filePath = "output.bag";

    auto context    = std::make_shared<ob::Context>();
    auto deviceList = context->queryDeviceList();
    if(deviceList->getCount() < 1) {
        std::cout << "No device found!" << std::endl;
        return;
    }

    auto device  = deviceList->getDevice(0);
    auto devInfo = device->getDeviceInfo();
    std::cout << "Recording from: " << devInfo->getName() << " (SN: " << devInfo->getSerialNumber() << ")" << std::endl;

    auto pipe = std::make_shared<ob::Pipeline>(device);

    // Sync device clock
    try {
        device->timerSyncWithHost();
    }
    catch(ob::Error &e) {
        std::cerr << "timerSyncWithHost: " << e.what() << std::endl;
    }

    // Enable all available streams
    auto config     = std::make_shared<ob::Config>();
    auto sensorList = device->getSensorList();
    for(uint32_t i = 0; i < sensorList->getCount(); i++) {
        config->enableStream(sensorList->getSensorType(i));
    }

    // Frame counting for FPS display
    std::mutex                      frameMutex;
    std::map<OBFrameType, uint64_t> frameCountMap;
    std::shared_ptr<const ob::FrameSet> renderFrameSet;

    pipe->start(config, [&](std::shared_ptr<ob::FrameSet> frameSet) {
        if(!frameSet)
            return;
        std::lock_guard<std::mutex> lock(frameMutex);
        renderFrameSet = frameSet;
        auto count = frameSet->getCount();
        for(uint32_t i = 0; i < count; i++) {
            auto frame = frameSet->getFrameByIndex(i);
            if(frame)
                frameCountMap[frame->getType()]++;
        }
    });

    // Start recording
    auto recordDevice = std::make_shared<ob::RecordDevice>(device, filePath);
    std::cout << "Recording started! Press ESC to stop." << std::endl;
    std::cout << "IMPORTANT: Always use ESC to stop! Otherwise, the bag file may be corrupted.\n" << std::endl;

    ob_smpl::CVWindow win("Recording", 1280, 720, ob_smpl::ARRANGE_GRID);

    std::atomic<bool> isPaused{false};
    win.setKeyPrompt("'S': Pause/Resume recording, 'Esc': Stop & save");
    win.setKeyPressedCallback([&](int key) {
        if(key == 's' || key == 'S') {
            if(!isPaused) {
                recordDevice->pause();
                isPaused = true;
                win.addLog("[PAUSED] Recording paused");
            }
            else {
                recordDevice->resume();
                isPaused = false;
                win.addLog("[RESUMED] Recording resumed");
            }
        }
    });

    auto startTime = ob_smpl::getNowTimesMs();

    while(win.run()) {
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            if(renderFrameSet)
                win.pushFramesToView(renderFrameSet);
        }

        // Print FPS every 2 seconds
        auto now = ob_smpl::getNowTimesMs();
        if(now - startTime > 2000) {
            std::lock_guard<std::mutex> lock(frameMutex);
            std::string                 fps;
            for(const auto &item : frameCountMap) {
                auto  name = ob::TypeHelper::convertOBFrameTypeToString(item.first);
                float rate = item.second / ((now - startTime) / 1000.0f);
                if(!fps.empty())
                    fps += ", ";
                fps += std::string(name) + "=" + ob_smpl::toString(rate, 1);
            }
            if(!fps.empty())
                win.addLog("FPS: " + fps);
            for(auto &item : frameCountMap)
                item.second = 0;
            startTime = now;
        }
    }

    pipe->stop();
    recordDevice = nullptr;  // Flush and save
    std::cout << "Recording saved to: " << filePath << std::endl;
}

// ============================================================
// Playback mode
// ============================================================
void doPlayback() {
    std::cout << "\n=== PLAYBACK MODE ===" << std::endl;
    std::cout << "Enter .bag file path: ";
    std::string filePath;
    std::getline(std::cin, filePath);

    // Remove surrounding quotes if present
    if(!filePath.empty() && (filePath.front() == '\'' || filePath.front() == '\"'))
        filePath = filePath.substr(1, filePath.size() - 2);

    if(filePath.size() <= 4 || filePath.substr(filePath.size() - 4) != ".bag") {
        std::cout << "Invalid file format. Please provide a .bag file." << std::endl;
        return;
    }

    auto playback = std::make_shared<ob::PlaybackDevice>(filePath);
    auto pipe     = std::make_shared<ob::Pipeline>(playback);

    std::cout << "Duration: " << playback->getDuration() << "ms" << std::endl;

    auto config = std::make_shared<ob::Config>();
    auto sensorList = playback->getSensorList();
    for(uint32_t i = 0; i < sensorList->getCount(); i++) {
        config->enableStream(sensorList->getSensorType(i));
    }
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION);

    std::mutex                          frameMutex;
    std::shared_ptr<const ob::FrameSet> renderFrameSet;

    auto frameCallback = [&](std::shared_ptr<ob::FrameSet> frameSet) {
        std::lock_guard<std::mutex> lock(frameMutex);
        renderFrameSet = frameSet;
    };

    std::atomic<bool>             exited(false);
    std::atomic<OBPlaybackStatus> playStatus(OB_PLAYBACK_STOPPED);
    std::mutex                    replayMutex;
    std::condition_variable       replayCv;

    playback->setPlaybackStatusChangeCallback([&](OBPlaybackStatus status) {
        playStatus = status;
        replayCv.notify_all();
    });

    pipe->start(config, frameCallback);

    // Auto-replay thread
    auto replayThread = std::thread([&] {
        while(!exited) {
            std::unique_lock<std::mutex> lock(replayMutex);
            replayCv.wait(lock, [&] { return exited.load() || playStatus.load() == OB_PLAYBACK_STOPPED; });
            if(exited)
                break;
            if(playStatus == OB_PLAYBACK_STOPPED) {
                pipe->stop();
                replayCv.wait_for(lock, std::chrono::milliseconds(1000), [&] { return exited.load(); });
                if(exited)
                    break;
                playStatus = OB_PLAYBACK_UNKNOWN;
                std::cout << "Replaying..." << std::endl;
                pipe->start(config, frameCallback);
            }
        }
    });

    ob_smpl::CVWindow win("Playback", 1280, 720, ob_smpl::ARRANGE_GRID);
    win.setKeyPrompt("'Esc': Exit");
    while(win.run() && !exited) {
        std::lock_guard<std::mutex> lock(frameMutex);
        if(renderFrameSet)
            win.pushFramesToView(renderFrameSet);
    }

    pipe->stop();
    exited = true;
    replayCv.notify_all();
    if(replayThread.joinable())
        replayThread.join();

    std::cout << "Playback finished." << std::endl;
}

// ============================================================
// Main menu
// ============================================================
int main(void) try {
    while(true) {
        std::cout << "\n=== Record & Playback ===" << std::endl;
        std::cout << "  1. Record streams to .bag" << std::endl;
        std::cout << "  2. Playback .bag file" << std::endl;
        std::cout << "  q. Quit" << std::endl;
        std::cout << "Choice: ";

        std::string choice;
        std::getline(std::cin, choice);
        if(choice == "q" || choice == "Q")
            break;
        if(choice == "1")
            doRecord();
        else if(choice == "2")
            doPlayback();
        else
            std::cout << "Invalid choice." << std::endl;
    }

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
