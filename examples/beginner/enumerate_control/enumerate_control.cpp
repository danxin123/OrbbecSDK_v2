// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Enumerate & Control — Browse devices, sensors, stream profiles, and get/set device properties.
// Demonstrates: Context, Device, Sensor, StreamProfile enumeration, Property get/set.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

// Forward declarations
void enumerateStreamProfiles(std::shared_ptr<ob::Sensor> sensor);
void enumerateSensors(std::shared_ptr<ob::Device> device);
void controlProperties(std::shared_ptr<ob::Device> device);

// ============================================================
// Stream profile printing
// ============================================================
void printVideoProfile(std::shared_ptr<ob::StreamProfile> profile, uint32_t index) {
    auto vp     = profile->as<ob::VideoStreamProfile>();
    auto format = ob::TypeHelper::convertOBFormatTypeToString(profile->getFormat());
    std::cout << "  " << index << ". " << format << " " << vp->getWidth() << "x" << vp->getHeight() << " @ " << vp->getFps() << "fps" << std::endl;
}

void printAccelProfile(std::shared_ptr<ob::StreamProfile> profile, uint32_t index) {
    auto ap   = profile->as<ob::AccelStreamProfile>();
    auto rate = ob::TypeHelper::convertOBIMUSampleRateTypeToString(ap->getSampleRate());
    std::cout << "  " << index << ". accel rate: " << rate << std::endl;
}

void printGyroProfile(std::shared_ptr<ob::StreamProfile> profile, uint32_t index) {
    auto gp   = profile->as<ob::GyroStreamProfile>();
    auto rate = ob::TypeHelper::convertOBIMUSampleRateTypeToString(gp->getSampleRate());
    std::cout << "  " << index << ". gyro rate: " << rate << std::endl;
}

void enumerateStreamProfiles(std::shared_ptr<ob::Sensor> sensor) {
    auto profileList = sensor->getStreamProfileList();
    auto sensorType  = sensor->getType();
    std::cout << "\n  Stream profiles (" << profileList->getCount() << "):" << std::endl;
    for(uint32_t i = 0; i < profileList->getCount(); i++) {
        auto profile = profileList->getProfile(i);
        if(ob_is_video_sensor_type(sensorType))
            printVideoProfile(profile, i);
        else if(sensorType == OB_SENSOR_ACCEL)
            printAccelProfile(profile, i);
        else if(sensorType == OB_SENSOR_GYRO)
            printGyroProfile(profile, i);
    }
}

// ============================================================
// Sensor enumeration
// ============================================================
void enumerateSensors(std::shared_ptr<ob::Device> device) {
    auto sensorList = device->getSensorList();
    while(true) {
        std::cout << "\nSensor list:" << std::endl;
        for(uint32_t i = 0; i < sensorList->getCount(); i++) {
            auto type = sensorList->getSensorType(i);
            std::cout << "  " << i << ". " << ob::TypeHelper::convertOBSensorTypeToString(type) << std::endl;
        }
        std::cout << "\nSelect sensor index (or 'b' to go back): ";
        std::string input;
        std::getline(std::cin, input);
        if(input == "b" || input == "B")
            break;

        int idx = std::atoi(input.c_str());
        if(idx < 0 || idx >= static_cast<int>(sensorList->getCount())) {
            std::cout << "Invalid selection." << std::endl;
            continue;
        }
        auto sensor = sensorList->getSensor(idx);
        enumerateStreamProfiles(sensor);
    }
}

// ============================================================
// Property control
// ============================================================
std::string permissionToString(OBPermissionType perm) {
    switch(perm) {
    case OB_PERMISSION_READ:
        return "R/_";
    case OB_PERMISSION_WRITE:
        return "_/W";
    case OB_PERMISSION_READ_WRITE:
        return "R/W";
    default:
        return "_/_";
    }
}

bool isPrimaryProperty(OBPropertyItem item) {
    return (item.type == OB_INT_PROPERTY || item.type == OB_FLOAT_PROPERTY || item.type == OB_BOOL_PROPERTY) && item.permission != OB_PERMISSION_DENY;
}

void printPropertyList(std::shared_ptr<ob::Device> device, const std::vector<OBPropertyItem> &props) {
    std::cout << "\n--- Properties (" << props.size() << ") ---" << std::endl;
    for(size_t i = 0; i < props.size(); i++) {
        auto       &p = props[i];
        std::string range;
        if(p.type == OB_BOOL_PROPERTY) {
            range = "Bool(0/1)";
        }
        else if(p.type == OB_INT_PROPERTY) {
            try {
                auto r = device->getIntPropertyRange(p.id);
                range  = "Int(" + std::to_string(r.min) + "~" + std::to_string(r.max) + " step:" + std::to_string(r.step) + ")";
            }
            catch(...) {
                range = "Int(range N/A)";
            }
        }
        else if(p.type == OB_FLOAT_PROPERTY) {
            try {
                auto r = device->getFloatPropertyRange(p.id);
                range  = "Float(" + std::to_string(r.min) + "~" + std::to_string(r.max) + ")";
            }
            catch(...) {
                range = "Float(range N/A)";
            }
        }
        std::cout << std::setw(3) << i << ". " << p.name << " [" << permissionToString(p.permission) << "] " << range << std::endl;
    }
    std::cout << "---\n";
}

void getPropertyValue(std::shared_ptr<ob::Device> device, OBPropertyItem item) {
    try {
        if(item.type == OB_BOOL_PROPERTY)
            std::cout << "  " << item.name << " = " << device->getBoolProperty(item.id) << std::endl;
        else if(item.type == OB_INT_PROPERTY)
            std::cout << "  " << item.name << " = " << device->getIntProperty(item.id) << std::endl;
        else if(item.type == OB_FLOAT_PROPERTY)
            std::cout << "  " << item.name << " = " << device->getFloatProperty(item.id) << std::endl;
    }
    catch(ob::Error &e) {
        std::cout << "  Get failed: " << e.what() << std::endl;
    }
}

void setPropertyValue(std::shared_ptr<ob::Device> device, OBPropertyItem item, const std::string &strValue) {
    try {
        if(item.type == OB_BOOL_PROPERTY) {
            device->setBoolProperty(item.id, std::atoi(strValue.c_str()));
            std::cout << "  Set " << item.name << " = " << strValue << std::endl;
        }
        else if(item.type == OB_INT_PROPERTY) {
            device->setIntProperty(item.id, std::atoi(strValue.c_str()));
            std::cout << "  Set " << item.name << " = " << strValue << std::endl;
        }
        else if(item.type == OB_FLOAT_PROPERTY) {
            device->setFloatProperty(item.id, static_cast<float>(std::atof(strValue.c_str())));
            std::cout << "  Set " << item.name << " = " << strValue << std::endl;
        }
    }
    catch(ob::Error &e) {
        std::cout << "  Set failed: " << e.what() << std::endl;
    }
}

void controlProperties(std::shared_ptr<ob::Device> device) {
    std::vector<OBPropertyItem> props;
    uint32_t                    count = device->getSupportedPropertyCount();
    for(uint32_t i = 0; i < count; i++) {
        auto item = device->getSupportedProperty(i);
        if(isPrimaryProperty(item))
            props.push_back(item);
    }
    std::sort(props.begin(), props.end(), [](const OBPropertyItem &a, const OBPropertyItem &b) { return a.id < b.id; });

    printPropertyList(device, props);

    std::cout << "Usage: <index> get | <index> set <value> | '?' to list | 'b' to go back\n";

    while(true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        if(line == "b" || line == "B")
            break;
        if(line == "?") {
            printPropertyList(device, props);
            continue;
        }

        std::istringstream       ss(line);
        std::string              tmp;
        std::vector<std::string> tokens;
        while(ss >> tmp)
            tokens.push_back(tmp);

        if(tokens.size() < 2) {
            std::cout << "Usage: <index> get | <index> set <value>\n";
            continue;
        }

        int idx = std::atoi(tokens[0].c_str());
        if(idx < 0 || idx >= static_cast<int>(props.size())) {
            std::cout << "Index out of range.\n";
            continue;
        }

        if(tokens[1] == "get") {
            getPropertyValue(device, props[idx]);
        }
        else if(tokens[1] == "set" && tokens.size() >= 3) {
            setPropertyValue(device, props[idx], tokens[2]);
        }
        else {
            std::cout << "Usage: <index> get | <index> set <value>\n";
        }
    }
}

// ============================================================
// Main — top-level device selection menu
// ============================================================
int main(void) try {
    ob::Context context;

    while(true) {
        auto deviceList = context.queryDeviceList();
        if(deviceList->getCount() < 1) {
            std::cout << "No device found! Please connect a supported device." << std::endl;
            std::cout << "\nPress any key to exit.";
            ob_smpl::waitForKeyPressed();
            return -1;
        }

        std::cout << "\n=== Enumerate & Control ===" << std::endl;
        for(uint32_t i = 0; i < deviceList->getCount(); i++) {
            auto dev  = deviceList->getDevice(i);
            auto info = dev->getDeviceInfo();
            std::cout << "  " << i << ". " << info->getName() << " (PID: 0x" << std::hex << std::setw(4) << std::setfill('0') << info->getPid() << std::dec
                      << ", SN: " << info->getSerialNumber() << ", " << info->getConnectionType() << ")" << std::endl;
        }

        std::cout << "\nSelect device index, or 'q' to quit: ";
        std::string input;
        std::getline(std::cin, input);
        if(input == "q" || input == "Q")
            break;

        int devIdx = std::atoi(input.c_str());
        if(devIdx < 0 || devIdx >= static_cast<int>(deviceList->getCount())) {
            std::cout << "Invalid selection.\n";
            continue;
        }

        auto device = deviceList->getDevice(devIdx);
        auto info   = device->getDeviceInfo();
        std::cout << "\nDevice: " << info->getName() << " (SN: " << info->getSerialNumber() << ")" << std::endl;

        while(true) {
            std::cout << "\n  1. Enumerate sensors & stream profiles" << std::endl;
            std::cout << "  2. Get/Set device properties" << std::endl;
            std::cout << "  b. Back to device selection" << std::endl;
            std::cout << "  Choice: ";

            std::string choice;
            std::getline(std::cin, choice);
            if(choice == "b" || choice == "B")
                break;
            if(choice == "1")
                enumerateSensors(device);
            else if(choice == "2")
                controlProperties(device);
            else
                std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
