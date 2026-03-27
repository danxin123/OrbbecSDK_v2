# PCL Point Cloud Examples

This directory contains the PCL-based examples for OrbbecSDK.
Use them when you want to convert SDK point-cloud frames into PCL data structures and visualize them with the PCL viewer.

## Included Samples

- [pcl](pcl/README.md) - generate `pcl::PointCloud<pcl::PointXYZ>`
- [pcl_color](pcl_color/README.md) - generate `pcl::PointCloud<pcl::PointXYZRGB>`

## Prerequisites

- Install PCL on your system.
- Make sure CMake can find PCL. If needed, pass `-DPCL_DIR=/path/to/PCL`.
- Build from the repository root with PCL examples enabled.

## Build

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOB_BUILD_PCL_EXAMPLES=ON -DPCL_DIR=/path/to/PCL
cmake --build build --config Release --target ob_pcl
cmake --build build --config Release --target ob_pcl_color
```

## Run

```bash
.\build\win_x64\bin\ob_pcl.exe           # Windows
.\build\win_x64\bin\ob_pcl_color.exe     # Windows
./build/linux_x86_64/bin/ob_pcl          # Linux x86_64
./build/linux_x86_64/bin/ob_pcl_color    # Linux x86_64
./build/linux_arm64/bin/ob_pcl           # Linux ARM64
./build/linux_arm64/bin/ob_pcl_color     # Linux ARM64
./build/macOS/bin/ob_pcl                 # macOS
./build/macOS/bin/ob_pcl_color           # macOS
```

## Which One Should I Choose?

| Sample | When to use it |
| --- | --- |
| [pcl](pcl/README.md) | You only need geometry and want an XYZ point cloud |
| [pcl_color](pcl_color/README.md) | You need aligned color + depth and want an RGB point cloud |

## Notes

- The CMake option is `OB_BUILD_PCL_EXAMPLES`, not `OB_BUILD_PCL`.
- On Windows, you may need to ensure the required PCL DLLs are available next to the executable or on `PATH`.
- These samples are intended for users who already have the core SDK examples working and want to move into point-cloud tooling.
