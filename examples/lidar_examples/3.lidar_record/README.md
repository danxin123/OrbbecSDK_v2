# LiDAR Record

This sample records LiDAR and IMU data to a `.bag` file.
It is the recommended way to capture a reusable LiDAR dataset for offline testing.

## When To Use It

- record a dataset for later playback
- capture LiDAR and IMU data together
- monitor stream frame rate while recording
- create repeatable input data for algorithm debugging

## Supported Devices

| Device Series | Models |
|---------------|--------|
| LiDAR Series | dToF LiDAR Pulsar SL450, dToF LiDAR Pulsar ME450 |

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_record
```

```bash
.\build\win_x64\bin\ob_lidar_record.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_record    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_record     # Linux ARM64
./build/macOS/bin/ob_lidar_record           # macOS
```

## How To Use It

1. Enter an output file name with the `.bag` suffix.
2. The sample validates the device and enables all available sensors.
3. Monitor the FPS output in the terminal while recording.
4. Press `Esc`, `Q`, or `q` to stop recording safely.

## Important

- Always stop the program with the documented exit key.
- Do not terminate the process directly, or the `.bag` file may be corrupted.

## Output

- a `.bag` file containing recorded LiDAR and IMU data
- live FPS information for the active sensor streams

## Related Examples

- [../4.lidar_playback/README.md](../4.lidar_playback/README.md) - replay the recorded dataset
- [../1.lidar_stream/README.md](../1.lidar_stream/README.md) - inspect the same streams live before recording
