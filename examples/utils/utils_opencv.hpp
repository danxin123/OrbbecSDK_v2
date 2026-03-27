// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#pragma once
#include <libobsensor/ObSensor.hpp>
#include <libobsensor/hpp/Utils.hpp>

#ifdef OB_HAVE_OPENCV
#include <opencv2/opencv.hpp>
#endif

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <map>
#include <functional>
#include <vector>
#include <memory>

#include "utils_types.h"
#include "utils.hpp"

namespace ob_smpl {

// arrange type
typedef enum {
    ARRANGE_SINGLE,      // Only show the first frame
    ARRANGE_ONE_ROW,     // Arrange the frames in the array as a row
    ARRANGE_ONE_COLUMN,  // Arrange the frames in the array as a column
    ARRANGE_GRID,        // Arrange the frames in the array as a grid
    ARRANGE_OVERLAY      // Overlay the first two frames in the array
} ArrangeMode;

#ifdef OB_HAVE_OPENCV
// Render a depth frame with 3D relief lighting effect using Scharr gradients.
// The result is a BGR cv::Mat with colormap + diffuse lighting applied.
// @param depthFrame  The depth frame to render
// @param colormapId  OpenCV colormap id (default: cv::COLORMAP_JET)
// @return BGR cv::Mat with 3D lighting effect
cv::Mat renderDepth3D(std::shared_ptr<const ob::Frame> depthFrame, int colormapId = cv::COLORMAP_JET);
#endif

// Save a video frame to disk as PNG via the SDK's built-in FrameSaveHelper.
// Depth (Y16/Z16) → 16-bit grayscale PNG, IR (Y8/Y16) → grayscale PNG, Color → RGB PNG.
// Format conversion is handled internally by the SDK.
// @param frame  The video frame to save (depth, color, or IR)
// @param path   Output file base path (extension is replaced with .png)
// @return The actual filename written, or "" on failure
std::string saveFrame(std::shared_ptr<const ob::Frame> frame, const std::string &path);

class CVWindow {
public:
    // create a window with the specified name, width and height
    CVWindow(std::string name, uint32_t width = 1280, uint32_t height = 720, ArrangeMode arrangeMode = ARRANGE_SINGLE);
    ~CVWindow() noexcept;

    // run the window loop
    bool run();

    // close window
    void close();

    // clear cached frames and mats
    void reset();

    // add frames to view
    void pushFramesToView(std::vector<std::shared_ptr<const ob::Frame>> frames, int groupId = 0);
    void pushFramesToView(std::shared_ptr<const ob::Frame> currentFrame, int groupId = 0);

    // set show frame info
    void setShowInfo(bool show);

    // set show frame syncTime info
    void setShowSyncTimeInfo(bool show);

    // set alpha, only valid when arrangeMode_ is ARRANGE_OVERLAY
    void setAlpha(float alpha);

    // set the window size
    void resize(int width, int height);

    // set the key pressed callback
    void setKeyPressedCallback(std::function<void(int)> callback);

    // set the key prompt
    void setKeyPrompt(const std::string &prompt);

    // set the log message
    void addLog(const std::string &log);

    // destroyWindow
    void destroyWindow();

private:
    std::string name_;
    ArrangeMode arrangeMode_;
    uint32_t    width_;
    uint32_t    height_;
    bool        closed_;
    bool        showInfo_;
    bool        showSyncTimeInfo_;
    float       alpha_;

    std::string prompt_;
    std::string log_;
    uint64_t    logCreatedTime_;

    std::function<void(int)> keyPressedCallback_;

#ifdef OB_HAVE_OPENCV
    // --- OpenCV-mode private members ---
    void processFrames();
    void arrangeFrames();

    cv::Mat visualize(std::shared_ptr<const ob::Frame> frame);
    void    drawInfo(cv::Mat &imageMat, std::shared_ptr<const ob::VideoFrame> &frame);
    cv::Mat resizeMatKeepAspectRatio(const cv::Mat &mat, int width, int height);

    bool        isWindowDestroyed_;
    bool        showPrompt_;
    uint64_t    winCreatedTime_;

    std::thread                                                  processThread_;
    std::map<int, std::vector<std::shared_ptr<const ob::Frame>>> srcFrameGroups_;
    std::mutex                                                   srcFrameGroupsMtx_;
    std::condition_variable                                      srcFrameGroupsCv_;

    using StreamsMatMap = std::map<int, std::pair<std::shared_ptr<const ob::Frame>, cv::Mat>>;
    StreamsMatMap matGroups_;
    std::mutex    renderMatsMtx_;
    cv::Mat       renderMat_;
#else
    // --- Console-mode private members ---
    std::map<int, std::vector<std::shared_ptr<const ob::Frame>>> lastFrameGroups_;
    std::mutex                                                   framesMtx_;
    uint64_t                                                     lastPrintTime_;
    uint64_t                                                     frameCount_;
    bool                                                         promptPrinted_;

    void printFrameInfo(const std::shared_ptr<const ob::Frame> &frame);
#endif
};

}  // namespace ob_smpl
