# Infrared Streams

Auto-detect and display single or dual infrared streams with optional emitter control and frame saving.

## When To Use It

- inspect the infrared output of a device with IR sensors
- verify whether the device exposes single IR or left/right IR streams
- compare IR images with the emitter enabled or disabled on supported devices
- save current IR images for later inspection

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
- OpenCV is required for the display window and image saving
- The connected device must expose at least one of `OB_SENSOR_IR`, `OB_SENSOR_IR_LEFT`, or `OB_SENSOR_IR_RIGHT`

## Features

| Key | Function |
|-----|----------|
| **E** | Toggle IR emitter/laser when `OB_PROP_LASER_BOOL` is supported |
| **S** | Save the current IR frame(s) as PNG |
| **ESC** | Exit |

- Auto-detect `OB_SENSOR_IR` / `OB_SENSOR_IR_LEFT` / `OB_SENSOR_IR_RIGHT`
- Stereo devices auto-display left and right IR streams side-by-side
- Print resolution, frame rate, and format of each IR stream on startup

## Key Concepts

- Multi-IR sensor enumeration with `enableVideoStream(sensorType, ...)`
- `OB_PROP_LASER_BOOL` property to control emitter when supported
- IR frame format handling (Y8 / Y16)
- `CVWindow` with single-view or one-row layout depending on detected IR streams

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_infrared_streams
```

## Run

```bash
.\build\win_x64\bin\ob_infrared_streams.exe     # Windows
./build/linux_x86_64/bin/ob_infrared_streams    # Linux x86_64
./build/linux_arm64/bin/ob_infrared_streams     # Linux ARM64
./build/macOS/bin/ob_infrared_streams           # macOS
```

## Notes

- On startup, the sample prints the detected IR sensors and active stream profiles.
- If the device does not expose any IR sensor, the sample prints a message and exits.
- The `E` shortcut appears in the window prompt only when emitter control is supported.
