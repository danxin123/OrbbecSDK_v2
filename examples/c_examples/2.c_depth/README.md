# Depth with C

This C API sample enables the depth stream only and prints the distance of the center pixel.

## When To Use It

- test depth streaming through the C API
- read raw depth values and convert them to millimeters
- use a simple C reference for depth-only applications

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- No OpenCV dependency is required

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_depth_c
```

```bash
.\build\win_x64\bin\ob_depth_c.exe     # Windows
./build/linux_x86_64/bin/ob_depth_c    # Linux x86_64
./build/linux_arm64/bin/ob_depth_c     # Linux ARM64
./build/macOS/bin/ob_depth_c           # macOS
```

## What The Sample Does

1. Creates a pipeline and config object
2. Enables the depth stream
3. Waits for depth frames in a loop
4. Reads the center pixel from the raw depth buffer
5. Converts the raw value to millimeters with the frame scale
6. Prints the measured distance in the terminal
7. Exits when you press `Esc`

## Result

![Quick_Start_C](../../../docs/resource/quick_start_c.jpg)
