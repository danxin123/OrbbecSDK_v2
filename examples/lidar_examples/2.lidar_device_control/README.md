# LiDAR Device Control

This terminal sample lets you read and modify supported LiDAR device properties.
Use it when you need to inspect property ranges, confirm permissions, or adjust device settings during testing.

## When To Use It

- inspect supported LiDAR properties
- verify read/write permission for each property
- get the current value of a property
- change device parameters from the command line

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A supported Orbbec LiDAR device must be connected

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_device_control
```

```bash
.\build\win_x64\bin\ob_lidar_device_control.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_device_control    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_device_control     # Linux ARM64
./build/macOS/bin/ob_lidar_device_control           # macOS
```

## Command Format

- `?` - print the property list
- `[index] get` - read the current property value
- `[index] set [value]` - write a new property value
- `exit` - quit the program

## What You Will See

- property name and internal ID
- read/write permission
- value type and valid range
- updated value after a successful write

## Related Examples

- [../1.lidar_stream/README.md](../1.lidar_stream/README.md) - inspect live stream output
- [../3.lidar_record/README.md](../3.lidar_record/README.md) - validate settings by recording data
