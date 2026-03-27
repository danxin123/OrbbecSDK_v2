# Save To Disk

## Overview

This sample demonstrates how to configure a pipeline to capture synchronized color and depth frames and save the first 4 valid frame pairs to disk as PNG files. The program discards several startup frames to ensure stable capture. PNG saving is handled internally by the SDK through `FrameSaveHelper`, so the sample does not rely on OpenCV-based image conversion or `imwrite`.

## When To Use It

- save a small set of RGB-D frames for quick inspection
- verify depth and color file output without building a full viewer
- generate timestamped PNG pairs for later analysis

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- A device that provides both color and depth streams must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_save_to_disk
```

```bash
.\build\win_x64\bin\ob_save_to_disk.exe     # Windows
./build/linux_x86_64/bin/ob_save_to_disk    # Linux x86_64
./build/linux_arm64/bin/ob_save_to_disk     # Linux ARM64
./build/macOS/bin/ob_save_to_disk           # macOS
```

## What It Does

1. Enables the color and depth streams and requires complete framesets.
2. Drops the first 15 frames to avoid unstable startup output.
3. Saves the next 4 valid depth and color frame pairs as PNG files.
4. Stops the pipeline automatically after saving.
5. Waits for a key press before the program exits.

## Output

- depth PNG files named like `Depth_<width>x<height>_<index>_<timestamp>ms.png`
- color PNG files named like `Color_<width>x<height>_<index>_<timestamp>ms.png`

## Notes

- The sample saves files through the SDK helper `ob::FrameSaveHelper::saveFrameToPng(...)`.
- It runs as a terminal sample and does not open a preview window.
- If no complete frameset arrives within 100 ms, it prints a retry message and continues waiting.

## Related Examples

- [../../beginner/rgbd_viewer/README.md](../../beginner/rgbd_viewer/README.md) - preview live color and depth streams
- [../../beginner/record_playback/README.md](../../beginner/record_playback/README.md) - record and replay stream data instead of saving a few PNG frames
