// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "utils.hpp"
#include "utils_c.h"

#include <chrono>
#include <cstdlib>

namespace ob_smpl {
char waitForKeyPressed(uint32_t timeout_ms) {
    return ob_smpl_wait_for_key_press(timeout_ms);
}

char pollTestAutomationKey() {
    return ob_smpl_test_poll_auto_key();
}

bool testModeEnabled() {
    return ob_smpl_test_mode_enabled() != 0;
}

uint64_t getNowTimesMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int getInputOption() {
    char inputOption = ob_smpl::waitForKeyPressed();
    if(inputOption == ESC_KEY) {
        return -1;
    }
    return inputOption - '0';
}

std::string resolveSaveOutputPath(const std::string &path) {
    const char *root = std::getenv("OB_EXAMPLE_TEST_OUTPUT_DIR");
    if(root == nullptr || root[0] == '\0') {
        return path;
    }

    std::string baseName = path;
    const auto  slashPos = baseName.find_last_of("\\/");
    if(slashPos != std::string::npos) {
        baseName = baseName.substr(slashPos + 1);
    }

    std::string outputRoot(root);
    if(outputRoot.empty()) {
        return baseName;
    }

    const char tail = outputRoot[outputRoot.size() - 1];
    if(tail == '/' || tail == '\\') {
        return outputRoot + baseName;
    }

#ifdef _WIN32
    return outputRoot + "\\" + baseName;
#else
    return outputRoot + "/" + baseName;
#endif
}

bool isLiDARDevice(std::shared_ptr<ob::Device> device) {
    std::shared_ptr<ob::SensorList> sensorList = device->getSensorList();

    for(uint32_t index = 0; index < sensorList->getCount(); index++) {
        OBSensorType sensorType = sensorList->getSensorType(index);
        if(sensorType == OB_SENSOR_LIDAR) {
            return true;
        }
    }

    return false;
}

bool supportAnsiEscape() {
    if(ob_smpl_support_ansi_escape() == 0) {
        return false;
    }
    return true;
}

bool isGemini305Device(int vid, int pid) {
    return ob_smpl_is_gemini305_device(vid, pid);
}

bool isAstraMiniDevice(int vid, int pid) {
    return ob_smpl_is_astra_mini_device(vid, pid);
}

}  // namespace ob_smpl
