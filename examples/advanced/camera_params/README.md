# Camera Params

Print camera **intrinsics**, **distortion coefficients**, **extrinsic matrices**, and demonstrate **2D↔3D coordinate transforms** — all from the terminal.

## No OpenCV Required

This example is purely CLI-based.

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
