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

## Run

```bash
./build/bin/ob_imu_viewer        # Linux
.\build\win_x64\bin\ob_imu_viewer.exe  # Windows
```

Press **ESC** to exit.

## Dependencies

- OrbbecSDK v2
- No OpenCV required
