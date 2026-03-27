// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#pragma once
#include "frame/Frame.hpp"

#include <string>
#include <vector>

namespace libobsensor {

class FrameSaveUtil {
public:
    // Save a video frame as PNG.
    // Depth/IR Y16 → 16-bit grayscale PNG, IR Y8 → 8-bit grayscale PNG, Color → 8-bit RGB PNG.
    // Format conversion (MJPG/UYVY/YUYV/BGR → RGB) is handled internally.
    static bool saveFrameToPng(const char *fileName, std::shared_ptr<Frame> frame);

    // Save a video frame as JPEG.
    // Color → RGB JPEG, IR Y8 → grayscale JPEG.
    // Format conversion is handled internally.
    static bool saveFrameToJpeg(const char *fileName, std::shared_ptr<Frame> frame, int quality = 95);

private:
    // Minimal PNG encoder (no external dependency).
    // Writes an uncompressed PNG file using stored deflate blocks.
    static bool writePng8(const char *fileName, const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels);
    static bool writePng16(const char *fileName, const uint16_t *data, uint32_t width, uint32_t height);

    // JPEG encoder via libjpeg-turbo (already linked in filter module).
    static bool writeJpeg(const char *fileName, const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels, int quality);

    // Convert a color frame to RGB pixel data.
    // Returns false if conversion is not supported.
    static bool convertToRgb(std::shared_ptr<const VideoFrame> frame, std::vector<uint8_t> &rgbOut, uint32_t &outWidth, uint32_t &outHeight);
};

}  // namespace libobsensor
