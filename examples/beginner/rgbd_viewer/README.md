# RGBD Viewer

Acquire **aligned RGB-D streams** and display them with OpenCV overlay visualization.

## What it does

1. Configures a Pipeline with both **Color** and **Depth** streams
2. Applies **software Depth-to-Color alignment** (`ob::Align(OB_STREAM_COLOR)`)
3. Displays the aligned RGBD in an **overlay** window (depth blended over color)
4. Supports **saving** depth (16-bit PNG) and color (8-bit PNG) to disk
5. Optional **3D depth rendering** with Scharr gradient lighting

## Controls

| Key | Action |
|-----|--------|
| **M** | Toggle 3D depth rendering |
| **S** | Save current depth + color as PNG |
| **C** | Cycle colormap (JET → TURBO → MAGMA → INFERNO → PLASMA) |
| **+/-** | Adjust depth/color overlay transparency |
| **Esc** | Exit |

## Build & Run

```bash
# Build from the repository root
# This target is generated only when OpenCV is found.
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_rgbd_viewer

# Run
.\build\win_x64\bin\ob_rgbd_viewer.exe     # Windows
./build/linux_x86_64/bin/ob_rgbd_viewer    # Linux x86_64
./build/linux_arm64/bin/ob_rgbd_viewer     # Linux ARM64
./build/macOS/bin/ob_rgbd_viewer           # macOS
```

## Key Concepts

- **`ob::Align(OB_STREAM_COLOR)`** — Software filter that warps depth map to match the color camera's coordinate space
- **`config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE)`** — Ensures both depth and color are present in every frameset
- **`DepthFrame::getData()`** — Raw 16-bit depth buffer, suitable for `cv::Mat(h, w, CV_16UC1, data)`
- **`FormatConvertFilter`** — Converts color formats (MJPG/YUYV/UYVY/RGB) to BGR for OpenCV

## Next Steps

- [align_modes](../../advanced/align_modes/) — Compare hardware vs software alignment
- [camera_params](../../advanced/camera_params/) — Get intrinsics, extrinsics, and do coordinate transforms
- [3d_measurement](../../application/3d_measurement/) — Measure real-world distances using aligned RGBD
