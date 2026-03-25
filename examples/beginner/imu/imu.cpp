// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// IMU Viewer — Real-time accelerometer & gyroscope ASCII visualization.
// Enhanced from 07_imu: adds terminal-based bar chart, rate display, and
// simple tilt estimation from accelerometer data.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_types.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <mutex>
#include <chrono>
#include <string>

// ASCII bar: maps value to a fixed-width bar string
static std::string asciiBar(float value, float maxAbsVal, int barWidth = 20) {
    if(maxAbsVal <= 0.0f)
        maxAbsVal = 1.0f;
    float ratio = value / maxAbsVal;
    if(ratio > 1.0f)
        ratio = 1.0f;
    if(ratio < -1.0f)
        ratio = -1.0f;

    int halfWidth = barWidth / 2;
    int filled    = static_cast<int>(std::abs(ratio) * halfWidth);

    std::string bar(barWidth, ' ');
    bar[halfWidth] = '|';  // center marker
    if(ratio >= 0) {
        for(int i = 0; i < filled && (halfWidth + 1 + i) < barWidth; i++)
            bar[halfWidth + 1 + i] = '#';
    }
    else {
        for(int i = 0; i < filled && (halfWidth - 1 - i) >= 0; i++)
            bar[halfWidth - 1 - i] = '#';
    }
    return "[" + bar + "]";
}

int main() try {
    ob::Pipeline pipe;
    auto         device = pipe.getDevice();

    auto accelSensor = device->getSensor(OB_SENSOR_ACCEL);
    auto gyroSensor  = device->getSensor(OB_SENSOR_GYRO);
    if(!accelSensor || !gyroSensor) {
        std::cerr << "Device does not support Accel or Gyro sensor." << std::endl;
        std::cout << "\nPress any key to exit.";
        ob_smpl::waitForKeyPressed();
        return 0;
    }

    auto config = std::make_shared<ob::Config>();
    config->enableAccelStream();
    config->enableGyroStream();
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    pipe.start(config);

    std::cout << "=== IMU Viewer ===" << std::endl;
    std::cout << "Press ESC to exit." << std::endl;
    std::cout << std::endl;

    uint64_t frameCount = 0;
    auto     t0         = std::chrono::steady_clock::now();

    const float ACCEL_MAX = 20.0f;   // m/s^2 (a bit over 2g)
    const float GYRO_MAX  = 10.0f;   // rad/s

    while(true) {
        auto key = ob_smpl::waitForKeyPressed(1);
        if(key == ESC_KEY)
            break;

        auto frameSet = pipe.waitForFrameset();
        if(!frameSet)
            continue;

        auto accelFrame = frameSet->getFrame(OB_FRAME_ACCEL)->as<ob::AccelFrame>();
        auto gyroFrame  = frameSet->getFrame(OB_FRAME_GYRO)->as<ob::GyroFrame>();

        auto av = accelFrame->getValue();
        auto gv = gyroFrame->getValue();

        frameCount++;

        // Display every 10 frames to avoid terminal flicker
        if(frameCount % 10 != 0)
            continue;

        // FPS
        auto  t1  = std::chrono::steady_clock::now();
        float sec = std::chrono::duration<float>(t1 - t0).count();
        float fps = (sec > 0) ? frameCount / sec : 0;

        // Simple tilt estimation from accelerometer
        float pitch = std::atan2(av.x, std::sqrt(av.y * av.y + av.z * av.z)) * 180.0f / 3.14159265f;
        float roll  = std::atan2(av.y, std::sqrt(av.x * av.x + av.z * av.z)) * 180.0f / 3.14159265f;

        // Move cursor up to overwrite previous output (ANSI escape)
        if(frameCount > 10) {
            std::cout << "\033[12A";
        }

        std::cout << std::fixed << std::setprecision(3);
        std::cout << "--- IMU @ " << std::setprecision(1) << fps << " fps ---" << std::setprecision(3) << std::endl;
        std::cout << "  Accel (m/s^2):" << std::endl;
        std::cout << "    X: " << std::setw(9) << av.x << "  " << asciiBar(av.x, ACCEL_MAX) << std::endl;
        std::cout << "    Y: " << std::setw(9) << av.y << "  " << asciiBar(av.y, ACCEL_MAX) << std::endl;
        std::cout << "    Z: " << std::setw(9) << av.z << "  " << asciiBar(av.z, ACCEL_MAX) << std::endl;
        std::cout << "  Gyro (rad/s):" << std::endl;
        std::cout << "    X: " << std::setw(9) << gv.x << "  " << asciiBar(gv.x, GYRO_MAX) << std::endl;
        std::cout << "    Y: " << std::setw(9) << gv.y << "  " << asciiBar(gv.y, GYRO_MAX) << std::endl;
        std::cout << "    Z: " << std::setw(9) << gv.z << "  " << asciiBar(gv.z, GYRO_MAX) << std::endl;
        std::cout << "  Temp: " << std::setprecision(1) << accelFrame->getTemperature() << " C" << std::endl;
        std::cout << "  Tilt  Pitch: " << std::setw(7) << pitch << " deg   Roll: " << std::setw(7) << roll << " deg" << std::endl;
        std::cout << std::string(60, ' ') << std::endl;
    }

    pipe.stop();
    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
