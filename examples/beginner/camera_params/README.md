# Camera Params

This example prints camera intrinsics, distortion, and extrinsics, then demonstrates basic 2D-to-3D and 3D-to-2D coordinate transforms.
It is a terminal-only sample for users who need to inspect calibration information directly.

## When To Use It

- inspect depth and color intrinsics
- inspect lens distortion parameters
- inspect depth-to-color and color-to-depth extrinsics
- verify basic coordinate transform behavior with live stream profiles

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
- No OpenCV dependency is required

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_camera_params
```

```bash
.\build\win_x64\bin\ob_camera_params.exe     # Windows
./build/linux_x86_64/bin/ob_camera_params    # Linux x86_64
./build/linux_arm64/bin/ob_camera_params     # Linux ARM64
./build/macOS/bin/ob_camera_params           # macOS
```

## What The Sample Does

1. Starts depth and color streaming
2. Waits for a valid frameset
3. Reads the active stream profiles from the returned frames
4. Prints:
   - depth and color stream resolution and frame rate
   - depth and color intrinsics
   - depth and color distortion coefficients
   - depth-to-color and color-to-depth extrinsics
5. Runs one 2D-to-3D transform demo and one 3D-to-2D transform demo

## Output Example

```
  Camera Parameters Report
==============================

  Depth stream: 640x576 @ 30fps
  Color stream: 1920x1080 @ 30fps

  [Depth Intrinsic]
    Resolution: 640 x 576
    fx=504.123  fy=504.456
    cx=320.789  cy=288.012

  [Extrinsic: Depth → Color]
    Rotation:
      [    0.999987    0.001234   -0.004567 ]
      ...
    Translation (mm): [25.123, -0.456, 0.789]

  [2D→3D Transform Demo]
    Input: pixel(320, 288) depth=1000mm
    Output: 3D(0.12, -0.05, 1000.00) mm

  [3D→2D Transform Demo]
    Input: 3D(0, 0, 1000) mm
    Output: pixel(320.79, 288.01)
```

## Key Concepts

- **`VideoStreamProfile::getIntrinsic()`** — Camera internal parameters (fx, fy, cx, cy)
- **`VideoStreamProfile::getDistortion()`** — Lens distortion coefficients (k1-k6, p1-p2)
- **`StreamProfile::getExtrinsicTo()`** — Rotation + Translation between two sensors
- **`CoordinateTransformHelper::transformation2dto3d()`** — Back-project pixel + depth → 3D point
- **`CoordinateTransformHelper::transformation3dto2d()`** — Project 3D point → pixel coordinate
