// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Infrared Streams — Display single or dual (left+right) IR streams with options.
// Enhanced from 08_infrared: adds IR stream info, save capability, and emitter toggle.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

int main() try {
    ob::Pipeline pipe;
    auto         device     = pipe.getDevice();
    auto         sensorList = device->getSensorList();

    // Detect available IR sensors
    std::vector<OBSensorType> irSensors;
    for(uint32_t i = 0; i < sensorList->getCount(); i++) {
        auto st = sensorList->getSensorType(i);
        if(st == OB_SENSOR_IR || st == OB_SENSOR_IR_LEFT || st == OB_SENSOR_IR_RIGHT) {
            irSensors.push_back(st);
        }
    }

    if(irSensors.empty()) {
        std::cerr << "No IR sensor found on this device." << std::endl;
        std::cout << "\nPress any key to exit.";
        ob_smpl::waitForKeyPressed();
        return 0;
    }

    // Print detected IR sensors
    std::cout << "=== Infrared Streams ===" << std::endl;
    std::cout << "Detected IR sensors:" << std::endl;
    for(auto &st: irSensors) {
        auto name = ob::TypeHelper::convertOBSensorTypeToString(st);
        std::cout << "  - " << name << std::endl;
    }

    bool hasDual = false;
    for(auto &st: irSensors) {
        if(st == OB_SENSOR_IR_LEFT || st == OB_SENSOR_IR_RIGHT)
            hasDual = true;
    }
    std::cout << (hasDual ? "Stereo IR mode" : "Single IR mode") << std::endl;

    // Enable all IR streams
    auto config = std::make_shared<ob::Config>();
    for(auto &st: irSensors) {
        config->enableVideoStream(st, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    }

    pipe.start(config);

    // Print stream profile info
    auto activeProfiles = config->getEnabledStreamProfileList();
    std::cout << "\nActive IR profiles:" << std::endl;
    for(uint32_t i = 0; i < activeProfiles->getCount(); i++) {
        auto sp = activeProfiles->getProfile(i);
        if(sp->is<ob::VideoStreamProfile>()) {
            auto profile = sp->as<ob::VideoStreamProfile>();
            auto name    = ob::TypeHelper::convertOBStreamTypeToString(profile->getType());
            std::cout << "  " << name << ": " << profile->getWidth() << "x" << profile->getHeight() << " @ " << profile->getFps()
                      << "fps, format=" << ob::TypeHelper::convertOBFormatTypeToString(profile->getFormat()) << std::endl;
        }
    }

    // Check if emitter toggle is supported
    bool emitterSupported = false;
    try {
        emitterSupported = device->isPropertySupported(OB_PROP_LASER_BOOL, OB_PERMISSION_READ_WRITE);
    }
    catch(...) {
    }

    bool emitterOn = true;
    if(emitterSupported) {
        try {
            emitterOn = device->getBoolProperty(OB_PROP_LASER_BOOL);
        }
        catch(...) {
        }
    }

    auto              arrangeMode = hasDual ? ob_smpl::ARRANGE_ONE_ROW : ob_smpl::ARRANGE_SINGLE;
    ob_smpl::CVWindow win("Infrared Streams", 1280, 720, arrangeMode);

    std::string prompt = "'S': Save frame";
    if(emitterSupported)
        prompt += ", 'E': Toggle emitter/laser";
    prompt += ", 'Esc': Exit";
    win.setKeyPrompt(prompt);

    uint64_t frameIdx = 0;
    bool     saveNext = false;

    win.setKeyPressedCallback([&](int key) {
        if(key == 's' || key == 'S') {
            saveNext = true;
        }
        else if((key == 'e' || key == 'E') && emitterSupported) {
            emitterOn = !emitterOn;
            try {
                device->setBoolProperty(OB_PROP_LASER_BOOL, emitterOn);
                win.addLog(std::string("Emitter ") + (emitterOn ? "ON" : "OFF"));
            }
            catch(ob::Error &e) {
                win.addLog(std::string("Emitter toggle failed: ") + e.what());
            }
        }
    });

    while(win.run()) {
        auto frameSet = pipe.waitForFrameset(100);
        if(!frameSet)
            continue;

        win.pushFramesToView(frameSet);
        frameIdx++;

        if(saveNext) {
            saveNext   = false;
            auto count = frameSet->getCount();
            for(uint32_t i = 0; i < count; i++) {
                auto frame = frameSet->getFrameByIndex(i);
                if(!frame)
                    continue;
                auto type = frame->getType();
                if(type != OB_FRAME_IR && type != OB_FRAME_IR_LEFT && type != OB_FRAME_IR_RIGHT)
                    continue;

                auto vf       = frame->as<ob::VideoFrame>();
                auto typeName = ob::TypeHelper::convertOBFrameTypeToString(type);
                auto filename = std::string("ir_") + typeName + "_" + std::to_string(frameIdx) + ".png";

                auto w = vf->getWidth();
                auto h = vf->getHeight();

                // Most IR frames are Y8 or Y16
                cv::Mat mat;
                auto    format = vf->getFormat();
                if(format == OB_FORMAT_Y8) {
                    mat = cv::Mat(h, w, CV_8UC1, vf->getData());
                }
                else if(format == OB_FORMAT_Y16 || format == OB_FORMAT_RLE) {
                    mat = cv::Mat(h, w, CV_16UC1, vf->getData());
                    cv::Mat mat8;
                    mat.convertTo(mat8, CV_8UC1, 255.0 / 4096.0);
                    mat = mat8;
                }
                else {
                    win.addLog("Unsupported IR format for save");
                    continue;
                }

                cv::imwrite(filename, mat);
                win.addLog("Saved: " + filename);
            }
        }
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
