# LiDAR Stream with C

This is the C API version of the LiDAR and IMU streaming sample.
It provides interactive sensor selection and prints live LiDAR and IMU data in the terminal.

## When To Use It

- develop against the C API instead of the C++ API
- inspect LiDAR, accelerometer, and gyroscope streams from a C program
- use a callback-based LiDAR streaming reference in C

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_stream_c
```

```bash
.\build\win_x64\bin\ob_lidar_stream_c.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_stream_c    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_stream_c     # Linux ARM64
./build/macOS/bin/ob_lidar_stream_c           # macOS
```

## How To Use It

1. Select a device if more than one LiDAR is connected.
2. Select the sensors you want to enable.
3. Choose a stream profile for each enabled sensor.
4. Watch the terminal output for point-cloud information and IMU values.
5. Press `Esc` to exit.

## Notes

- This sample is functionally similar to the C++ `lidar_stream` example, but uses explicit C-style object management and error handling.
