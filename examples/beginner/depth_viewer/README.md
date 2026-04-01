# Depth Viewer

Use this example when you want to inspect the depth stream only and do not need the color image.
It is a practical sample for checking depth quality, switching colormaps, and saving raw depth images.

## When To Use It

- verify that the depth camera is streaming correctly
- inspect depth-only output without RGB alignment
- save 16-bit depth PNG files for later analysis
- compare 2D colormap and 3D-style rendering

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
- OpenCV is required for the display window and PNG saving

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_depth_viewer
```

```bash
.\build\win_x64\bin\ob_depth_viewer.exe     # Windows
./build/linux_x86_64/bin/ob_depth_viewer    # Linux x86_64
./build/linux_arm64/bin/ob_depth_viewer     # Linux ARM64
./build/macOS/bin/ob_depth_viewer           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `M` | Toggle 3D-style depth rendering |
| `C` | Cycle the depth colormap |
| `S` | Save the current depth frame as a 16-bit PNG |
| `Esc` | Exit |

## What You Will See

- a live depth window
- the current visualization mode shown in the overlay
- terminal messages when render mode changes
- a saved depth PNG when you press `S`

## Related Examples

- [../rgbd_viewer/README.md](../rgbd_viewer/README.md) - visualize aligned color + depth together
- [../../application/3d_measurement/README.md](../../application/3d_measurement/README.md) - measure real-world distance from RGB-D data
