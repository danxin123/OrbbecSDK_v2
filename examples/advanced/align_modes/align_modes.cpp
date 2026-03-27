// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Align Modes — Compare Hardware vs Software Depth-to-Color alignment.
// Demonstrates: ALIGN_D2C_HW_MODE, ob::Align (SW), ALIGN_DISABLE, pipeline restart.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>

enum AlignMode { MODE_DISABLE = 0, MODE_SW, MODE_HW, MODE_COUNT };
static const char *modeNames[] = { "DISABLED", "SOFTWARE (Align filter)", "HARDWARE (D2C HW)" };

// Check if the current pipeline supports HW D2C for the given color+depth profiles.
bool checkHWD2CSupport(std::shared_ptr<ob::Pipeline> pipe, std::shared_ptr<ob::StreamProfile> colorProfile, std::shared_ptr<ob::StreamProfile> depthProfile) {
    auto hwProfiles = pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE);
    if(hwProfiles->count() == 0)
        return false;

    auto depthVsp = depthProfile->as<ob::VideoStreamProfile>();
    for(uint32_t i = 0; i < hwProfiles->getCount(); i++) {
        auto vsp = hwProfiles->getProfile(i)->as<ob::VideoStreamProfile>();
        if(vsp->getWidth() == depthVsp->getWidth() && vsp->getHeight() == depthVsp->getHeight() && vsp->getFormat() == depthVsp->getFormat()
           && vsp->getFps() == depthVsp->getFps()) {
            return true;
        }
    }
    return false;
}

int main(void) try {
    auto pipe = std::make_shared<ob::Pipeline>();
    pipe->enableFrameSync();

    // Find compatible color + depth profiles
    auto colorProfiles = pipe->getStreamProfileList(OB_SENSOR_COLOR);
    auto depthProfiles = pipe->getStreamProfileList(OB_SENSOR_DEPTH);

    std::shared_ptr<ob::StreamProfile> selectedColor, selectedDepth;
    bool                               hwSupported = false;

    // Try to find profiles that support HW D2C (same FPS)
    for(uint32_t i = 0; i < colorProfiles->getCount() && !hwSupported; i++) {
        auto cp = colorProfiles->getProfile(i);
        for(uint32_t j = 0; j < depthProfiles->getCount(); j++) {
            auto dp = depthProfiles->getProfile(j);
            if(cp->as<ob::VideoStreamProfile>()->getFps() != dp->as<ob::VideoStreamProfile>()->getFps())
                continue;
            if(checkHWD2CSupport(pipe, cp, dp)) {
                selectedColor = cp;
                selectedDepth = dp;
                hwSupported   = true;
                break;
            }
        }
    }

    // If no HW D2C profiles found, just use defaults
    if(!selectedColor)
        selectedColor = colorProfiles->getProfile(0);
    if(!selectedDepth)
        selectedDepth = depthProfiles->getProfile(0);

    auto colorVsp = selectedColor->as<ob::VideoStreamProfile>();
    auto depthVsp = selectedDepth->as<ob::VideoStreamProfile>();
    std::cout << "Color: " << colorVsp->getWidth() << "x" << colorVsp->getHeight() << " @ " << colorVsp->getFps() << "fps" << std::endl;
    std::cout << "Depth: " << depthVsp->getWidth() << "x" << depthVsp->getHeight() << " @ " << depthVsp->getFps() << "fps" << std::endl;
    std::cout << "HW D2C Support: " << (hwSupported ? "YES" : "NO") << std::endl;

    // Build config
    auto config = std::make_shared<ob::Config>();
    config->enableStream(selectedColor);
    config->enableStream(selectedDepth);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    AlignMode currentMode = MODE_SW;

    // SW Align filter
    auto swAlignFilter = std::make_shared<ob::Align>(OB_STREAM_COLOR);

    // Helper to apply mode settings
    auto applyMode = [&]() {
        pipe->stop();
        if(currentMode == MODE_HW) {
            config->setAlignMode(ALIGN_D2C_HW_MODE);
        }
        else {
            config->setAlignMode(ALIGN_DISABLE);
        }
        pipe->start(config);
    };

    // Start with SW mode (no HW align in config)
    config->setAlignMode(ALIGN_DISABLE);
    pipe->start(config);

    ob_smpl::CVWindow win("Align Modes", 1280, 720, ob_smpl::ARRANGE_OVERLAY);
    win.setKeyPrompt("'T': Cycle align mode, '+/-': Transparency, 'Esc': Exit");

    win.setKeyPressedCallback([&](int key) {
        if(key == 't' || key == 'T') {
            // Cycle to next mode
            int next = (static_cast<int>(currentMode) + 1) % MODE_COUNT;
            // Skip HW mode if not supported
            if(next == MODE_HW && !hwSupported)
                next = (next + 1) % MODE_COUNT;
            currentMode = static_cast<AlignMode>(next);

            if(currentMode == MODE_SW) {
                config->setAlignMode(ALIGN_DISABLE);
                pipe->stop();
                pipe->start(config);
                win.addLog(std::string("Align: ") + modeNames[currentMode]);
            }
            else {
                applyMode();
                win.addLog(std::string("Align: ") + modeNames[currentMode]);
            }
        }
    });

    while(win.run()) {
        auto frameSet = pipe->waitForFrameset(100);
        if(!frameSet)
            continue;

        if(currentMode == MODE_SW) {
            // Apply software alignment
            auto aligned = swAlignFilter->process(frameSet);
            if(aligned) {
                win.pushFramesToView(aligned);
            }
        }
        else {
            // HW mode or disabled — just display
            win.pushFramesToView(frameSet);
        }
    }

    pipe->stop();
    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
