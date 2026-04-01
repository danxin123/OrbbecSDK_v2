# 3D Measurement

Click **two points** on a live RGBD image to measure their real-world 3D distance.

## What it does

1. Configures Pipeline with Color + Depth, aligned via `ob::Align(OB_STREAM_COLOR)`
2. Displays the color image in an OpenCV window with mouse interaction
3. On **left-click**, records a point and reads its depth from the aligned depth frame
4. After **two clicks**, projects both pixels to 3D via `CoordinateTransformHelper::transformation2dto3d()`
5. Computes the **Euclidean distance** in millimeters and overlays the result on the image

## Controls

| Key / Action | Result |
|------------|--------|
| **Left-click** | Place point A, then B |
| **Click again** | Reset and start new measurement |
| **R** | Reset current measurement |
| **Esc** | Exit |

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

## Build & Run

```bash
# Build from the repository root
# This target is generated only when OpenCV is found.
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_3d_measurement

# Run
.\build\win_x64\bin\ob_3d_measurement.exe     # Windows
./build/linux_x86_64/bin/ob_3d_measurement    # Linux x86_64
./build/linux_arm64/bin/ob_3d_measurement     # Linux ARM64
./build/macOS/bin/ob_3d_measurement           # macOS
```

## How it works

```
Pixel (u,v) + Depth(u,v)
         │
         ▼
  transformation2dto3d(pixel, depthMm, intrinsic, identity_extrinsic)
         │
         ▼
  3D Point (X, Y, Z) in mm
```

After D2C alignment, the depth frame's intrinsics match the color camera.
An **identity extrinsic** is used since both measurements are in the same coordinate system.

The real-world distance between two 3D points:
$$d = \sqrt{(X_2 - X_1)^2 + (Y_2 - Y_1)^2 + (Z_2 - Z_1)^2}$$

## Key Concepts

- **`ob::Align(OB_STREAM_COLOR)`** — Warps depth to color camera space so pixel coordinates match
- **`DepthFrame::getValueScale()`** — Converts raw uint16 depth → millimeters
- **`CoordinateTransformHelper::transformation2dto3d()`** — Back-projects a 2D pixel with depth to a 3D point
- **Identity `OBExtrinsic`** — No rotation/translation needed within the same (aligned) coordinate frame

## Next Steps

- [camera_params](../../advanced/camera_params/) — Inspect intrinsics, extrinsics, and distortion
- [align_modes](../../advanced/align_modes/) — Compare hardware and software alignment behavior
- [../../README.md](../../README.md) — Return to the examples overview and choose the next workflow
