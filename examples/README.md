# OrbbecSDK v2 Examples

## Quick Start

```bash
# Build with examples enabled
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release

# Run the quick start example (no OpenCV needed)
./build/bin/ob_quick_start_cli      # Linux
.\build\win_x64\bin\ob_quick_start_cli.exe  # Windows
```

## Directory Structure

```
examples/
├── beginner/               # Beginner-level examples, start here for new users
│   ├── quick_start/        # Pipeline basics — print depth data to terminal (no OpenCV)
│   ├── rgbd_viewer/        # RGB-D visualization + D2C alignment + save (OpenCV)
│   ├── depth_viewer/       # Depth stream + colormap + 3D rendering + save (OpenCV)
│   ├── enumerate_control/  # Device enumeration + property control CLI (no OpenCV)
│   ├── record_playback/    # Recording/playback .bag file combined (OpenCV)
│   ├── hot_plugin/         # Device hot-plug event handling (no OpenCV)
│   └── imu/                # IMU six-axis ASCII visualization (no OpenCV)
├── advanced/               # Advanced examples, deep dive into SDK features
│   ├── align_modes/        # Hardware vs Software depth alignment comparison (OpenCV)
│   ├── camera_params/      # Camera intrinsics/extrinsics + coordinate transformation (no OpenCV)
│   ├── infrared_streams/   # Single/dual infrared + emitter control (OpenCV)
│   └── ... (other advanced examples)
├── application/            # Application-level examples
│   └── 3d_measurement/     # Click two points to measure 3D distance (OpenCV)
├── c_examples/             # C language API examples
├── lidar_examples/         # LiDAR-specific examples
└── utils/                  # Shared utility library
```

## Level 1 — Beginner

| Example | OpenCV | Description |
|---------|--------|-------------|
| [quick_start](beginner/quick_start) | ✗ | Minimal Pipeline example — print center pixel depth value to terminal |
| [rgbd_viewer](beginner/rgbd_viewer) | ✓ | RGB-D visualization, D2C alignment, S=save, M=3D rendering, C=colormap cycle |
| [depth_viewer](beginner/depth_viewer) | ✓ | Depth stream visualization, M=3D rendering, S=save 16-bit PNG, D=raw depth, C=colormap |
| [enumerate_control](beginner/enumerate_control) | ✗ | Device/sensor/StreamProfile enumeration + property get/set CLI menu |
| [record_playback](beginner/record_playback) | ✓ | Record all streams to .bag file / playback with pause/resume support |
| [hot_plugin](beginner/hot_plugin) | ✗ | Device hot-plug callback, R=restart device to trigger disconnect/reconnect |
| [imu](beginner/imu) | ✗ | Accelerometer/Gyroscope six-axis ASCII bar chart + inclination angle estimation |

## Level 2 — Advanced

### Depth Processing & Alignment

| Example | OpenCV | Description |
|---------|--------|-------------|
| [align_modes](advanced/align_modes) | ✓ | Hardware / Software / Disabled D2C alignment mode comparison |
| [camera_params](advanced/camera_params) | ✗ | Camera intrinsics/distortion/extrinsics print + 2D⇄3D coordinate transformation demo |
| [infrared_streams](advanced/infrared_streams) | ✓ | Single/dual infrared display, E=toggle emitter, S=save |
| [post_processing](advanced/post_processing) | ✓ | Depth post-processing filters |
| [hdr](advanced/hdr) | ✓ | Depth HDR merge |
| [confidence](advanced/confidence) | ✓ | Depth + confidence stream display |
| [preset](advanced/preset) | ✗ | Depth preset values |

### Recording & Streaming

| Example | OpenCV | Description |
|---------|--------|-------------|
| [callback](advanced/callback) | ✓ | Frame callback custom processing |
| [laser_interleave](advanced/laser_interleave) | ✓ | Laser interleave mode |

### Device & System

| Example | OpenCV | Description |
|---------|--------|-------------|
| [forceip](advanced/forceip) | ✗ | GigE device IP configuration |
| [optional_depth_presets_update](advanced/optional_depth_presets_update) | ✗ | Depth preset value update |
| [metadata](advanced/metadata) | ✗ | Per-frame metadata reading |
| [save_to_disk](advanced/save_to_disk) | ✓ | Save color/depth frames as images |

### Multi-Device

| Example | OpenCV | Description |
|---------|--------|-------------|
| [multi_devices_sync](advanced/multi_devices_sync) | ✓ | Multi-device hardware synchronization |
| [multi_devices_sync_gmsltrigger](advanced/multi_devices_sync_gmsltrigger) | ✓ | GMSL PWM trigger synchronization (Linux) |

### Wrapper Integration

| Example | Requires | Description |
|---------|----------|-------------|
| [wrapper_pcl](advanced/wrapper_pcl) | PCL | PCL point cloud visualization |
| [wrapper_open3d](advanced/wrapper_open3d) | Open3D | Open3D integration |

## Level 3 — Application

| Example | OpenCV | Description |
|---------|--------|-------------|
| [3d_measurement](application/3d_measurement) | ✓ | Click two points to measure 3D distance |

## Specialized

- **LiDAR** → [LiDAR_README.md](LiDAR_README.md)
- **C API** → [c_examples/](c_examples/)

## Learning Path

1. **Start** → [quick_start](beginner/quick_start) — Minimal runnable example
2. **See RGB-D** → [rgbd_viewer](beginner/rgbd_viewer) — Visualization + D2C alignment
3. **Depth deep-dive** → [depth_viewer](beginner/depth_viewer) — Colormap, 3D, save
4. **Know your device** → [enumerate_control](beginner/enumerate_control) — Enumeration + properties
5. **Camera math** → [camera_params](advanced/camera_params) — Intrinsics/extrinsics and coordinate transformation
6. **Alignment** → [align_modes](advanced/align_modes) — Hardware vs Software alignment
7. **Depth quality** → [post_processing](advanced/post_processing), [hdr](advanced/hdr)
8. **Multi-camera** → [multi_devices_sync](advanced/multi_devices_sync)
9. **Record** → [record_playback](beginner/record_playback) — Recording + playback
10. **Application** → [3d_measurement](application/3d_measurement) — Real-world measurement application

## Utilities
The [utils/](utils/) directory provides tools shared by all examples:

- **utils.hpp/cpp** — Key press detection, timestamp utilities, device type identification
- **utils_opencv.hpp/cpp** — `CVWindow` (5 layout modes), `renderDepth3D()` 3D depth rendering
- **utils_c.h/c** — C language utility functions

## Error Handling

All examples use a `try/catch` block wrapping `main()`. SDK errors are reported via `ob::Error` exceptions with:
- `getFunction()` — Function where the error occurred
- `getArgs()` — Arguments at time of error
- `what()` — Error description
- `getExceptionType()` — Error category (see `OBExceptionType` in `ObTypes.h`)
