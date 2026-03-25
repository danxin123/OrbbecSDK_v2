// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Quick Start — Zero external dependency, pure CLI example.
// Demonstrates: Pipeline creation, frame retrieval, reading depth values,
//               SDK log configuration (severity, file, console, callback).

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"

#include <iostream>
#include <iomanip>
#include <mutex>

// --------------------------------------------------------------------------
// Log callback — receives SDK internal log messages in real-time.
// --------------------------------------------------------------------------
static std::mutex logMutex;

static void onLogMessage(OBLogSeverity severity, const char *logMsg) {
    static const char *sevNames[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
    const char *sev = (severity >= 0 && severity <= OB_LOG_SEVERITY_FATAL) ? sevNames[severity] : "???";

    std::lock_guard<std::mutex> lock(logMutex);
    std::cerr << "[SDK " << sev << "] " << logMsg << std::endl;
}

// --------------------------------------------------------------------------
// Configure SDK logging before creating any device / pipeline.
// --------------------------------------------------------------------------
static void setupLogging() {
    // 1. Set global log severity (DEBUG shows everything).
    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_INFO);

    // 2. Console output — show WARN and above to avoid flooding stdout.
    ob::Context::setLoggerToConsole(OB_LOG_SEVERITY_WARN);

    // 3. File output — write INFO and above to ./Log/ directory.
    ob::Context::setLoggerToFile(OB_LOG_SEVERITY_INFO, ".");

    // 4. Callback output — receive WARN and above in our own handler.
    ob::Context::setLoggerToCallback(OB_LOG_SEVERITY_WARN, onLogMessage);

    std::cout << "[Log] SDK logging configured:" << std::endl;
    std::cout << "      Console >= WARN | File >= INFO (./Log/) | Callback >= WARN" << std::endl;
}

int main(void) try {
    // Configure SDK logging first.
    setupLogging();

    // Create a pipeline with default device.
    ob::Pipeline pipe;

    // Start the pipeline with default config.
    // The default config will enable depth and color streams automatically.
    pipe.start();

    std::cout << "Pipeline started. Press 'ESC' to exit, 'L' to cycle log level." << std::endl;
    std::cout << "Streaming depth frames — printing center pixel depth value..." << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    uint64_t frameCount = 0;

    // Log severity levels to cycle through with 'L' key.
    static const OBLogSeverity levels[] = {
        OB_LOG_SEVERITY_DEBUG, OB_LOG_SEVERITY_INFO, OB_LOG_SEVERITY_WARN,
        OB_LOG_SEVERITY_ERROR, OB_LOG_SEVERITY_OFF
    };
    static const char *levelNames[] = { "DEBUG", "INFO", "WARN", "ERROR", "OFF" };
    int currentLevel = 1; // start at INFO

    while(true) {
        // Wait for a frameset from the pipeline (timeout 1000ms).
        auto frameSet = pipe.waitForFrameset(1000);
        if(!frameSet) {
            continue;
        }

        // Get the depth frame from the frameset.
        auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH);
        if(!depthFrame) {
            continue;
        }

        auto depth = depthFrame->as<ob::DepthFrame>();
        uint32_t width  = depth->getWidth();
        uint32_t height = depth->getHeight();
        float    scale  = depth->getValueScale();

        // Read the center pixel depth value.
        uint16_t *data       = reinterpret_cast<uint16_t *>(depth->getData());
        uint32_t  centerX    = width / 2;
        uint32_t  centerY    = height / 2;
        uint16_t  rawValue   = data[centerY * width + centerX];
        float     depthInMm  = rawValue * scale;

        // Print every 15 frames to avoid flooding the terminal.
        if(frameCount % 15 == 0) {
            std::cout << "Frame #" << std::setw(6) << depth->getIndex()
                      << " | " << width << "x" << height
                      << " | Center depth: " << std::fixed << std::setprecision(1) << depthInMm << " mm"
                      << " | Timestamp: " << depth->getTimeStampUs() / 1000 << " ms"
                      << std::endl;
        }
        frameCount++;

        // Non-blocking key check: exit on ESC, cycle log level on 'L'.
        char key = ob_smpl::waitForKeyPressed(1);
        if(key == ESC_KEY) {
            break;
        }
        if(key == 'l' || key == 'L') {
            currentLevel = (currentLevel + 1) % 5;
            ob::Context::setLoggerSeverity(levels[currentLevel]);
            std::cout << "[Log] Global severity changed to: " << levelNames[currentLevel] << std::endl;
        }
    }

    // Stop the pipeline.
    pipe.stop();
    std::cout << "\nPipeline stopped. Total frames received: " << frameCount << std::endl;

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
