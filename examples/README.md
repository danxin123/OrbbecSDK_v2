# OrbbecSDK v2 Examples

This directory contains runnable examples for the most common OrbbecSDK workflows.
If you are new to the SDK, start with the beginner examples.
If you are working with LiDAR devices, go straight to [LiDAR Examples](LiDAR_README.md).

## Before You Start

1. Build from the repository root, not from inside `examples/`.
2. Complete the platform environment setup if your device requires it:
   - Windows: [../scripts/env_setup/obsensor_metadata_win10.md](../scripts/env_setup/obsensor_metadata_win10.md)
   - Linux: [../scripts/env_setup/setup.sh](../scripts/env_setup/setup.sh)
3. Decide whether you need optional dependencies.

| Dependency | Needed for | Notes |
| --- | --- | --- |
| None | `quick_start`, `enumerate_control`, `imu`, `camera_params`, `metadata`, most LiDAR examples, all C examples | Good starting point for first-time users |
| OpenCV | `rgbd_viewer`, `depth_viewer`, `align_modes`, `record_playback`, `3d_measurement`, and most visualization samples | If OpenCV is not found, these targets are skipped |
| PCL | `application/depth_pcl` | Enable `OB_BUILD_PCL_EXAMPLES=ON` |
| Open3D | `application/rgbd_open3d` | Enable `OB_BUILD_OPEN3D_EXAMPLES=ON` |

## Build Examples

### Standard build

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

### Optional PCL examples

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOB_BUILD_PCL_EXAMPLES=ON -DPCL_DIR=/path/to/PCL
cmake --build build --config Release
```

### Optional Open3D examples

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOB_BUILD_OPEN3D_EXAMPLES=ON -DOpen3D_DIR=/path/to/Open3D
cmake --build build --config Release
```

## Output Directories

When you build from the repository root, example binaries are generated under the platform-specific output directory:

- Windows x64: `build/win_x64/bin`
- Linux x86_64: `build/linux_x86_64/bin`
- Linux ARM64: `build/linux_arm64/bin`
- macOS: `build/macOS/bin`

## Run Your First Example

The recommended first example is `quick_start`.
It has no external dependency and verifies that the SDK, device connection, and basic streaming path all work.

```bash
cmake --build build --config Release --target ob_quick_start_cli
```

```bash
.\build\win_x64\bin\ob_quick_start_cli.exe     # Windows
./build/linux_x86_64/bin/ob_quick_start_cli    # Linux x86_64
./build/linux_arm64/bin/ob_quick_start_cli     # Linux ARM64
./build/macOS/bin/ob_quick_start_cli           # macOS
```

## Which Example Should I Start With?

| Goal | Example | Why start here |
| --- | --- | --- |
| Verify the device can stream | [beginner/quick_start](beginner/quick_start/README.md) | Smallest runnable example with terminal output only |
| Visualize RGB-D data | [beginner/rgbd_viewer](beginner/rgbd_viewer/README.md) | Shows aligned color + depth in one window |
| Focus on depth only | [beginner/depth_viewer](beginner/depth_viewer/README.md) | Good for checking depth quality and colormaps |
| Inspect devices and properties | [beginner/enumerate_control](beginner/enumerate_control/README.md) | Helps you see sensors, profiles, and device properties |
| Record and replay a `.bag` file | [beginner/record_playback](beginner/record_playback/README.md) | Covers the common capture -> playback workflow |
| Use LiDAR devices | [LiDAR_README.md](LiDAR_README.md) | Dedicated guide for LiDAR streaming, control, recording, and playback |
| Use the C API | [c_examples/](c_examples/) | Minimal C-language entry points |

## Recommended Learning Path

1. [quick_start](beginner/quick_start/README.md) - Confirm the SDK and camera are working.
2. [rgbd_viewer](beginner/rgbd_viewer/README.md) or [depth_viewer](beginner/depth_viewer/README.md) - See live image output.
3. [enumerate_control](beginner/enumerate_control/README.md) - Understand sensors, stream profiles, and properties.
4. [record_playback](beginner/record_playback/README.md) - Save real data and replay it offline.
5. [camera_params](advanced/camera_params/README.md) and [align_modes](advanced/align_modes/README.md) - Learn calibration and alignment behavior.
6. Move to advanced or application examples only after the basics are clear.

## Example Index

### Beginner examples

| Directory | Target | Requires | Description |
| --- | --- | --- | --- |
| [beginner/quick_start](beginner/quick_start/README.md) | `ob_quick_start_cli` | None | Minimal pipeline example that prints depth information to the terminal |
| [beginner/rgbd_viewer](beginner/rgbd_viewer/README.md) | `ob_rgbd_viewer` | OpenCV | Aligned RGB-D visualization with save, colormap, and 3D view controls |
| [beginner/depth_viewer](beginner/depth_viewer/README.md) | `ob_depth_viewer` | OpenCV | Depth-only visualization with raw depth and 3D rendering |
| [beginner/enumerate_control](beginner/enumerate_control/README.md) | `ob_enumerate_control` | None | Enumerate devices, sensors, stream profiles, and device properties |
| [beginner/record_playback](beginner/record_playback/README.md) | `ob_record_playback` | OpenCV | Record device data to `.bag` and replay it later |
| [beginner/hot_plugin](beginner/hot_plugin/README.md) | `ob_hot_plugin` | None | Handle device connect and disconnect events |
| [beginner/imu](beginner/imu/README.md) | `ob_imu_viewer` | None | Real-time IMU terminal visualization |

### Advanced examples

| Directory | Target | Requires | Description |
| --- | --- | --- | --- |
| [advanced/align_modes](advanced/align_modes/README.md) | `ob_align_modes` | OpenCV | Compare disabled, software, and hardware D2C alignment |
| [advanced/camera_params](advanced/camera_params/README.md) | `ob_camera_params` | None | Print intrinsics, extrinsics, distortion, and coordinate transforms |
| [advanced/infrared_streams](advanced/infrared_streams/README.md) | `ob_infrared_streams` | OpenCV | Display single or dual IR streams and control emitter state |
| [advanced/post_processing](advanced/post_processing/README.md) | `ob_post_processing` | OpenCV | Demonstrate recommended depth post-processing filters |
| [advanced/hdr](advanced/hdr/README.md) | `ob_hdr` | OpenCV | HDR merge sample for supported devices |
| [advanced/confidence](advanced/confidence/README.md) | `ob_confidence` | OpenCV | Show depth and confidence streams together |
| [advanced/callback](advanced/callback/README.md) | `ob_callback` | OpenCV | Process frames in a callback-based pipeline |
| [advanced/save_to_disk](advanced/save_to_disk/README.md) | `ob_save_to_disk` | OpenCV | Save synchronized color and depth frames to disk |
| [advanced/metadata](advanced/metadata/README.md) | `ob_metadata` | None | Print frame metadata values |
| [advanced/preset](advanced/preset/README.md) | `ob_preset` | None | Load and inspect device preset values |
| [advanced/forceip](advanced/forceip/README.md) | `ob_device_forceip` | None | Configure IP settings for supported network devices |
| [advanced/optional_depth_presets_update](advanced/optional_depth_presets_update/README.md) | `ob_device_optional_depth_presets_update` | None | Update optional depth presets on supported devices |
| [advanced/laser_interleave](advanced/laser_interleave/README.md) | `ob_laser_interleave` | OpenCV | Laser frame interleave sample for supported devices |
| [advanced/multi_devices_sync](advanced/multi_devices_sync/README.md) | `ob_multi_devices_sync` | OpenCV | Multi-device synchronization sample |
| [advanced/multi_devices_sync_gmsltrigger](advanced/multi_devices_sync_gmsltrigger/README.md) | `ob_multi_devices_sync_gmsltrigger` | Linux / GMSL | Send PWM trigger signals for GMSL synchronization |

### Application examples

| Directory | Target | Requires | Description |
| --- | --- | --- | --- |
| [application/3d_measurement](application/3d_measurement/README.md) | `ob_3d_measurement` | OpenCV | Click two pixels and measure real-world 3D distance |
| [application/depth_pcl](application/depth_pcl/README.md) | `ob_pcl`, `ob_pcl_color` | PCL | Point cloud generation and PCL visualization examples |
| [application/rgbd_open3d](application/rgbd_open3d/README.md) | `ob_open3d` | Open3D | Real-time RGB-D visualization with Open3D |

### C API examples

| Directory | Target | Requires | Description |
| --- | --- | --- | --- |
| [c_examples/0.c_quick_start](c_examples/0.c_quick_start/readme.md) | `ob_quick_start_c` | None | Minimal C API stream start example |
| [c_examples/1.c_enumerate](c_examples/1.c_enumerate/README.md) | `ob_enumerate_c` | None | Enumerate devices, sensors, and stream profiles with the C API |
| [c_examples/2.c_depth](c_examples/2.c_depth/README.md) | `ob_depth_c` | None | Read depth frames and print center distance in C |

### LiDAR examples

See [LiDAR_README.md](LiDAR_README.md) for the LiDAR-specific guide and build targets.

## Notes

- Use `cmake --build build --config Release --target <target>` if you only want to build one sample.
- Some advanced samples require a specific device family or capability. Always check the sample README before running.
- If an OpenCV-dependent target is missing, verify that OpenCV is installed and that `OpenCV_DIR` points to a valid `OpenCVConfig.cmake`.
- For bag recording examples, always exit with the key described in the README so the file is flushed correctly.
