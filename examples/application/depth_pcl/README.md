# PCL Point Cloud Examples

This directory contains the PCL-based examples for OrbbecSDK.
Use them when you want to convert SDK point-cloud frames into PCL data structures and visualize them with the PCL viewer.

## Included Samples

- [pcl](pcl/README.md) - generate `pcl::PointCloud<pcl::PointXYZ>`
- [pcl_color](pcl_color/README.md) - generate `pcl::PointCloud<pcl::PointXYZRGB>`

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
| Astra Mini Series | Astra Mini Pro, Astra Mini S Pro |

> Refer to the [Supported Devices and Firmware](https://github.com/orbbec/OrbbecSDK_v2?tab=readme-ov-file#supported-devices-and-firmware) section in the main README for more details.

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
