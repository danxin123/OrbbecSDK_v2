# Depth Viewer

Use this example when you want to inspect the depth stream only and do not need the color image.
It is a practical sample for checking depth quality, switching colormaps, and saving raw depth images.

## When To Use It

- verify that the depth camera is streaming correctly
- inspect depth-only output without RGB alignment
- save 16-bit depth PNG files for later analysis
- compare 2D colormap and 3D-style rendering

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
