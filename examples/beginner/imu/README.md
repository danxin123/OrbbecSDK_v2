# IMU Viewer

Real-time ASCII visualization of accelerometer and gyro data.

## Features

- Real-time terminal refresh of IMU six-axis data (Accel X/Y/Z, Gyro X/Y/Z)
- ASCII bar chart visualizing value magnitude and direction
- Simple inclination angle estimation (Pitch/Roll) from acceleration components
- Display sensor temperature and real-time frame rate
- No OpenCV dependency (pure terminal)

## Key Concepts

- `ob::AccelFrame` / `ob::GyroFrame` — `getValue()` and `getTemperature()` methods
- `config->enableAccelStream()` / `enableGyroStream()`
- `OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE` — synchronous output mode
- ANSI escape codes for terminal refresh

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_imu_viewer
```

```bash
.\build\win_x64\bin\ob_imu_viewer.exe     # Windows
./build/linux_x86_64/bin/ob_imu_viewer    # Linux x86_64
./build/linux_arm64/bin/ob_imu_viewer     # Linux ARM64
./build/macOS/bin/ob_imu_viewer           # macOS
```

Press **ESC** to exit.

## Dependencies

- OrbbecSDK v2
- No OpenCV required
