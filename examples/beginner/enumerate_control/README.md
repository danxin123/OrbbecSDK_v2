# Enumerate & Control

This terminal example helps you inspect connected devices, available sensors, stream profiles, and device properties.
Use it as the first troubleshooting tool when you need to confirm what the SDK can see from the hardware.

## When To Use It

- check whether the device is detected correctly
- inspect supported sensors and stream profiles
- read or update device properties from the terminal
- confirm property ranges and read/write permissions

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- No OpenCV or additional GUI dependency is required

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_enumerate_control
```

```bash
.\build\win_x64\bin\ob_enumerate_control.exe     # Windows
./build/linux_x86_64/bin/ob_enumerate_control    # Linux x86_64
./build/linux_arm64/bin/ob_enumerate_control     # Linux ARM64
./build/macOS/bin/ob_enumerate_control           # macOS
```

## How To Use It

1. Start the program and select a device index, or enter `q` to quit from the top-level device menu.
2. Choose `1` to inspect sensors and stream profiles, or `2` to inspect and modify device properties.
3. In the sensor menu, enter a sensor index to print its stream profiles, or enter `b` to go back.
4. In the property menu, use the commands below, or enter `b` to go back.

Property commands:

- `?` - list all available properties
- `<index> get` - read the current property value
- `<index> set <value>` - write a new property value
- `b` - go back to the previous menu

## What You Will Get

- device name, PID, serial number, and connection type
- sensor list and available stream profiles
- property type, current value, range, and permission information

## Related Examples

- [../quick_start/README.md](../quick_start/README.md) - verify basic streaming first
- [../../advanced/camera_params/README.md](../../advanced/camera_params/README.md) - inspect calibration data after device discovery
