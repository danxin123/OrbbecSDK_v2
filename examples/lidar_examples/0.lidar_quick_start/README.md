# LiDAR Quick Start

This is the fastest LiDAR example to run.
It starts the default LiDAR stream and saves the current point cloud to a PLY file when you press a key.

## When To Use It

- verify that the LiDAR can stream correctly
- save a quick sample point cloud without building a full workflow
- check the generated PLY file in a point-cloud viewer

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_quick_start
```

```bash
.\build\win_x64\bin\ob_lidar_quick_start.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_quick_start    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_quick_start     # Linux ARM64
./build/macOS/bin/ob_lidar_quick_start           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `R` | Capture the current LiDAR point cloud and save it to `LiDARPoints.ply` |
| `Esc` | Exit |

## Output

- The sample saves `LiDARPoints.ply` in the current working directory.
- The file can be opened with common point-cloud visualization tools.

## Related Examples

- [../1.lidar_stream/README.md](../1.lidar_stream/README.md) - inspect live LiDAR and IMU data
- [../3.lidar_record/README.md](../3.lidar_record/README.md) - record LiDAR data to a `.bag` file
