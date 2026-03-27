// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include "FrameSaveUtil.hpp"
#include "logger/Logger.hpp"

#include <turbojpeg.h>

#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>

namespace libobsensor {

// ---------------------------------------------------------------------------
// CRC32 for PNG (standard CRC-32/ISO 3309)
// ---------------------------------------------------------------------------
static uint32_t crc32Table[256];
static bool     crc32TableReady = false;

static void initCrc32Table() {
    if(crc32TableReady)
        return;
    for(uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for(int j = 0; j < 8; j++) {
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        crc32Table[i] = c;
    }
    crc32TableReady = true;
}

static uint32_t crc32(const uint8_t *data, size_t len) {
    initCrc32Table();
    uint32_t crc = 0xFFFFFFFFu;
    for(size_t i = 0; i < len; i++) {
        crc = crc32Table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}

// ---------------------------------------------------------------------------
// Adler-32 for zlib
// ---------------------------------------------------------------------------
static uint32_t adler32(const uint8_t *data, size_t len) {
    uint32_t a = 1, b = 0;
    for(size_t i = 0; i < len; i++) {
        a = (a + data[i]) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

// ---------------------------------------------------------------------------
// Helper: append big-endian 32-bit integer to buffer
// ---------------------------------------------------------------------------
static void appendBE32(std::vector<uint8_t> &buf, uint32_t val) {
    buf.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>(val & 0xFF));
}

// Write a PNG chunk (length + type + data + crc)
static void writePngChunk(std::ofstream &ofs, const char *type, const uint8_t *data, uint32_t dataLen) {
    uint8_t lenBuf[4] = { static_cast<uint8_t>((dataLen >> 24) & 0xFF), static_cast<uint8_t>((dataLen >> 16) & 0xFF),
                          static_cast<uint8_t>((dataLen >> 8) & 0xFF), static_cast<uint8_t>(dataLen & 0xFF) };
    ofs.write(reinterpret_cast<const char *>(lenBuf), 4);
    ofs.write(type, 4);
    if(dataLen > 0) {
        ofs.write(reinterpret_cast<const char *>(data), dataLen);
    }

    // CRC covers type + data
    std::vector<uint8_t> crcBuf(4 + dataLen);
    std::memcpy(crcBuf.data(), type, 4);
    if(dataLen > 0) {
        std::memcpy(crcBuf.data() + 4, data, dataLen);
    }
    uint32_t crcVal    = crc32(crcBuf.data(), crcBuf.size());
    uint8_t  crcOut[4] = { static_cast<uint8_t>((crcVal >> 24) & 0xFF), static_cast<uint8_t>((crcVal >> 16) & 0xFF),
                           static_cast<uint8_t>((crcVal >> 8) & 0xFF), static_cast<uint8_t>(crcVal & 0xFF) };
    ofs.write(reinterpret_cast<const char *>(crcOut), 4);
}

// Build zlib stored (uncompressed) blocks from raw data
static void buildZlibStored(const std::vector<uint8_t> &rawData, std::vector<uint8_t> &zlibData) {
    // zlib header: CMF=0x78, FLG=0x01
    zlibData.push_back(0x78);
    zlibData.push_back(0x01);

    size_t offset = 0;
    while(offset < rawData.size()) {
        size_t  blockSize = std::min<size_t>(rawData.size() - offset, 65535);
        uint8_t bfinal    = (offset + blockSize >= rawData.size()) ? 1 : 0;
        zlibData.push_back(bfinal);
        uint16_t len  = static_cast<uint16_t>(blockSize);
        uint16_t nlen = static_cast<uint16_t>(~len);
        zlibData.push_back(static_cast<uint8_t>(len & 0xFF));
        zlibData.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
        zlibData.push_back(static_cast<uint8_t>(nlen & 0xFF));
        zlibData.push_back(static_cast<uint8_t>((nlen >> 8) & 0xFF));
        zlibData.insert(zlibData.end(), rawData.begin() + static_cast<std::ptrdiff_t>(offset),
                        rawData.begin() + static_cast<std::ptrdiff_t>(offset + blockSize));
        offset += blockSize;
    }

    // Adler-32 checksum
    uint32_t adlerVal = adler32(rawData.data(), rawData.size());
    appendBE32(zlibData, adlerVal);
}

// ---------------------------------------------------------------------------
// writePng8: 8-bit PNG (grayscale or RGB, uncompressed deflate)
// ---------------------------------------------------------------------------
bool FrameSaveUtil::writePng8(const char *fileName, const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
    uint8_t colorType = (channels == 1) ? 0 : 2;  // 0=grayscale, 2=RGB

    std::ofstream ofs(fileName, std::ios::binary);
    if(!ofs.is_open()) {
        LOG_ERROR("Failed to open file: {}", fileName);
        return false;
    }

    // PNG signature
    const uint8_t sig[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    ofs.write(reinterpret_cast<const char *>(sig), 8);

    // IHDR
    std::vector<uint8_t> ihdr;
    appendBE32(ihdr, width);
    appendBE32(ihdr, height);
    ihdr.push_back(8);          // bit depth
    ihdr.push_back(colorType);
    ihdr.push_back(0);          // compression
    ihdr.push_back(0);          // filter
    ihdr.push_back(0);          // interlace
    writePngChunk(ofs, "IHDR", ihdr.data(), static_cast<uint32_t>(ihdr.size()));

    // Raw image data: filter_byte(0) + row pixels for each row
    uint32_t             rowBytes = width * channels;
    std::vector<uint8_t> rawData(height * (1 + rowBytes));
    for(uint32_t y = 0; y < height; y++) {
        rawData[y * (1 + rowBytes)] = 0;  // filter: None
        std::memcpy(&rawData[y * (1 + rowBytes) + 1], &data[y * rowBytes], rowBytes);
    }

    std::vector<uint8_t> zlibData;
    buildZlibStored(rawData, zlibData);
    writePngChunk(ofs, "IDAT", zlibData.data(), static_cast<uint32_t>(zlibData.size()));

    writePngChunk(ofs, "IEND", nullptr, 0);
    ofs.close();
    return ofs.good();
}

// ---------------------------------------------------------------------------
// writePng16: 16-bit grayscale PNG
// ---------------------------------------------------------------------------
bool FrameSaveUtil::writePng16(const char *fileName, const uint16_t *data, uint32_t width, uint32_t height) {
    std::ofstream ofs(fileName, std::ios::binary);
    if(!ofs.is_open()) {
        LOG_ERROR("Failed to open file: {}", fileName);
        return false;
    }

    const uint8_t sig[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    ofs.write(reinterpret_cast<const char *>(sig), 8);

    std::vector<uint8_t> ihdr;
    appendBE32(ihdr, width);
    appendBE32(ihdr, height);
    ihdr.push_back(16);  // bit depth
    ihdr.push_back(0);   // grayscale
    ihdr.push_back(0);
    ihdr.push_back(0);
    ihdr.push_back(0);
    writePngChunk(ofs, "IHDR", ihdr.data(), static_cast<uint32_t>(ihdr.size()));

    // Row = filter(0) + big-endian 16-bit pixels
    uint32_t             rowBytes = width * 2;
    std::vector<uint8_t> rawData(height * (1 + rowBytes));
    for(uint32_t y = 0; y < height; y++) {
        rawData[y * (1 + rowBytes)] = 0;
        for(uint32_t x = 0; x < width; x++) {
            uint16_t val                                 = data[y * width + x];
            rawData[y * (1 + rowBytes) + 1 + x * 2]     = static_cast<uint8_t>((val >> 8) & 0xFF);
            rawData[y * (1 + rowBytes) + 1 + x * 2 + 1] = static_cast<uint8_t>(val & 0xFF);
        }
    }

    std::vector<uint8_t> zlibData;
    buildZlibStored(rawData, zlibData);
    writePngChunk(ofs, "IDAT", zlibData.data(), static_cast<uint32_t>(zlibData.size()));

    writePngChunk(ofs, "IEND", nullptr, 0);
    ofs.close();
    return ofs.good();
}

// ---------------------------------------------------------------------------
// writeJpeg: JPEG via libjpeg-turbo
// ---------------------------------------------------------------------------
bool FrameSaveUtil::writeJpeg(const char *fileName, const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels, int quality) {
    tjhandle handle = tjInitCompress();
    if(!handle) {
        LOG_ERROR("Failed to init JPEG compressor");
        return false;
    }

    int pixelFormat = (channels == 1) ? TJPF_GRAY : TJPF_RGB;
    int subsamp     = (channels == 1) ? TJSAMP_GRAY : TJSAMP_444;

    unsigned char *jpegBuf  = nullptr;
    unsigned long  jpegSize = 0;

    int ret = tjCompress2(handle, data, static_cast<int>(width), 0, static_cast<int>(height), pixelFormat, &jpegBuf, &jpegSize, subsamp, quality,
                          TJFLAG_FASTDCT);
    if(ret != 0) {
        LOG_ERROR("JPEG compression failed: {}", tjGetErrorStr2(handle));
        tjDestroy(handle);
        return false;
    }

    std::ofstream ofs(fileName, std::ios::binary);
    if(!ofs.is_open()) {
        LOG_ERROR("Failed to open file: {}", fileName);
        tjFree(jpegBuf);
        tjDestroy(handle);
        return false;
    }

    ofs.write(reinterpret_cast<const char *>(jpegBuf), static_cast<std::streamsize>(jpegSize));
    ofs.close();

    tjFree(jpegBuf);
    tjDestroy(handle);
    return ofs.good();
}

// ---------------------------------------------------------------------------
// convertToRgb: convert color frame to RGB pixel buffer
// ---------------------------------------------------------------------------
bool FrameSaveUtil::convertToRgb(std::shared_ptr<const VideoFrame> frame, std::vector<uint8_t> &rgbOut, uint32_t &outWidth, uint32_t &outHeight) {
    auto format = frame->getFormat();
    outWidth    = frame->getWidth();
    outHeight   = frame->getHeight();
    uint32_t pixelCount = outWidth * outHeight;

    if(format == OB_FORMAT_RGB) {
        rgbOut.assign(frame->getData(), frame->getData() + pixelCount * 3);
        return true;
    }

    // MJPG → RGB via turbojpeg
    if(format == OB_FORMAT_MJPG) {
        tjhandle handle = tjInitDecompress();
        if(!handle)
            return false;

        int            jpegW = 0, jpegH = 0, jpegSub = 0, jpegCS = 0;
        const uint8_t *jpegData = frame->getData();
        size_t         jpegSize = frame->getDataSize();

        if(tjDecompressHeader3(handle, jpegData, static_cast<unsigned long>(jpegSize), &jpegW, &jpegH, &jpegSub, &jpegCS) != 0) {
            tjDestroy(handle);
            return false;
        }

        outWidth  = static_cast<uint32_t>(jpegW);
        outHeight = static_cast<uint32_t>(jpegH);
        rgbOut.resize(outWidth * outHeight * 3);

        if(tjDecompress2(handle, jpegData, static_cast<unsigned long>(jpegSize), rgbOut.data(), jpegW, 0, jpegH, TJPF_RGB, TJFLAG_FASTDCT) != 0) {
            tjDestroy(handle);
            return false;
        }
        tjDestroy(handle);
        return true;
    }

    // BGR → RGB
    if(format == OB_FORMAT_BGR) {
        rgbOut.resize(pixelCount * 3);
        const uint8_t *src = frame->getData();
        for(uint32_t i = 0; i < pixelCount; i++) {
            rgbOut[i * 3]     = src[i * 3 + 2];
            rgbOut[i * 3 + 1] = src[i * 3 + 1];
            rgbOut[i * 3 + 2] = src[i * 3];
        }
        return true;
    }

    // YUYV/YUY2 → RGB
    if(format == OB_FORMAT_YUYV || format == OB_FORMAT_YUY2) {
        rgbOut.resize(pixelCount * 3);
        const uint8_t *src = frame->getData();
        auto clamp = [](int v) -> uint8_t { return static_cast<uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v)); };
        for(uint32_t i = 0; i < pixelCount / 2; i++) {
            int y0 = src[i * 4], u = src[i * 4 + 1] - 128, y1 = src[i * 4 + 2], v = src[i * 4 + 3] - 128;
            rgbOut[i * 6]     = clamp(y0 + ((359 * v) >> 8));
            rgbOut[i * 6 + 1] = clamp(y0 - ((88 * u + 183 * v) >> 8));
            rgbOut[i * 6 + 2] = clamp(y0 + ((454 * u) >> 8));
            rgbOut[i * 6 + 3] = clamp(y1 + ((359 * v) >> 8));
            rgbOut[i * 6 + 4] = clamp(y1 - ((88 * u + 183 * v) >> 8));
            rgbOut[i * 6 + 5] = clamp(y1 + ((454 * u) >> 8));
        }
        return true;
    }

    // UYVY → RGB
    if(format == OB_FORMAT_UYVY) {
        rgbOut.resize(pixelCount * 3);
        const uint8_t *src = frame->getData();
        auto clamp = [](int v) -> uint8_t { return static_cast<uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v)); };
        for(uint32_t i = 0; i < pixelCount / 2; i++) {
            int u = src[i * 4] - 128, y0 = src[i * 4 + 1], v = src[i * 4 + 2] - 128, y1 = src[i * 4 + 3];
            rgbOut[i * 6]     = clamp(y0 + ((359 * v) >> 8));
            rgbOut[i * 6 + 1] = clamp(y0 - ((88 * u + 183 * v) >> 8));
            rgbOut[i * 6 + 2] = clamp(y0 + ((454 * u) >> 8));
            rgbOut[i * 6 + 3] = clamp(y1 + ((359 * v) >> 8));
            rgbOut[i * 6 + 4] = clamp(y1 - ((88 * u + 183 * v) >> 8));
            rgbOut[i * 6 + 5] = clamp(y1 + ((454 * u) >> 8));
        }
        return true;
    }

    return false;
}

// ---------------------------------------------------------------------------
// saveFrameToPng
// ---------------------------------------------------------------------------
bool FrameSaveUtil::saveFrameToPng(const char *fileName, std::shared_ptr<Frame> frame) {
    if(!frame || !frame->is<VideoFrame>()) {
        LOG_ERROR("saveFrameToPng: invalid frame");
        return false;
    }

    auto videoFrame = frame->as<VideoFrame>();
    auto format     = videoFrame->getFormat();
    auto type       = frame->getType();

    // Depth: 16-bit grayscale
    if(type == OB_FRAME_DEPTH && (format == OB_FORMAT_Y16 || format == OB_FORMAT_Z16)) {
        return writePng16(fileName, reinterpret_cast<const uint16_t *>(videoFrame->getData()), videoFrame->getWidth(), videoFrame->getHeight());
    }

    // IR: Y8 → 8-bit grayscale, Y16 → 16-bit grayscale
    if(type == OB_FRAME_IR || type == OB_FRAME_IR_LEFT || type == OB_FRAME_IR_RIGHT) {
        if(format == OB_FORMAT_Y8) {
            return writePng8(fileName, videoFrame->getData(), videoFrame->getWidth(), videoFrame->getHeight(), 1);
        }
        if(format == OB_FORMAT_Y16) {
            return writePng16(fileName, reinterpret_cast<const uint16_t *>(videoFrame->getData()), videoFrame->getWidth(), videoFrame->getHeight());
        }
    }

    // Color: convert to RGB → 8-bit RGB PNG
    if(type == OB_FRAME_COLOR) {
        std::vector<uint8_t> rgbData;
        uint32_t             w, h;
        if(!convertToRgb(videoFrame, rgbData, w, h)) {
            LOG_ERROR("saveFrameToPng: unsupported color format");
            return false;
        }
        return writePng8(fileName, rgbData.data(), w, h, 3);
    }

    LOG_ERROR("saveFrameToPng: unsupported frame type or format");
    return false;
}

// ---------------------------------------------------------------------------
// saveFrameToJpeg
// ---------------------------------------------------------------------------
bool FrameSaveUtil::saveFrameToJpeg(const char *fileName, std::shared_ptr<Frame> frame, int quality) {
    if(!frame || !frame->is<VideoFrame>()) {
        LOG_ERROR("saveFrameToJpeg: invalid frame");
        return false;
    }

    auto videoFrame = frame->as<VideoFrame>();
    auto format     = videoFrame->getFormat();
    auto type       = frame->getType();

    // IR Y8 → grayscale JPEG
    if((type == OB_FRAME_IR || type == OB_FRAME_IR_LEFT || type == OB_FRAME_IR_RIGHT) && format == OB_FORMAT_Y8) {
        return writeJpeg(fileName, videoFrame->getData(), videoFrame->getWidth(), videoFrame->getHeight(), 1, quality);
    }

    // Color → RGB JPEG
    if(type == OB_FRAME_COLOR) {
        std::vector<uint8_t> rgbData;
        uint32_t             w, h;
        if(!convertToRgb(videoFrame, rgbData, w, h)) {
            LOG_ERROR("saveFrameToJpeg: unsupported color format");
            return false;
        }
        return writeJpeg(fileName, rgbData.data(), w, h, 3, quality);
    }

    LOG_ERROR("saveFrameToJpeg: unsupported frame type (use PNG for 16-bit depth)");
    return false;
}

}  // namespace libobsensor
