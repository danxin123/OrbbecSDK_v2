# HDR

This sample demonstrates HDR depth merging on devices that support HDR-related configuration.
It shows the original depth and infrared frames together with the HDR-merged depth result.

## When To Use It

- evaluate HDR depth behavior on supported devices
- compare original input frames with merged HDR depth output
- understand how the SDK HDR merge filter is used in a real sample

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- OpenCV is required for the display window
- The device must support HDR merge

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_hdr
```

```bash
.\build\win_x64\bin\ob_hdr.exe     # Windows
./build/linux_x86_64/bin/ob_hdr    # Linux x86_64
./build/linux_arm64/bin/ob_hdr     # Linux ARM64
./build/macOS/bin/ob_hdr           # macOS
```

## Operation

- The sample automatically configures HDR or frame-interleave-based HDR depending on device capability.
- The last row of the window shows the HDR-merged depth result.
- Press `Esc` to exit.

## Notes

- This sample is intended for devices that support HDR-related features. The original README notes Gemini 330 series support.
- If the device does not support HDR merge, the sample exits early with a message.

## Result

![hdr](../../../docs/resource/hdr.jpg)
