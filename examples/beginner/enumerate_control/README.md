# Enumerate & Control

This terminal example helps you inspect connected devices, available sensors, stream profiles, and device properties.
Use it as the first troubleshooting tool when you need to confirm what the SDK can see from the hardware.

## When To Use It

- check whether the device is detected correctly
- inspect supported sensors and stream profiles
- read or update device properties from the terminal
- confirm property ranges and read/write permissions

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
