# OrbbecSDK v2 Examples

## Quick Start

```bash
# Build with examples enabled
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release

# Run the quick start example
./build/bin/ob_quick_start      # Linux
.\build\win_x64\bin\ob_quick_start.exe  # Windows
```

Press **M** to toggle 3D depth rendering, **C** to cycle colormaps.

## Level 1 — Beginner

Sequential numbered tutorials for getting started with OrbbecSDK.

| # | Example | Description |
|---|---------|-------------|
| 01 | [quick_start](beginner/01_quick_start) | Pipeline basics — get frames from RGB-D camera and display them. Press **M** for 3D depth, **C** to cycle colormaps. |
| 02 | [depth_viewer](beginner/02_depth_viewer) | Depth stream with colormap visualization. |
| 03 | [color_viewer](beginner/03_color_viewer) | Color stream display with format handling. |
| 04 | [enumerate](beginner/04_enumerate) | Device discovery — list devices, sensors, and stream profiles. |
| 05 | [point_cloud](beginner/05_point_cloud) | Generate depth/RGBD point cloud and save as PLY. |
| 06 | [multi_streams](beginner/06_multi_streams) | Display all streams (color, depth, IR) simultaneously. |
| 07 | [imu](beginner/07_imu) | Read accelerometer and gyroscope data. |
| 08 | [infrared](beginner/08_infrared) | Infrared stream display. |
| 09 | [firmware_update](beginner/09_firmware_update) | OTA firmware upgrade from BIN file. |

## Level 2 — Advanced

Deep-dive into individual SDK features, grouped by category.

### Recording & Playback

| Example | Description |
|---------|-------------|
| [record](advanced/record) | Record color/depth streams to Rosbag file. |
| [record_nogui](advanced/record_nogui) | CLI-based recording without rendering. |
| [playback](advanced/playback) | Play back recorded Rosbag data. |

### Device & System

| Example | Description |
|---------|-------------|
| [control](advanced/control) | Adjust device parameters (laser, white balance, etc.). |
| [hot_plugin](advanced/hot_plugin) | Handle device plug/unplug events. |
| [forceip](advanced/forceip) | Configure IP for GigE network devices. |
| [firmware_update (multi)](advanced/multi_devices_firmware_update) | Upgrade firmware on multiple connected devices. |
| [depth_presets_update](advanced/optional_depth_presets_update) | Update optional depth presets from BIN file. |

### Depth Processing

| Example | Description |
|---------|-------------|
| [sync_align](advanced/sync_align) | Synchronize and align depth-to-color streams. |
| [hw_d2c_align](advanced/hw_d2c_align) | Hardware depth-to-color alignment. |
| [post_processing](advanced/post_processing) | Apply post-processing filters to depth. |
| [hdr](advanced/hdr) | HDR merge for depth frames. |
| [preset](advanced/preset) | Set/get device depth presets. |
| [coordinate_transform](advanced/coordinate_transform) | Transform between coordinate systems. |
| [confidence](advanced/confidence) | Depth and confidence stream display. |
| [decimation](advanced/decimation) | Decimation filter with selectable profiles. |

### Streaming

| Example | Description |
|---------|-------------|
| [callback](advanced/callback) | Frame callback with custom data processing. |
| [common_usages](advanced/common_usages) | Common SDK usage patterns and recipes. |
| [laser_interleave](advanced/laser_interleave) | Laser interleave mode control. |

### Multi-Device

| Example | Description |
|---------|-------------|
| [multi_devices](advanced/multi_devices) | Connect and stream from multiple cameras. |
| [multi_devices_sync](advanced/multi_devices_sync) | Multi-device hardware synchronization via JSON config. |
| [multi_devices_sync_gmsltrigger](advanced/multi_devices_sync_gmsltrigger) | GMSL PWM trigger for multi-device sync (Linux only). |

### Utilities & Misc

| Example | Description |
|---------|-------------|
| [logger](advanced/logger) | Configure SDK log output level and path. |
| [metadata](advanced/metadata) | Retrieve per-frame metadata. |
| [save_to_disk](advanced/save_to_disk) | Save color/depth frames as image files. |

### Wrapper Integration

| Example | Description | Requires |
|---------|-------------|----------|
| [wrapper_opencv](advanced/wrapper_opencv) | OpenCV integration samples. | OpenCV |
| [wrapper_pcl](advanced/wrapper_pcl) | PCL point cloud visualization. | PCL |
| [wrapper_open3d](advanced/wrapper_open3d) | Open3D integration. | Open3D |

## Specialized — LiDAR

See [LiDAR_README.md](LiDAR_README.md) for LiDAR-specific examples.

## C Language Examples

| # | Example | Description |
|---|---------|-------------|
| 1 | [c_quick_start](c_examples/0.c_quick_start) | Quick start using the C API. |
| 2 | [c_enumerate](c_examples/1.c_enumerate) | Device enumeration using the C API. |
| 3 | [c_depth](c_examples/2.c_depth) | Depth stream using the C API. |

## Learning Path

1. **Start here** → [01_quick_start](beginner/01_quick_start) — Get your first frames displayed
2. **Explore streams** → [02_depth_viewer](beginner/02_depth_viewer), [03_color_viewer](beginner/03_color_viewer), [08_infrared](beginner/08_infrared)
3. **Understand devices** → [04_enumerate](beginner/04_enumerate)
4. **3D output** → [05_point_cloud](beginner/05_point_cloud)
5. **Alignment** → [sync_align](advanced/sync_align), [hw_d2c_align](advanced/hw_d2c_align)
6. **Depth quality** → [post_processing](advanced/post_processing), [hdr](advanced/hdr)
7. **Multi-camera** → [multi_devices](advanced/multi_devices), [multi_devices_sync](advanced/multi_devices_sync)
8. **Recording** → [record](advanced/record), [playback](advanced/playback)

## Utilities

The [utils/](utils/) directory provides shared utilities used by all examples:

- **utils.hpp/cpp** — Key press helpers, timestamp utilities, device type checks
- **utils_opencv.hpp/cpp** — `CVWindow` class for frame display (5 layout modes), `renderDepth3D()` for 3D depth relief rendering
- **utils_c.h/c** — C-language utility functions

## Error Handling

All examples use a `try/catch` block wrapping `main()`. SDK errors are reported via `ob::Error` exceptions with:
- `getFunction()` — Function where the error occurred
- `getArgs()` — Arguments at time of error
- `what()` — Error description
- `getExceptionType()` — Error category (see `OBExceptionType` in `ObTypes.h`)
