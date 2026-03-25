// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

// Camera Params — Print intrinsics, extrinsics, distortion, and demonstrate 2D↔3D transforms.
// Demonstrates: StreamProfile intrinsic/distortion/extrinsic, CoordinateTransformHelper.
// No OpenCV required — pure CLI.

#include <libobsensor/ObSensor.hpp>
#include "libobsensor/hpp/Utils.hpp"

#include "utils.hpp"

#include <iostream>
#include <iomanip>

void printIntrinsic(const char *name, const OBCameraIntrinsic &intr) {
    std::cout << "\n  [" << name << " Intrinsic]" << std::endl;
    std::cout << "    Resolution: " << intr.width << " x " << intr.height << std::endl;
    std::cout << "    fx=" << intr.fx << "  fy=" << intr.fy << std::endl;
    std::cout << "    cx=" << intr.cx << "  cy=" << intr.cy << std::endl;
}

void printDistortion(const char *name, const OBCameraDistortion &dist) {
    std::cout << "\n  [" << name << " Distortion]" << std::endl;
    std::cout << "    k1=" << dist.k1 << "  k2=" << dist.k2 << "  k3=" << dist.k3 << std::endl;
    std::cout << "    k4=" << dist.k4 << "  k5=" << dist.k5 << "  k6=" << dist.k6 << std::endl;
    std::cout << "    p1=" << dist.p1 << "  p2=" << dist.p2 << std::endl;
}

void printExtrinsic(const char *label, const OBExtrinsic &ext) {
    std::cout << "\n  [Extrinsic: " << label << "]" << std::endl;
    std::cout << "    Rotation:" << std::endl;
    for(int r = 0; r < 3; r++) {
        std::cout << "      [";
        for(int c = 0; c < 3; c++) {
            std::cout << std::setw(12) << std::fixed << std::setprecision(6) << ext.rot[r * 3 + c];
        }
        std::cout << " ]" << std::endl;
    }
    std::cout << "    Translation (mm): [" << ext.trans[0] << ", " << ext.trans[1] << ", " << ext.trans[2] << "]" << std::endl;
}

void demo2Dto3D(const OBCameraIntrinsic &depthIntrinsic, const OBExtrinsic &extrinsicD2C, float depthMm) {
    // Use center pixel
    OBPoint2f pixel  = {depthIntrinsic.cx, depthIntrinsic.cy};
    OBPoint3f point  = {};
    bool      result = ob::CoordinateTransformHelper::transformation2dto3d(pixel, depthMm, depthIntrinsic, extrinsicD2C, &point);
    std::cout << "\n  [2D→3D Transform Demo]" << std::endl;
    std::cout << "    Input: pixel(" << pixel.x << ", " << pixel.y << ") depth=" << depthMm << "mm" << std::endl;
    if(result) {
        std::cout << "    Output: 3D(" << point.x << ", " << point.y << ", " << point.z << ") mm" << std::endl;
    }
    else {
        std::cout << "    Transform failed." << std::endl;
    }
}

void demo3Dto2D(const OBCameraIntrinsic &depthIntrinsic, const OBCameraDistortion &depthDistortion, const OBExtrinsic &extrinsicC2D) {
    // A point 1 meter in front of the camera
    OBPoint3f point = {0.0f, 0.0f, 1000.0f};
    OBPoint2f pixel = {};
    bool      result = ob::CoordinateTransformHelper::transformation3dto2d(point, depthIntrinsic, depthDistortion, extrinsicC2D, &pixel);
    std::cout << "\n  [3D→2D Transform Demo]" << std::endl;
    std::cout << "    Input: 3D(" << point.x << ", " << point.y << ", " << point.z << ") mm" << std::endl;
    if(result) {
        std::cout << "    Output: pixel(" << pixel.x << ", " << pixel.y << ")" << std::endl;
    }
    else {
        std::cout << "    Transform failed." << std::endl;
    }
}

int main(void) try {
    // Start pipeline to get active stream profiles
    auto config = std::make_shared<ob::Config>();
    config->enableVideoStream(OB_STREAM_DEPTH, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->enableVideoStream(OB_STREAM_COLOR, OB_WIDTH_ANY, OB_HEIGHT_ANY, OB_FPS_ANY, OB_FORMAT_ANY);
    config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE);

    ob::Pipeline pipe;
    pipe.start(config);

    std::cout << "Waiting for frames to get stream profiles..." << std::endl;

    // Grab a frameset to get active profiles
    std::shared_ptr<ob::FrameSet> frameSet;
    for(int i = 0; i < 30; i++) {
        frameSet = pipe.waitForFrameset(500);
        if(frameSet && frameSet->getFrame(OB_FRAME_DEPTH) && frameSet->getFrame(OB_FRAME_COLOR))
            break;
    }

    if(!frameSet) {
        std::cerr << "Failed to get frames." << std::endl;
        pipe.stop();
        return -1;
    }

    auto depthFrame = frameSet->getFrame(OB_FRAME_DEPTH);
    auto colorFrame = frameSet->getFrame(OB_FRAME_COLOR);

    auto depthProfile = depthFrame->getStreamProfile()->as<ob::VideoStreamProfile>();
    auto colorProfile = colorFrame->getStreamProfile()->as<ob::VideoStreamProfile>();

    std::cout << "\n==============================" << std::endl;
    std::cout << "  Camera Parameters Report" << std::endl;
    std::cout << "==============================" << std::endl;

    // Active stream info
    std::cout << "\n  Depth stream: " << depthProfile->getWidth() << "x" << depthProfile->getHeight() << " @ " << depthProfile->getFps() << "fps" << std::endl;
    std::cout << "  Color stream: " << colorProfile->getWidth() << "x" << colorProfile->getHeight() << " @ " << colorProfile->getFps() << "fps" << std::endl;

    // Intrinsics
    auto depthIntrinsic = depthProfile->getIntrinsic();
    auto colorIntrinsic = colorProfile->getIntrinsic();
    printIntrinsic("Depth", depthIntrinsic);
    printIntrinsic("Color", colorIntrinsic);

    // Distortion
    auto depthDistortion = depthProfile->getDistortion();
    auto colorDistortion = colorProfile->getDistortion();
    printDistortion("Depth", depthDistortion);
    printDistortion("Color", colorDistortion);

    // Extrinsics
    auto extrinsicD2C = depthProfile->getExtrinsicTo(colorProfile);
    auto extrinsicC2D = colorProfile->getExtrinsicTo(depthProfile);
    printExtrinsic("Depth → Color", extrinsicD2C);
    printExtrinsic("Color → Depth", extrinsicC2D);

    // Transform demos
    demo2Dto3D(depthIntrinsic, extrinsicD2C, 1000.0f);
    demo3Dto2D(depthIntrinsic, depthDistortion, extrinsicC2D);

    std::cout << "\n==============================\n" << std::endl;

    pipe.stop();

    std::cout << "Press any key to exit.";
    ob_smpl::waitForKeyPressed();

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
