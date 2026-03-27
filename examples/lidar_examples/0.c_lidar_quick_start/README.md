# LiDAR Quick Start with C

This is the C API version of the LiDAR quick-start example.
It saves the current LiDAR point cloud to a PLY file when you press a key.

## When To Use It

- start from the C API instead of C++
- verify LiDAR streaming with the C interface
- save a quick sample point cloud to `LiDARPoints.ply`

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_quick_start_c
```

```bash
.\build\win_x64\bin\ob_lidar_quick_start_c.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_quick_start_c    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_quick_start_c     # Linux ARM64
./build/macOS/bin/ob_lidar_quick_start_c           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `R` | Capture the current LiDAR point cloud and save it to `LiDARPoints.ply` |
| `Esc` | Exit |

## Notes

- This sample demonstrates the same workflow as the C++ version, but with explicit C-style resource management and error handling.
