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
├── beginner/               # 入门级示例，新手从这里开始
│   ├── quick_start/        # Pipeline 基础 — 终端打印深度数据 (no OpenCV)
│   ├── rgbd_viewer/        # RGB-D 可视化 + D2C 对齐 + 保存 (OpenCV)
│   ├── depth_viewer/       # 深度流 + colormap + 3D 渲染 + 保存 (OpenCV)
│   ├── enumerate_control/  # 设备枚举 + 属性控制 CLI (no OpenCV)
│   ├── record_playback/    # 录制/回放 .bag 二合一 (OpenCV)
│   ├── hot_plugin/         # 设备热插拔事件处理 (no OpenCV)
│   └── imu/                # IMU 六轴 ASCII 可视化 (no OpenCV)
├── advanced/               # 进阶示例，深入 SDK 功能
│   ├── align_modes/        # HW vs SW 深度对齐对比 (OpenCV)
│   ├── camera_params/      # 内外参获取 + 坐标变换 (no OpenCV)
│   ├── infrared_streams/   # 单/双目 IR + emitter 控制 (OpenCV)
│   └── ... (其他 advanced 示例)
├── application/            # 应用级示例
│   └── 3d_measurement/     # 双击测距 (OpenCV)
├── c_examples/             # C 语言 API 示例
├── lidar_examples/         # LiDAR 专用示例
└── utils/                  # 共享工具库
```

## Level 1 — Beginner

| Example | OpenCV | Description |
|---------|--------|-------------|
| [quick_start](beginner/quick_start) | ✗ | Pipeline 最小示例 — 终端打印中心点深度值 |
| [rgbd_viewer](beginner/rgbd_viewer) | ✓ | RGB-D 可视化，D2C Align，S 保存，M 3D 渲染，C 切换 colormap |
| [depth_viewer](beginner/depth_viewer) | ✓ | 深度流可视化，M 3D 渲染，S 保存 16-bit PNG，D 原始深度，C colormap |
| [enumerate_control](beginner/enumerate_control) | ✗ | 设备/传感器/StreamProfile 枚举 + 属性 get/set CLI 菜单 |
| [record_playback](beginner/record_playback) | ✓ | 录制所有流到 .bag / 回放 .bag 文件，支持暂停/恢复 |
| [hot_plugin](beginner/hot_plugin) | ✗ | 设备热插拔回调，R 重启设备触发断连重连 |
| [imu](beginner/imu) | ✗ | Accel/Gyro 六轴 ASCII 横条图 + 倾斜角估算 |

## Level 2 — Advanced

### Depth Processing & Alignment

| Example | OpenCV | Description |
|---------|--------|-------------|
| [align_modes](advanced/align_modes) | ✓ | HW / SW / Disable 三种 D2C 对齐模式切换对比 |
| [camera_params](advanced/camera_params) | ✗ | 内参/畸变/外参打印 + 2D⇄3D 坐标变换演示 |
| [infrared_streams](advanced/infrared_streams) | ✓ | 单/双目 IR 显示，E 切换 emitter，S 保存 |
| [post_processing](advanced/post_processing) | ✓ | 深度后处理滤波器 |
| [hdr](advanced/hdr) | ✓ | 深度 HDR 合并 |
| [confidence](advanced/confidence) | ✓ | 深度 + 置信度流显示 |
| [preset](advanced/preset) | ✗ | 深度预设值 |

### Recording & Streaming

| Example | OpenCV | Description |
|---------|--------|-------------|
| [callback](advanced/callback) | ✓ | 帧回调自定义处理 |
| [laser_interleave](advanced/laser_interleave) | ✓ | 激光交错模式 |

### Device & System

| Example | OpenCV | Description |
|---------|--------|-------------|
| [forceip](advanced/forceip) | ✗ | GigE 设备 IP 配置 |
| [optional_depth_presets_update](advanced/optional_depth_presets_update) | ✗ | 深度预设值更新 |
| [metadata](advanced/metadata) | ✗ | 逐帧 metadata 读取 |
| [save_to_disk](advanced/save_to_disk) | ✓ | 保存色彩/深度帧为图像 |

### Multi-Device

| Example | OpenCV | Description |
|---------|--------|-------------|
| [multi_devices_sync](advanced/multi_devices_sync) | ✓ | 多设备硬件同步 |
| [multi_devices_sync_gmsltrigger](advanced/multi_devices_sync_gmsltrigger) | ✓ | GMSL PWM 触发同步 (Linux) |

### Wrapper Integration

| Example | Requires | Description |
|---------|----------|-------------|
| [wrapper_pcl](advanced/wrapper_pcl) | PCL | PCL 点云可视化 |
| [wrapper_open3d](advanced/wrapper_open3d) | Open3D | Open3D 集成 |

## Level 3 — Application

| Example | OpenCV | Description |
|---------|--------|-------------|
| [3d_measurement](application/3d_measurement) | ✓ | 双击选点 → 3D 距离测量 |

## Specialized

- **LiDAR** → [LiDAR_README.md](LiDAR_README.md)
- **C API** → [c_examples/](c_examples/)

## Learning Path

1. **Start** → [quick_start](beginner/quick_start) — 最小可运行示例
2. **See RGB-D** → [rgbd_viewer](beginner/rgbd_viewer) — 可视化 + D2C 对齐
3. **Depth deep-dive** → [depth_viewer](beginner/depth_viewer) — colormap, 3D, 保存
4. **Know your device** → [enumerate_control](beginner/enumerate_control) — 枚举 + 属性
5. **Camera math** → [camera_params](advanced/camera_params) — 内外参与坐标变换
6. **Alignment** → [align_modes](advanced/align_modes) — HW vs SW 对齐
7. **Depth quality** → [post_processing](advanced/post_processing), [hdr](advanced/hdr)
8. **Multi-camera** → [multi_devices_sync](advanced/multi_devices_sync)
9. **Record** → [record_playback](beginner/record_playback) — 录制 + 回放
10. **Application** → [3d_measurement](application/3d_measurement) — 真实测距应用

## Utilities

[utils/](utils/) 目录提供所有示例共享的工具：

- **utils.hpp/cpp** — 按键、时间戳、设备类型判断
- **utils_opencv.hpp/cpp** — `CVWindow` (5 种布局模式), `renderDepth3D()` 3D 深度渲染
- **utils_c.h/c** — C 语言工具函数

## Error Handling

All examples use a `try/catch` block wrapping `main()`. SDK errors are reported via `ob::Error` exceptions with:
- `getFunction()` — Function where the error occurred
- `getArgs()` — Arguments at time of error
- `what()` — Error description
- `getExceptionType()` — Error category (see `OBExceptionType` in `ObTypes.h`)
