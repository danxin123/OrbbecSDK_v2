# OrbbecSDK v2 Examples 目录重构计划

## TL;DR

重构 examples/ 目录，降低入门门槛、面向场景组织、合并重复示例、补全 API 覆盖。核心策略：quick_start 零依赖化、rgbd 拆为 viewer + align_modes + camera_params 三层、新增 3d_measurement 应用示例、被移除的示例归档到 archive/ 而非删除。

---

## 一、设计原则

1. **单一概念原则** — 每个示例只教一个核心概念，beginner 示例 5 分钟可理解
2. **渐进依赖原则** — beginner 尽量零外部依赖 → advanced 可选 OpenCV → application 可选 Open3D/PCL
3. **API 全覆盖** — Pipeline同步/回调、Sensor回调、Filter链、属性控制、帧元数据等核心 pattern 都保留
4. **归档而非删除** — 被移除的示例移至 archive/，保留代码但不在主目录露出
5. **受众全覆盖** — 机器人/SLAM、3D视觉、AI/ML、SDK评估者

---

## 二、新目录结构

```
examples/
├── beginner/                     # 入门级：0-1个外部依赖
│   ├── quick_start/              # [P0 重写] 纯CLI，零依赖
│   ├── depth_viewer/             # [P1 改造] 深度流 + 3D渲染 + 保存PNG
│   ├── rgbd_viewer/              # [P0 新写] RGBD流 + 软件Align + 显示
│   ├── enumerate_control/        # [P1 合并] 设备枚举 + 属性控制
│   ├── point_cloud/              # [P2 微调] 保留PLY保存
│   ├── hot_plugin/               # [P2 移动] 从advanced移入
│   ├── imu/                      # [P2 增强] 保留！加终端可视化
│   └── record_playback/          # [P1 合并] 录制 + 回放
├── advanced/                     # 进阶级：深入单个SDK特性
│   ├── align_modes/              # [P1 新写] HW vs SW Align对比切换
│   ├── camera_params/            # [P1 新写] 内参/畸变/外参 + 2D↔3D投影
│   ├── callback/                 # [保留] Pipeline + Sensor回调模式
│   ├── post_processing/          # [保留] Filter链演示
│   ├── hdr/                      # [保留] HDR深度合帧
│   ├── metadata/                 # [保留] 帧元数据分析
│   ├── multi_devices_sync/       # [保留] 多设备硬件同步
│   ├── infrared_streams/         # [P2 改造] 双目IR展示（原08_infrared增强）
│   ├── laser_interleave/         # [保留] 激光交替模式
│   ├── save_to_disk/             # [保留] 保存深度/彩色到磁盘
│   ├── forceip/                  # [保留] 网络设备IP配置
│   └── preset/                   # [保留] 设备预设管理
├── application/                  # 应用级：端到端场景演示
│   ├── 3d_measurement/           # [P0 新写] RGB图鼠标两点测距
│   ├── rgbd_open3d/              # [P2 改造] RGBD + Open3D集成
│   └── depth_pcl/                # [P2 重命名] PCL点云可视化
├── archive/                      # 归档：不再主动推荐但保留代码
│   ├── color_viewer/             # 功能已入 rgbd_viewer
│   ├── common_usages/            # 过于庞杂，按需拆解到其他示例
│   ├── coordinate_transform/     # 功能已入 camera_params
│   ├── decimation/               # 特定设备功能
│   ├── hw_d2c_align/             # 功能已入 align_modes
│   ├── sync_align/               # 功能已入 rgbd_viewer + align_modes
│   ├── logger/                   # 低使用频率
│   ├── multi_devices/            # 被 multi_devices_sync 替代
│   ├── multi_streams/            # 功能分散到各 viewer
│   ├── record_nogui/             # 合入 record_playback
│   ├── wrapper_opencv/           # 与 utils_opencv 重叠
│   ├── firmware_update/          # 工具属性而非示例
│   └── multi_devices_firmware_update/  # 同上
├── c_examples/                   # [不变] C语言示例
├── lidar_examples/               # [不变] LiDAR专用示例
├── utils/                        # [不变] 共享工具库
└── publish_files/                # [P2 同步更新] 独立编译入口
```

---

## 三、各示例详细方案

### Phase 1 — P0 核心示例（必须首先完成）

#### 1. `beginner/quick_start/` — 重写（纯CLI，零依赖）

- **目标**：SDK 的 Hello World，零外部依赖，终端即可运行
- **当前问题**：依赖 CVWindow + renderDepth3D（OpenCV），新用户无法直接编译
- **实现要点**：
  - 使用 `ob::Pipeline` 默认配置启动
  - `pipe.waitForFrameset()` 获取帧集
  - 打印中心点深度值（`depthFrame->getData()` 读取 `CV_16UC1` 像素 × `getValueScale()`）
  - 打印帧基本信息（分辨率、帧率、时间戳）
  - 按 ESC 退出（使用 `ob_smpl::waitForKeyPressed()`）
  - 总代码量 ~50 行
- **依赖**：仅 `ob::examples::utils`（不含 utils_opencv）
- **CMakeLists**：不需要 `find_package(OpenCV)`
- **参考代码**：`examples/advanced/metadata/metadata.cpp`（纯CLI模式）

#### 2. `beginner/rgbd_viewer/` — 新写（RGBD流获取 + 显示）

- **目标**：展示如何获取对齐的 RGBD 流并显示，是后续所有 RGBD 应用的基础
- **核心概念**：仅限「获取 RGBD + 软件 Align + 可视化」，**不含** HW Align、不含内外参获取
- **实现要点**：
  - `ob::Config` 启用 COLOR + DEPTH 流
  - `config->setFrameAggregateOutputMode(OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE)`
  - 创建 `ob::Align(OB_STREAM_COLOR)` 做软件 D2C
  - 用 CVWindow OVERLAY 模式显示对齐后的 RGBD
  - 按键 S 保存当前帧 depth.png + color.png（参考 `save_to_disk.cpp` 的 `saveDepthFrame()` / `saveColorFrame()`）
  - 按键 M 切换 3D 深度渲染（调用 `ob_smpl::renderDepth3D()`）
- **依赖**：`ob::examples::utils`（含 OpenCV）
- **参考代码**：
  - `examples/advanced/sync_align/sync_align.cpp` — Align + Pipeline 模式
  - `examples/advanced/save_to_disk/save_to_disk.cpp` — 保存逻辑
  - `examples/beginner/01_quick_start/quick_start.cpp` — CVWindow 用法

#### 3. `application/3d_measurement/` — 新写（RGB图两点测距）

- **目标**：最直观的 SDK 价值展示 — 在 RGB 图上点两个点，测量真实 3D 距离
- **核心概念**：Aligned RGBD + 2D→3D 反投影 + 欧几里得距离
- **实现要点**：
  - 获取对齐的 RGBD 流（复用 rgbd_viewer 的模式）
  - `cv::setMouseCallback()` 记录两个点击点坐标 (u1,v1) (u2,v2)
  - 从 aligned depth 帧读取两点深度值 z1, z2
  - 通过深度内参 `getIntrinsic()` 反投影到 3D
  - 计算欧几里得距离
  - 用 `cv::line()` + `cv::putText()` 在图上标注线段和距离
  - 或使用 SDK 内置 `ob::CoordinateTransformHelper::transformation2dto3d()` 替代手动计算
- **依赖**：`ob::examples::utils`（含 OpenCV）
- **参考代码**：
  - `examples/advanced/coordinate_transform/coordinate_transform.cpp` — `transformation2dto3d()` 用法
  - `include/libobsensor/hpp/Utils.hpp` — `CoordinateTransformHelper` API

---

### Phase 2 — P1 改造与合并

#### 4. `beginner/depth_viewer/` — 改造

- **基于**：`examples/beginner/02_depth_viewer/depth.cpp`
- **增加功能**：
  - 按键 M 切换 3D 渲染（集成 `renderDepth3D()`）
  - 按键 D 演示 `cv::Mat` 读取深度帧：`cv::Mat rawMat(height, width, CV_16UC1, depthFrame->getData())`
  - 按键 S 保存 depth.png（`cv::imwrite("depth.png", depthMat, {cv::IMWRITE_PNG_COMPRESSION, 0})`）
- **参考**：`save_to_disk.cpp` 中的 `saveDepthFrame()`

#### 5. `beginner/enumerate_control/` — 合并

- **来源**：`examples/beginner/04_enumerate/enumerate.cpp` + `examples/advanced/control/device_control.cpp`
- **实现**：CLI 菜单导航，第一层列出设备/传感器/StreamProfile，第二层进入属性控制
- **不依赖 OpenCV**

#### 6. `advanced/align_modes/` — 新写（从 rgbd_stream 拆出）

- **目标**：对比演示 HW Align vs SW Align
- **实现要点**：
  - 启动时检测设备是否支持 HW D2C（参考 `hw_d2c_align.cpp` 的 `checkIfSupportHWD2CAlign()`）
  - 按键 T 切换 HW / SW / Disable 三种模式
  - 显示当前 Align 模式 + 实时帧率，方便用户感受性能差异
  - HW 模式用 `config->setAlignMode(ALIGN_D2C_HW_MODE)` + pipeline restart
  - SW 模式用 `ob::Align(OB_STREAM_COLOR)` filter
- **参考代码**：
  - `examples/advanced/hw_d2c_align/hw_d2c_align.cpp` — HW Align 全流程
  - `examples/advanced/sync_align/sync_align.cpp` — SW Align 全流程

#### 7. `advanced/camera_params/` — 新写（替代 coordinate_transform）

- **目标**：展示如何获取和使用相机内外参
- **实现要点**：
  - 获取 depth/color StreamProfile
  - 打印 `getIntrinsic()` (fx,fy,cx,cy)、`getDistortion()` (k1-k6,p1,p2)、`getExtrinsicTo()` (R,T)
  - 演示 2D→3D、3D→2D、Depth2D→Color2D 四种变换（精简 coordinate_transform.cpp 的逻辑）
  - 不依赖 OpenCV（纯 CLI）
- **参考代码**：`examples/advanced/coordinate_transform/coordinate_transform.cpp`

#### 8. `beginner/record_playback/` — 合并

- **来源**：`examples/advanced/record/device_record.cpp` + `examples/advanced/playback/device_playback.cpp`
- **实现**：CLI 菜单 — 1)录制 2)回放 3)退出
- **录制部分**：参考 `record_nogui` 的纯 CLI 方式，但保留 OpenCV 可视化为可选
- **回放部分**：直接复用 `device_playback.cpp` 逻辑

---

### Phase 3 — P2 迁移、增强与收尾

#### 9. `beginner/hot_plugin/` — 直接移动
- 从 `advanced/hot_plugin/` 复制到 `beginner/hot_plugin/`，代码无需修改

#### 10. `beginner/imu/` — 保留并增强
- 基于现有 `07_imu/imu.cpp`
- 增加简单的终端 ASCII 可视化（加速度/角速度实时显示）
- 不依赖 OpenCV

#### 11. `advanced/infrared_streams/` — 改造
- 基于 `08_infrared/infrared.cpp`
- 增加同时显示 IR_LEFT + IR_RIGHT 双目红外流
- 展示结构光 pattern 可见性

#### 12. `application/rgbd_open3d/` — 改造
- 基于 `advanced/wrapper_open3d/open3d.cpp`
- 重命名 + 集成 RGBD 获取（而非仅深度点云）

#### 13. `application/depth_pcl/` — 重命名
- 将 `advanced/wrapper_pcl/` 重命名为 `application/depth_pcl/`

#### 14. 目录迁移与归档
- 将被移除的示例移至 `archive/` 目录
- 每个归档示例保留原始 CMakeLists.txt 和源码
- archive/ 目录有独立 CMakeLists.txt，默认不编译（需手动 `OB_BUILD_ARCHIVE_EXAMPLES=ON`）

#### 15. 顶层 CMakeLists.txt 重写
- 按新目录结构重新组织 `add_subdirectory()` 调用
- 新增 `archive/` 条件编译
- 保持 c_examples/ 和 lidar_examples/ 不变

#### 16. `publish_files/` 同步更新
- 更新 `publish_files/CMakeLists.txt` 适配新目录结构
- 确保 SDK 安装后独立编译示例正常工作

#### 17. README 更新
- 重写 `examples/README.md` 适配新结构
- 更新 Learning Path 路径
- 新增 archive/ 说明

---

## 四、关键文件清单

### 新建文件
| 文件 | 说明 |
|------|------|
| `examples/beginner/quick_start/quick_start.cpp` | 纯CLI快速启动（重写） |
| `examples/beginner/quick_start/CMakeLists.txt` | 无OpenCV依赖 |
| `examples/beginner/rgbd_viewer/rgbd_viewer.cpp` | RGBD流获取+显示 |
| `examples/beginner/rgbd_viewer/CMakeLists.txt` | OpenCV依赖 |
| `examples/beginner/enumerate_control/enumerate_control.cpp` | 合并枚举+控制 |
| `examples/beginner/enumerate_control/CMakeLists.txt` | 无OpenCV依赖 |
| `examples/beginner/record_playback/record_playback.cpp` | 合并录制+回放 |
| `examples/beginner/record_playback/CMakeLists.txt` | OpenCV可选 |
| `examples/advanced/align_modes/align_modes.cpp` | HW/SW Align对比 |
| `examples/advanced/align_modes/CMakeLists.txt` | OpenCV依赖 |
| `examples/advanced/camera_params/camera_params.cpp` | 内外参获取+坐标变换 |
| `examples/advanced/camera_params/CMakeLists.txt` | 无OpenCV依赖 |
| `examples/application/3d_measurement/3d_measurement.cpp` | 两点测距 |
| `examples/application/3d_measurement/CMakeLists.txt` | OpenCV依赖 |
| `examples/archive/CMakeLists.txt` | 归档示例统一入口 |

### 修改文件
| 文件 | 修改内容 |
|------|----------|
| `examples/CMakeLists.txt` | 重写：适配新目录结构 |
| `examples/beginner/02_depth_viewer/depth.cpp` | 增加3D渲染+保存功能 |
| `examples/beginner/07_imu/imu.cpp` | 增加终端ASCII可视化 |
| `examples/beginner/08_infrared/infrared.cpp` | 增强为双目IR显示 |
| `examples/publish_files/CMakeLists.txt` | 适配新目录结构 |
| `examples/README.md` | 完全重写 |

### 移动/重命名
| 原路径 | 新路径 |
|--------|--------|
| `examples/advanced/hot_plugin/` | `examples/beginner/hot_plugin/` |
| `examples/advanced/wrapper_pcl/` | `examples/application/depth_pcl/` |
| `examples/advanced/wrapper_open3d/` | `examples/application/rgbd_open3d/` |
| `examples/beginner/03_color_viewer/` | `examples/archive/color_viewer/` |
| `examples/beginner/06_multi_streams/` | `examples/archive/multi_streams/` |
| `examples/beginner/09_firmware_update/` | `examples/archive/firmware_update/` |
| `examples/advanced/common_usages/` | `examples/archive/common_usages/` |
| `examples/advanced/coordinate_transform/` | `examples/archive/coordinate_transform/` |
| `examples/advanced/decimation/` | `examples/archive/decimation/` |
| `examples/advanced/hw_d2c_align/` | `examples/archive/hw_d2c_align/` |
| `examples/advanced/sync_align/` | `examples/archive/sync_align/` |
| `examples/advanced/logger/` | `examples/archive/logger/` |
| `examples/advanced/multi_devices/` | `examples/archive/multi_devices/` |
| `examples/advanced/record_nogui/` | `examples/archive/record_nogui/` |
| `examples/advanced/wrapper_opencv/` | `examples/archive/wrapper_opencv/` |
| `examples/advanced/multi_devices_firmware_update/` | `examples/archive/multi_devices_firmware_update/` |

---

## 五、CMakeLists.txt 顶层结构草案

```cmake
cmake_minimum_required(VERSION 3.10)

find_package(OpenCV QUIET)
if(NOT ${OpenCV_FOUND})
    message(WARNING "OpenCV not found, some examples will not be built.")
endif()

option(OB_BUILD_ARCHIVE_EXAMPLES "Build archived examples" OFF)

# Utils (always built)
add_subdirectory(utils)

# ============================================================
# Beginner — No OpenCV
# ============================================================
add_subdirectory(beginner/quick_start)
add_subdirectory(beginner/enumerate_control)
add_subdirectory(beginner/point_cloud)
add_subdirectory(beginner/hot_plugin)
add_subdirectory(beginner/imu)

# ============================================================
# Beginner — OpenCV Required
# ============================================================
if(${OpenCV_FOUND})
    add_subdirectory(beginner/depth_viewer)
    add_subdirectory(beginner/rgbd_viewer)
    add_subdirectory(beginner/record_playback)
endif()

# ============================================================
# Advanced — No OpenCV
# ============================================================
add_subdirectory(advanced/camera_params)
add_subdirectory(advanced/forceip)
add_subdirectory(advanced/preset)
add_subdirectory(advanced/metadata)

# ============================================================
# Advanced — OpenCV Required
# ============================================================
if(${OpenCV_FOUND})
    add_subdirectory(advanced/align_modes)
    add_subdirectory(advanced/callback)
    add_subdirectory(advanced/post_processing)
    add_subdirectory(advanced/hdr)
    add_subdirectory(advanced/multi_devices_sync)
    add_subdirectory(advanced/infrared_streams)
    add_subdirectory(advanced/laser_interleave)
    add_subdirectory(advanced/save_to_disk)
endif()

# ============================================================
# Application — Specialized dependencies
# ============================================================
if(${OpenCV_FOUND})
    add_subdirectory(application/3d_measurement)
endif()
if(OB_BUILD_PCL_EXAMPLES)
    add_subdirectory(application/depth_pcl)
endif()
if(OB_BUILD_OPEN3D_EXAMPLES)
    add_subdirectory(application/rgbd_open3d)
endif()

# ============================================================
# Archive — Opt-in
# ============================================================
if(OB_BUILD_ARCHIVE_EXAMPLES)
    add_subdirectory(archive)
endif()

# ============================================================
# C & LiDAR (unchanged)
# ============================================================
add_subdirectory(c_examples)
add_subdirectory(lidar_examples)

include(InstallRequiredSystemLibraries)
```

---

## 六、编号去除计划

所有 beginner 示例去除数字前缀（`01_quick_start` → `quick_start`），使目录名更稳定（新增/删除不影响编号）。

---

## 七、验证清单

### 编译验证
1. `cmake -S . -B build -DOB_BUILD_EXAMPLES=ON` — 无 OpenCV 时，所有无 OpenCV 示例编译通过
2. `cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=...` — OpenCV 示例全部编译通过
3. `cmake -S . -B build -DOB_BUILD_ARCHIVE_EXAMPLES=ON` — archive 示例可选编译通过
4. `cmake -S . -B build_publish -S examples/publish_files` — 独立编译模式工作正常

### 运行验证（需要设备）
5. `ob_quick_start` — 终端输出中心深度值，ESC退出
6. `ob_rgbd_viewer` — 显示对齐的RGBD，S保存PNG，M切换3D
7. `ob_3d_measurement` — 鼠标点两点显示距离
8. `ob_enumerate_control` — 设备枚举 + 属性交互
9. `ob_align_modes` — T键切换HW/SW/Off，帧率有变化

### 文档验证
10. `examples/README.md` 中所有链接有效
11. 每个新示例目录包含独立 README.md

---

## 八、优先级与执行顺序

| 优先级 | 阶段 | 任务 | 复杂度 | 依赖 |
|--------|------|------|--------|------|
| P0 | Phase 1 | quick_start 重写（纯CLI） | 低 | 无 |
| P0 | Phase 1 | rgbd_viewer 新写 | 中 | 无 |
| P0 | Phase 1 | 3d_measurement 新写 | 中 | rgbd_viewer 模式可参考 |
| P1 | Phase 2 | depth_viewer 改造 | 低 | 无 |
| P1 | Phase 2 | enumerate_control 合并 | 中 | 无 |
| P1 | Phase 2 | align_modes 新写 | 中 | 无 |
| P1 | Phase 2 | camera_params 新写 | 中 | 无 |
| P1 | Phase 2 | record_playback 合并 | 中 | 无 |
| P2 | Phase 3 | hot_plugin 移动 | 低 | 无 |
| P2 | Phase 3 | imu 增强 | 低 | 无 |
| P2 | Phase 3 | infrared_streams 改造 | 低 | 无 |
| P2 | Phase 3 | 目录迁移/归档 | 低 | Phase 1-2 完成后 |
| P2 | Phase 3 | 顶层 CMakeLists 重写 | 低 | 目录迁移完成后 |
| P2 | Phase 3 | publish_files 同步 | 低 | CMakeLists 完成后 |
| P2 | Phase 3 | README 更新 | 低 | 全部完成后 |

**建议执行路径**：P0 三个任务可并行开发 → P1 四个改造/合并可并行 → P2 顺序执行

---

## 九、决策记录

| 决策 | 理由 |
|------|------|
| 保留 IMU 示例 | 机器人/SLAM 用户核心需求，VIO/SLAM 必需 |
| 保留 callback 示例 | Pipeline 回调模式是实时系统（机器人控制循环、AI推理）的必备 pattern |
| 保留 metadata 示例 | 帧时间戳分析对数据采集和多传感器同步至关重要 |
| 保留 IR 示例（增强为双目） | 结构光原理展示 + 低光环境特征跟踪需求 |
| rgbd_stream 拆为三个示例 | 「单一概念原则」— beginner 只教获取流，advanced 教 Align 对比和参数获取 |
| point_cloud 不做 OpenCV 2D 投影 | 3D 点云投影到 2D 违反用户期望，保留 PLY 保存即可 |
| 归档而非删除 | 用户选择，降低信息丢失风险 |
| 去除数字前缀 | 目录名更稳定，增删示例不影响编号 |
| c_examples / lidar_examples 不变 | 受众独立，不需要混入 C++ 示例体系 |

---

## 十、已知风险

1. **OpenCV Debug/Release ABI 不匹配**：Miniconda 仅有 Release OpenCV，MSVC Debug 构建会触发 `debug_build_guard` LNK2019。需要在 `utils/CMakeLists.txt` 中对 `utils_opencv.cpp` 添加 `/U_DEBUG` 编译选项。
2. **HW Align 设备兼容性**：不是所有 Orbbec 设备支持硬件 D2C，`align_modes` 示例需要优雅处理不支持的情况。
3. **归档示例的维护成本**：archive/ 中的示例如果不持续编译验证，可能随 SDK API 变更而腐化。建议 CI 中定期编译 archive。
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
