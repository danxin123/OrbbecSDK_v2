# Plan: Reorganize C++ Examples & Add 3D Depth Rendering to Quick Start

## TL;DR
Reorganize C++ examples from the current flat numeric-prefix structure (`0.basic.xxx`, `1.stream.xxx`, ...) into a `beginner/advanced/applications` three-tier structure matching pyorbbecsdk, and add 3D depth relief rendering to the quick_start example via Scharr-gradient lighting + colormap toggle.

## Phase 1: Restructure Examples Directory

### Steps

1. Create new top-level directories under `examples/`:
   - `beginner/` — numbered tutorials 01–09, designed for sequential learning
   - `advanced/` — single-feature deep-dive scripts grouped by sub-category
   - `applications/` — complete multi-feature applications (future)
   - Keep `lidar_examples/`, `c_examples/`, `utils/`, `publish_files/` unchanged

2. Map current examples to new structure:

   **beginner/** (sequential numbered tutorials):
   | New Name | Old Source | Topic |
   |----------|-----------|-------|
   | 01_quick_start | 0.basic.quick_start | Pipeline basics, first frame display |
   | 02_depth_viewer | 1.stream.depth | Depth stream, 2D/3D visualization, colormaps |
   | 03_color_and_depth | 1.stream.color + 3.advanced.sync_align | Color+depth, alignment |
   | 04_enumerate | 0.basic.enumerate | Device discovery, stream profiles |
   | 05_point_cloud | 3.advanced.point_cloud | PointCloudFilter, save PLY |
   | 06_multi_streams | 1.stream.multi_streams | All streams simultaneously |
   | 07_imu | 1.stream.imu | ACCEL + GYRO data |
   | 08_infrared | 1.stream.infrared | IR stream display |
   | 09_firmware_update | 2.device.firmware_update | OTA firmware upgrade |

   **advanced/** (sub-grouped):
   - Recording & Playback: record, record_nogui, playback
   - Device & System: control, hot_plugin, enumerate (detailed)
   - Depth Processing: post_processing, hdr, preset, confidence, decimation, hw_d2c_align, coordinate_transform
   - Multi-Device: multi_devices, multi_devices_sync, multi_devices_sync_gmsltrigger
   - Network: forceip
   - Misc: logger, metadata, save_to_disk, common_usages, laser_interleave, optional_depth_presets_update

   **Wrapper examples** (`5.wrapper.*` → `wrappers/`):
   - opencv, pcl, open3d (keep as-is or move to wrappers/)

3. Update `examples/CMakeLists.txt` to reference new subdirectory paths

4. Update each example's `CMakeLists.txt` if project names change

5. Update `examples/README.md` to match pyorbbecsdk format:
   - Quick Start section
   - Level 1 — Beginner (numbered table with descriptions)
   - Level 2 — Advanced (grouped sub-tables)
   - Level 3 — Applications
   - Specialized — LiDAR
   - Utilities reference
   - Learning Path guide

## Phase 2: Add 3D Depth Rendering to quick_start

### Steps

6. **Modify `quick_start.cpp`** — Add 3D depth relief rendering:
   - After `pipe.waitForFrameset()`, extract depth frame
   - Convert raw uint16 depth to float mm
   - Clip to fixed range [200, 5000] mm
   - Normalize with gamma correction (γ=0.8)
   - Compute Scharr gradients (grad_x, grad_y) on normalized depth
   - Compute diffuse lighting: `lighting = -0.707*(grad_x+grad_y)/magnitude`
   - Apply colormap (JET default, toggle with 'C')
   - Multiply colormap by lighting for 3D relief effect
   - Overlay center-point distance text
   - Add 'M' key toggle between 2D flat / 3D relief modes
   - Keep the existing CVWindow-based display as default; add optional raw OpenCV rendering path

7. **Alternative approach** (simpler, recommended): Add 3D rendering as a utility function in `utils_opencv` and use it in quick_start
   - Add `render_depth_3d()` function to `utils_opencv.hpp/cpp`
   - This is reusable across multiple examples
   - quick_start calls it to display depth with 3D relief alongside color

### Key Implementation Details (from pyorbbecsdk reference):
```cpp
// Scharr gradient → surface-normal lighting:
cv::Mat grad_x, grad_y;
cv::Scharr(depth_8bit, grad_x, CV_32F, 1, 0);
cv::Scharr(depth_8bit, grad_y, CV_32F, 0, 1);
cv::Mat mag;
cv::magnitude(grad_x, grad_y, mag);
mag += 1.0f;
cv::Mat lighting = -0.707f * (grad_x + grad_y) / mag;
lighting = lighting * 0.15f + 0.85f;
cv::min(cv::max(lighting, 0.7f), 1.0f, lighting);
cv::Mat colored;
cv::applyColorMap(depth_8bit, colored, cv::COLORMAP_JET);
// Multiply color by lighting (broadcast over 3 channels)
std::vector<cv::Mat> channels(3);
cv::split(colored, channels);
for (auto& ch : channels) {
    ch.convertTo(ch, CV_32F);
    ch = ch.mul(lighting);
    ch.convertTo(ch, CV_8U);
}
cv::merge(channels, result);
```

## Phase 3: Sync All Documentation Referencing Example Paths

### Steps

8. **`docs/tutorial/orbbecsdkv1_to_openorbbecsdkv2.md`** — KEY FILE, 8 path refs:
   - `examples/2.device.record` → `examples/advanced/record`
   - `examples/2.device.playback` → `examples/advanced/playback`
   - `examples/2.device.firmware_update` → `examples/beginner/09_firmware_update`
   - `examples/3.advanced.hdr` → `examples/advanced/hdr`
   - `examples/3.advanced.laser_interleave` → `examples/advanced/laser_interleave`
   - QuickStart ref → `examples/beginner/01_quick_start`

9. **`docs/tutorial/installation_and_development_guide.md`** — examples/README.md link (keep)

10. **`docs/tutorial/building_orbbec_sdk.md`** — examples/README.md link (keep)

11. **`docs/api-example-doc-test-matrix.md`** — update scan path description

12. **`README.md` (root)** — Quick Start snippet & examples link

13. **`LiDAR_README.md`** — verify lidar example refs unchanged

14. **`examples/README.md`** — FULL REWRITE to 3-tier format

15. **`examples/CMakeLists.txt`** — update all 30+ add_subdirectory() calls

16. **`CMakeLists.txt` (root)** — verify install paths still correct

17. **37 individual `examples/*/README.md`** — update cross-references

18. **`scripts/run_examples.sh`** — update if hardcoded example paths exist

19. **`.github/workflows/examples-smoke.yml`** — verify indirect refs

## Relevant Files (Complete List)

**Core code changes:**
- `examples/0.basic.quick_start/quick_start.cpp` — add 3D rendering
- `examples/utils/utils_opencv.hpp` — add renderDepth3D()
- `examples/utils/utils_opencv.cpp` — implement 3D rendering

**Build system:**
- `examples/CMakeLists.txt` — all add_subdirectory() calls
- `CMakeLists.txt` (root) — install paths
- All `examples/*/CMakeLists.txt` — project names & paths

**Documentation to update:**
- `docs/tutorial/orbbecsdkv1_to_openorbbecsdkv2.md` — 8 example path refs
- `docs/tutorial/installation_and_development_guide.md` — example link
- `docs/tutorial/building_orbbec_sdk.md` — example link
- `docs/api-example-doc-test-matrix.md` — scan description
- `README.md` (root) — Quick Start & examples link
- `LiDAR_README.md` — verify lidar refs
- `examples/README.md` — full rewrite
- `examples/LiDAR_README.md` — verify refs
- 37x `examples/*/README.md` — cross-references

**CI/scripts:**
- `scripts/run_examples.sh` — if hardcoded paths
- `.github/workflows/examples-smoke.yml` — indirect refs

## Verification

1. Linux build: `cmake -DOB_BUILD_EXAMPLES=ON && make -j`
2. Windows build: `cmake --build build --config Release`
3. Run quick_start — verify 3D depth rendering, M key toggle, C key colormap
4. CI examples-smoke workflow passes
5. **Doc link check**: run `scripts/run_docs_checks.sh` to validate all internal links
6. **Manual check**: verify all 8 refs in `orbbecsdkv1_to_openorbbecsdkv2.md` updated
7. **Grep old paths**: search entire project for `0.basic.`, `1.stream.`, `2.device.`, `3.advanced.`, `4.misc.`, `5.wrapper.` to confirm no stale refs

## Decisions

- Follow pyorbbecsdk `beginner/advanced/applications` 3-tier model
- 3D rendering as shared utility in utils_opencv (reusable)
- C examples (`c_examples/`) and LiDAR examples unchanged
- CMake target names stay simple (e.g. `ob_quick_start`)
- Implementation order: Phase 2 (3D rendering) → Phase 1 (restructure) → Phase 3 (docs sync)

## Further Considerations

1. **Backward compat**: Old names in external docs — add rename note in README, no symlinks
2. **CVWindow integration**: Add custom render callback in CVWindow for 3D depth display
3. **Phased rollout**: Phase 2 first (low risk), then Phase 1+3 together (atomic directory rename + doc sync)
