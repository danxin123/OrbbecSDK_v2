# LiDAR Examples

This directory contains user-facing examples for Orbbec LiDAR devices such as Pulsar ME450 and Pulsar SL450.
Use this guide when you want to:

- capture a point cloud and save it to PLY
- stream live LiDAR and IMU data
- inspect or change device properties
- record a `.bag` file
- replay recorded LiDAR data

## Before You Start

- LiDAR support is available in OrbbecSDK v2.6.2 and later.
- Build from the repository root:

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

- Output directories when built from the repository root:
  - Windows x64: `build/win_x64/bin`
  - Linux x86_64: `build/linux_x86_64/bin`
  - Linux ARM64: `build/linux_arm64/bin`
  - macOS: `build/macOS/bin`

Useful references:

- Top-level LiDAR guide: [../LiDAR_README.md](../LiDAR_README.md)
- LiDAR API User Guide: <https://orbbec.github.io/docs/OrbbecSDKv2_LiDAR_User_Guide/>
- Orbbec Viewer for LiDAR: [../docs/tutorial/orbbecviewer.md#orbbecviewer-for-lidar](../docs/tutorial/orbbecviewer.md#orbbecviewer-for-lidar)

## Which Example Should I Run?

| Goal | Example | Target |
| --- | --- | --- |
| Save one point cloud to PLY as quickly as possible | [0.lidar_quick_start](lidar_examples/0.lidar_quick_start/README.md) | `ob_lidar_quick_start` |
| Watch live LiDAR and IMU frames and choose stream profiles | [1.lidar_stream](lidar_examples/1.lidar_stream/README.md) | `ob_lidar_stream` |
| Inspect or change LiDAR properties from the terminal | [2.lidar_device_control](lidar_examples/2.lidar_device_control/README.md) | `ob_lidar_device_control` |
| Record live LiDAR + IMU data to `.bag` | [3.lidar_record](lidar_examples/3.lidar_record/README.md) | `ob_lidar_record` |
| Replay a recorded `.bag` file | [4.lidar_playback](lidar_examples/4.lidar_playback/README.md) | `ob_lidar_playback` |
| Use the C API for the same workflow | [C API examples](#c-api-examples) | `ob_lidar_quick_start_c`, `ob_lidar_stream_c` |

## Recommended Order

1. [0.lidar_quick_start](lidar_examples/0.lidar_quick_start/README.md) - verify the device and save one PLY file
2. [1.lidar_stream](lidar_examples/1.lidar_stream/README.md) - learn sensor selection and frame formats
3. [2.lidar_device_control](lidar_examples/2.lidar_device_control/README.md) - inspect and change supported properties
4. [3.lidar_record](lidar_examples/3.lidar_record/README.md) - capture a reusable dataset
5. [4.lidar_playback](lidar_examples/4.lidar_playback/README.md) - replay and debug offline
6. Move to the C API examples only if your application needs the C interface

## Typical Workflows

### 1. Capture a point cloud and save it to PLY

```bash
cmake --build build --config Release --target ob_lidar_quick_start
```

```bash
.\build\win_x64\bin\ob_lidar_quick_start.exe   # Windows
./build/linux_x86_64/bin/ob_lidar_quick_start  # Linux x86_64
./build/linux_arm64/bin/ob_lidar_quick_start   # Linux ARM64
./build/macOS/bin/ob_lidar_quick_start         # macOS
```

What to do:

1. Start the program.
2. Press `R` to save the current LiDAR frame.
3. The sample writes `LiDARPoints.ply` to the current working directory.
4. Press `Esc` to exit.

### 2. Inspect live LiDAR and IMU streaming

```bash
cmake --build build --config Release --target ob_lidar_stream
```

This sample is the best place to understand what the device can actually output.

What to do:

1. Select a device. If only one device is connected, it is selected automatically.
2. Select the sensor set to enable:
   - `LiDAR`
   - `Accel`
   - `Gyro`
   - or all sensors
3. Choose a stream profile for each selected sensor.
4. Watch the terminal output for frame index, timestamp, point-cloud format, and IMU values.
5. Press `Esc` to exit.

### 3. Inspect or change LiDAR properties

```bash
cmake --build build --config Release --target ob_lidar_device_control
```

Typical commands after the program starts:

- `?` - list all available properties
- `[index] get` - read the current value
- `[index] set [value]` - write a new value
- `exit` - leave the program

This sample is useful when you need to inspect property ranges, check read/write permissions, or adjust device behavior for testing.

### 4. Record a LiDAR dataset

```bash
cmake --build build --config Release --target ob_lidar_record
```

What to do:

1. Enter an output file name with the `.bag` suffix.
2. The sample enables all available sensors and starts recording.
3. Watch the FPS output in the terminal.
4. Stop the recording with `Esc`, `Q`, or `q`.

Important:

- Do not close the terminal or kill the process directly.
- Use the documented exit key so the `.bag` file is flushed correctly.

### 5. Replay a recorded dataset

```bash
cmake --build build --config Release --target ob_lidar_playback
```

What to do:

1. Enter the path to a valid `.bag` file.
2. The sample prints the recording duration and starts playback.
3. Press `P` to pause or resume.
4. Press `Esc` to stop playback and exit.

This is the quickest way to validate an offline dataset with the same frame-processing path used for live streaming.

## Example Index

### C++ examples

| Example | Target | Description |
| --- | --- | --- |
| [0.lidar_quick_start](lidar_examples/0.lidar_quick_start/README.md) | `ob_lidar_quick_start` | Save LiDAR point cloud data to `LiDARPoints.ply` on demand |
| [1.lidar_stream](lidar_examples/1.lidar_stream/README.md) | `ob_lidar_stream` | Select sensors and stream profiles, then print LiDAR and IMU data |
| [2.lidar_device_control](lidar_examples/2.lidar_device_control/README.md) | `ob_lidar_device_control` | Interactively get and set supported LiDAR device properties |
| [3.lidar_record](lidar_examples/3.lidar_record/README.md) | `ob_lidar_record` | Record LiDAR and IMU data to a `.bag` file |
| [4.lidar_playback](lidar_examples/4.lidar_playback/README.md) | `ob_lidar_playback` | Replay a recorded `.bag` file and monitor frame information |

### C API examples

| Example | Target | Description |
| --- | --- | --- |
| [0.c_lidar_quick_start](lidar_examples/0.c_lidar_quick_start/README.md) | `ob_lidar_quick_start_c` | C API version of the PLY capture example |
| [1.c_lidar_stream](lidar_examples/1.c_lidar_stream/README.md) | `ob_lidar_stream_c` | C API version of the live LiDAR + IMU streaming sample |

## Notes

- Stream and control samples automatically select the first device when only one LiDAR is connected.
- `lidar_record` is intended to produce data for `lidar_playback`; together they form the standard capture -> replay workflow.
- `lidar_stream` and `c_lidar_stream` are the best references if you want to build your own LiDAR application, because they show device selection, stream profile selection, and frame parsing.
- The shared utility helpers used by the examples live under [utils/](utils/).
