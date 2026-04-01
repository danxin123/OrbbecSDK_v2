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

## Supported Devices

| Device Series | Models |
|---------------|--------|
| Gemini 330 Series | Gemini 330, Gemini 330L, Gemini 335, Gemini 335L, Gemini 335Le, Gemini 336, Gemini 336L, Gemini 335Lg |
| Gemini 305 Series | Gemini 305 |
| Gemini 340 Series | Gemini 345, Gemini 345Lg |
| Gemini 435 Series | Gemini 435Le |
| Gemini 2 Series | Gemini 2, Gemini 2L, Gemini 215, Gemini 210 |
| Femto Series | Femto Bolt, Femto Mega, Femto Mega I |
| Astra Series | Astra 2 |


> Refer to the [Supported Devices and Firmware](https://github.com/orbbec/OrbbecSDK_v2?tab=readme-ov-file#supported-devices-and-firmware) section in the main README for more details.

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
