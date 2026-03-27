# LiDAR Stream

This sample streams live LiDAR and IMU data and lets you select sensors and stream profiles at runtime.
It is the best starting point when you want to understand what a connected LiDAR device can output.

## When To Use It

- inspect LiDAR, accelerometer, and gyroscope streams
- choose stream profiles interactively
- study frame formats, timestamps, and valid point counts
- use the sample as a reference for your own LiDAR application

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_stream
```

```bash
.\build\win_x64\bin\ob_lidar_stream.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_stream    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_stream     # Linux ARM64
./build/macOS/bin/ob_lidar_stream           # macOS
```

## How To Use It

1. Select a device if more than one LiDAR is connected.
2. Choose which sensors to enable:
   - `LiDAR`
   - `Accel`
   - `Gyro`
   - or all sensors
3. Select a stream profile for each enabled sensor.
4. Watch the terminal output for frame index, timestamp, format, valid point count, and IMU values.
5. Press `Esc` to exit.

## What You Will See

- LiDAR point-cloud frame information
- accelerometer and gyroscope values
- stream-profile selection menus
- device information including IP address on supported models

## Related Examples

- [../2.lidar_device_control/README.md](../2.lidar_device_control/README.md) - inspect and change LiDAR properties
- [../3.lidar_record/README.md](../3.lidar_record/README.md) - record the same streams to a `.bag` file
