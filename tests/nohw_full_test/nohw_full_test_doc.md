# nohw_full_test 综合文档

> **目的**: 汇总 `nohw_full_test.cpp` 现有全部 46 个测试用例 + `playback_test_plan.md` 规划的 33 个新用例，作为 review 和重构的统一参考。
>
> **文件**: `tests/nohw_full_test/nohw_full_test.cpp` (~1116 行)
> **基础设施**: `tests/test_common.hpp`
> **构建目标**: `ob_nohw_full_test`

---

## 目录

- [1. 测试基础设施](#1-测试基础设施)
- [2. 现有测试用例总览](#2-现有测试用例总览)
- [3. 各测试组详细说明](#3-各测试组详细说明)
  - [3.1 TC_CPP_01 Context](#31-tc_cpp_01-context)
  - [3.2 TC_CPP_02 Discovery](#32-tc_cpp_02-discovery)
  - [3.3 TC_CPP_10 Frame](#33-tc_cpp_10-frame)
  - [3.4 TC_CPP_11 Metadata](#34-tc_cpp_11-metadata)
  - [3.5 TC_CPP_12 FrameFactory](#35-tc_cpp_12-framefactory)
  - [3.6 TC_CPP_13 Filter](#36-tc_cpp_13-filter)
  - [3.7 TC_CPP_14 Property](#37-tc_cpp_14-property)
  - [3.8 TC_CPP_18 Playback (现有)](#38-tc_cpp_18-playback-现有)
  - [3.9 TC_CPP_20 CoordTransform](#39-tc_cpp_20-coordtransform)
  - [3.10 TC_CPP_22 Version](#310-tc_cpp_22-version)
  - [3.11 TC_CPP_23 Logger](#311-tc_cpp_23-logger)
  - [3.12 TC_CPP_24 Error](#312-tc_cpp_24-error)
  - [3.13 TC_CPP_25 DataStruct](#313-tc_cpp_25-datastruct)
- [4. 规划新增用例 (Playback 扩展)](#4-规划新增用例-playback-扩展)
  - [4.1 Phase 0: 基础设施](#41-phase-0-基础设施)
  - [4.2 Phase 1: 设备信息 (18_1x)](#42-phase-1-设备信息-18_1x)
  - [4.3 Phase 2: 帧数据 (18_2x)](#43-phase-2-帧数据-18_2x)
  - [4.4 Phase 3: 标定参数 (18_3x)](#44-phase-3-标定参数-18_3x)
  - [4.5 Phase 4: Filter (18_4x)](#45-phase-4-filter-18_4x)
  - [4.6 Phase 5: 私有 Filter (18_49~18_56)](#46-phase-5-私有-filter-18_4918_56)
  - [4.7 Phase 6: 坐标变换 (18_6x)](#47-phase-6-坐标变换-18_6x)
- [5. 重构建议](#5-重构建议)
- [6. 完整用例索引](#6-完整用例索引)

---

## 1. 测试基础设施

### 1.1 Fixture 体系

| Fixture | 基类 | 职责 | 使用者 |
|---------|------|------|--------|
| `SDKTestBase` | `::testing::Test` | 轻量基类，仅持有 `env_` 引用，不创建 Context | TC_CPP_10/11/12/13/18/20/22/23/24/25 |
| `ContextTest` | `SDKTestBase` | SetUp 创建 `ob::Context`，TearDown 释放 | TC_CPP_01/02/14 |
| `HardwareTest` | `SDKTestBase` | SetUp 检查 `hwAvailable_`，无硬件自动 GTEST_SKIP | hw_full_test 专用 |
| `TC_CPP_18_Playback` | `SDKTestBase` | SetUp 检查 `playbackBagPath` 非空，否则 GTEST_SKIP | 18_03 ~ 18_07 |
| `TC_CPP_18_PlaybackParam` *(规划)* | `::testing::TestWithParam<string>` | 参数化 bag 路径，每个 bag 跑一遍 | 18_10 ~ 18_51 |

### 1.2 TestEnvironment 单例

定义在 `test_common.hpp`，通过环境变量和本地文件发现初始化：

| 成员 | 类型 | 来源 | 用途 |
|------|------|------|------|
| `hwAvailable_` | bool | 硬编码 true（hw_full_test 通过 CI env 覆盖） | 跳过硬件测试 |
| `deviceType_` | string | `DEVICE_TYPE` env，默认 `"gemini-335"` | 设备类型过滤 |
| `bagPath_` | string | `PLAYBACK_BAG_PATH` env → 或 `findLocalPlaybackBag()` | Playback 测试路径 |
| `firmwarePath_` | string | `FIRMWARE_FILE_PATH` env → 或 `tests/resource/firmware/*.bin` | 固件升级测试 |
| `depthPresetPath_` | string | `DEPTH_PRESET_PATH` env → 或 `tests/resource/present/*.bin` | 深度预设测试 |
| `allowDestructive_` | bool | `ALLOW_DESTRUCTIVE_TESTS` env | 保护性跳过 |

**`findLocalPlaybackBag()`** 搜索路径优先级:
1. `tests/resource/rosbag/*.bag`
2. 向上回溯 4 层相对路径
3. Legacy `tests/rosbag/` 路径兼容

### 1.3 关键头文件

```
tests/test_common.hpp         — Fixture 基类 + TestEnvironment
include/libobsensor/ObSensor.hpp — SDK 入口
include/libobsensor/hpp/Device.hpp — PlaybackDevice
include/libobsensor/hpp/Filter.hpp — 15 种 Filter 子类
include/libobsensor/hpp/Frame.hpp  — Frame / VideoFrame / PointsFrame / FrameSet
include/libobsensor/hpp/StreamProfile.hpp — getIntrinsic / getDistortion / getExtrinsicTo
include/libobsensor/hpp/Utils.hpp  — CoordinateTransformHelper
include/libobsensor/h/ObTypes.h    — 全部枚举和结构体
```

---

## 2. 现有测试用例总览

| # | 测试组 | Fixture | 用例数 | 涉及 API 层 |
|---|--------|---------|--------|-------------|
| 1 | TC_CPP_01 Context | `ContextTest` | 6 | C++ 高层 + C 底层 |
| 2 | TC_CPP_02 Discovery | `ContextTest` | 2 | C++ 高层 |
| 3 | TC_CPP_10 Frame | `SDKTestBase` | 2 | C 底层 |
| 4 | TC_CPP_11 Metadata | `SDKTestBase` | 1 | C 底层 |
| 5 | TC_CPP_12 FrameFactory | `SDKTestBase` | 3 | C 底层 |
| 6 | TC_CPP_13 Filter | `SDKTestBase` | 6 | C 底层 + C++ 高层 |
| 7 | TC_CPP_14 Property | `ContextTest` | 1 | (stub) |
| 8 | TC_CPP_18 Playback | `TC_CPP_18_Playback` | 5 | C++ 高层(控制面) |
| 9 | TC_CPP_20 CoordTransform | `SDKTestBase` | 4 | C++ 高层 |
| 10 | TC_CPP_22 Version | `SDKTestBase` | 3 | C++ 高层 |
| 11 | TC_CPP_23 Logger | `SDKTestBase` | 5 | C 底层 |
| 12 | TC_CPP_24 Error | `SDKTestBase` | 5 | C 底层 + C++ 异常 |
| 13 | TC_CPP_25 DataStruct | `SDKTestBase` | 3 | 纯数据结构 |
| | **合计** | | **46** | |

---

## 3. 各测试组详细说明

### 3.1 TC_CPP_01 Context

**Fixture**: `ContextTest` (创建 `ob::Context`)

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 01_01 | `context_default_create_destroy` | Context 创建非空；`queryDeviceList()` 非空，count >= 0 |
| 01_02 | `context_config_path` | 空字符串路径可用；无效路径抛 `ob::Error` 或安全降级 |
| 01_03 | `context_repeated_create_destroy` | 循环 10 次创建/销毁 Context, 每次 queryDeviceList 成功 |
| 01_04 | `free_idle_memory` | `freeIdleMemory()` 前后 queryDeviceList 正常 |
| 01_05 | `uvc_backend` | 设置 AUTO/LIBUVC/V4L2 后端；无 USB PAL → compile/runtime GTEST_SKIP |
| 01_06 | `extension_plugin_directory` | `ob_set_extensions_directory("." / "" / 无效路径)` 不崩溃 |

**Review 要点**:
- 01_05 有完整的编译时/运行时双层 skip 保护 (`#if !defined(BUILD_USB_PAL)` + catch)
- 01_02 和 01_06 都测试了无效路径的健壮性，可考虑统一边界值策略
- 01_03 的 10 次循环覆盖了资源泄漏场景

---

### 3.2 TC_CPP_02 Discovery

**Fixture**: `ContextTest`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 02_02 | `net_device_enum_toggle` | `enableNetDeviceEnumeration(true/false/true)` 不抛异常 |
| 02_06 | `clock_sync` | `enableDeviceClockSync(0)` 不抛异常 |

**Review 要点**:
- 这两个用例仅验证"不崩溃"，没有断言行为效果
- 缺少 `setDeviceChangedCallback` 的覆盖

---

### 3.3 TC_CPP_10 Frame

**Fixture**: `SDKTestBase` (仅用 C API)

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 10_04 | `frame_ref_count` | `ob_frame_add_ref` → 双次 `ob_delete_frame` 不崩溃 |
| 10_13 | `frameset_push_frame` | 创建空 frameset → push depth frame → count >= 1 |

**Review 要点**:
- 全部使用 C API (`ob_create_frame`, `ob_frame_add_ref`)
- 缺少 C++ 包装层 `Frame` / `FrameSet` 类的直接覆盖
- Frame format 验证仅 Y16，可扩展到更多 format

---

### 3.4 TC_CPP_11 Metadata

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 11_06 | `update_metadata_c_api` | 创建 depth frame → `ob_frame_update_metadata(0xAB, 0xCD)` → `ob_frame_get_metadata` 验证字节一致 |

**Review 要点**:
- 仅测试 C API 的 metadata 读写
- 缺少 34 种 `OBFrameMetadataType` 的 `hasMetadata()` / `getMetadataValue()` 覆盖（规划在 18_23）

---

### 3.5 TC_CPP_12 FrameFactory

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 12_01 | `create_frame_and_video_frame` | `ob_create_frame(DEPTH, Y16, 1024)` → type/format/size 正确；`ob_create_video_frame(COLOR, RGB, 640, 480)` → width/height 正确 |
| 12_03 | `create_frame_from_buffer` | 外部 buffer(0x42) → `ob_create_frame_from_buffer` → data[0]==0x42；delete 触发 destroy callback |
| 12_04 | `create_empty_frameset` | 空 frameset count==0 → push depth → count>=1 |

**Review 要点**:
- 12_03 验证了自定义 buffer 的生命周期回调
- 缺少 `ob_create_frame_from_stream_profile` 的覆盖

---

### 3.6 TC_CPP_13 Filter

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 13_01 | `create_all_builtin_filters` | 7 个必选 filter 必须创建成功；8 个可选 private filter 创建失败只 log 不 fail |
| 13_02 | `create_invalid_filter` | `"TotallyBogusFilter"` → 必须返回 error + null |
| 13_05 | `filter_enable_disable` | DecimationFilter: enable→true, disable→false, enable→true |
| 13_06 | `filter_reset_and_config` | `getConfigSchema()`非空; `getConfigSchemaList()` count>0; 读/写 config value; `reset()` |
| 13_07 | `filter_type_check` | PointCloudFilter: `getName()`=="PointCloudFilter", `is<PointCloudFilter>()`==true, `as<PointCloudFilter>()`非空 |
| 13_17 | `private_filter` | `ob_create_private_filter("SomePrivateFilter", "invalid_key")` → error(预期) |

**必选 Filter 列表 (13_01)**:
```
DecimationFilter, ThresholdFilter, Align, FormatConverter, HDRMerge, PointCloudFilter, SequenceIdFilter
```

**可选 Private Filter 列表 (13_01)**:
```
SpatialAdvancedFilter, SpatialFastFilter, SpatialModerateFilter, TemporalFilter,
HoleFillingFilter, NoiseRemovalFilter, DisparityTransform, FalsePositiveFilter
```

**Review 要点**:
- 13_01 是 filter 枚举完备性的关键用例
- 13_06 验证了 Config Schema 的完整 CRUD 流程(get schema → read value → write value → reset)
- 缺少 **真实帧数据** 经过 filter 的 `process()` 效果验证（规划在 18_4x）
- 缺少 filter 特有 API (如 `setScaleValue`, `setValueRange`) 的覆盖（规划在 18_4x）

---

### 3.7 TC_CPP_14 Property

**Fixture**: `ContextTest`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 14_16 | `sdk_level_property` | **TODO stub** — 仅 `SUCCEED()`, 无实际逻辑 |

**Review 要点**:
- 需补充 SDK 级属性（不依赖硬件的属性）

---

### 3.8 TC_CPP_18 Playback (现有)

**Fixture**: `TC_CPP_18_Playback` (继承 `SDKTestBase`, SetUp 检查 bag path)

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 18_03 | `playback_device_create_and_play` | PlaybackDevice 创建非空; DeviceInfo 有 name; Pipeline start + waitForFrameset(5s) 获得帧 |
| 18_04 | `playback_duration_position` | `getDuration()` > 0; `getPosition()` >= 0 |
| 18_05 | `playback_seek_and_rate` | seek(duration/2) 不崩溃; setPlaybackRate(0.5/1.0/2.0) 不崩溃 |
| 18_06 | `playback_pause_resume_status` | Pipeline 运行 → pause → status==PAUSED → resume → status==PLAYING |
| 18_07 | `playback_status_callback` | 注册 callback → pause/resume → callback 次数 >= 1 |

**Review 要点**:
- 现有 5 个用例只覆盖 PlaybackDevice 的 **控制面**(创建/时长/seek/暂停/回调)
- **不验证**帧数据内容、设备信息字段、传感器列表、内外参、Filter 效果
- 18_03 的 pipeline 启用了 `OB_FRAME_AGGREGATE_OUTPUT_ANY_SITUATION`，这是参考模式
- 18_06/18_07 有 `sleep_for` 延迟，CI 上可能不稳定

---

### 3.9 TC_CPP_20 CoordTransform

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 20_01 | `3d_to_3d` | 单位外参 → dst==src; 平移外参 → dst == src + trans; 精度 1e-3 |
| 20_02 | `2d_depth_to_3d` | cx,cy 处像素 + depth=1000 → 3d 点 z≈1000, x≈0, y≈0 |
| 20_03 | `3d_to_2d` | 原点(0,0,1000) → 投影到 cx,cy; 精度 1.0px |
| 20_04 | `2d_to_2d` | 同内参+单位外参 → 输出≈输入; 精度 2.0px |

**Review 要点**:
- 全部使用合成的 intrinsic/extrinsic，不涉及真实设备参数
- 精度要求较宽松 (1e-3 ~ 2.0)
- 规划 18_50/18_51 用 playback 真实参数做对照

---

### 3.10 TC_CPP_22 Version

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 22_01 | `full_version` | `getVersion()` > 0 |
| 22_02 | `major_minor_patch` | major/minor/patch >= 0; `major*10000 + minor*100 + patch == getVersion()` |
| 22_03 | `stage_version` | stage 非空时必须含 alpha/beta/rc/release 子串 |

---

### 3.11 TC_CPP_23 Logger

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 23_01 | `log_severity_set` | 遍历 DEBUG/INFO/WARN/ERROR/FATAL/OFF 六级，`ob_set_logger_severity` 不出错 |
| 23_02 | `log_to_file` | 设置文件日志 → 创建 Context + queryDeviceList → 验证日志文件存在且非空 |
| 23_03 | `log_to_console` | `ob_set_logger_to_console(INFO)` 不出错 |
| 23_04 | `log_callback` | 注册 callback → 创建 Context → callback 次数 > 0 |
| 23_05 | `external_message` | 注册 callback → `ob_log_external_message("NOHW_TEST_MARKER")` → callback 捕获该 marker |

**Review 要点**:
- 23_02 会在文件系统创建/删除临时日志
- 23_04/23_05 有 `atomic<int>` / `atomic<bool>` 跨线程验证，依赖 100ms sleep
- 每个测试结束都恢复到 WARN 级别，避免污染后续测试

---

### 3.12 TC_CPP_24 Error

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 24_01 | `exception_type_info` | 创建无效 filter → 捕获 `ob::Error` → `what()` 非空 |
| 24_02 | `invalid_value_exception` | `ob_create_video_frame(0,0,0)` → 如果 error 则 `ob_error_get_exception_type` 可调用 |
| 24_03 | `wrong_api_sequence` | 未 start 的 Pipeline 调 `waitForFrameset(100)` → 返回 null 或抛异常 |
| 24_06 | `filter_null_frame` | DecimationFilter `process(nullptr)` → 抛异常或静默（两者均可） |
| 24_07 | `all_exception_types` | 验证 9 种 `OBExceptionType` 枚举值无重复 |

**Review 要点**:
- 24_02 的断言很弱（仅验证 exception_type 可读），不强制要求抛错
- 24_03 同时接受 null 返回 和 `ob::Error` 异常
- 24_06 同时接受抛异常 和 不抛异常

---

### 3.13 TC_CPP_25 DataStruct

**Fixture**: `SDKTestBase`

| 用例 | 名称 | 验证内容 |
|------|------|---------|
| 25_05 | `config_mode_structs` | `OBDepthWorkMode`, `OBMultiDeviceSyncConfig`, `OBNetIpConfig` 的字段赋值/读取 |
| 25_06 | `hdr_roi_point_imu_structs` | `OBHdrConfig`, `OBRegionOfInterest`, `OBPoint3f`, `OBAccelValue` 字段正确 |
| 25_07 | `property_range_structs` | `OBIntPropertyRange`, `OBFloatPropertyRange`, `OBFilterConfigSchemaItem` 字段和范围约束 |

**Review 要点**:
- 纯数据结构测试，不调用 SDK API
- 验证默认初始化和字段赋值正确性

---

## 4. 规划新增用例 (Playback 扩展)

> 详细方案见 [playback_test_plan.md](playback_test_plan.md)

### 4.1 Phase 0: 基础设施

**需修改文件**: `test_common.hpp`, `nohw_full_test.cpp`

| 项目 | 内容 |
|------|------|
| `findAllPlaybackBags()` | 新增函数，返回 `tests/resource/rosbag/` 下所有 `*.bag` 路径 |
| `TestEnvironment::allBagPaths_` | 缓存所有 bag 路径; 新增 `allPlaybackBagPaths()` 访问器 |
| `TC_CPP_18_PlaybackParam` | 参数化 fixture, `::testing::TestWithParam<string>`; helper: `createPlaybackDevice()`, `startPipeline()`, `getFirstFrameset()` |
| `INSTANTIATE_TEST_SUITE_P` | `AllBags` 实例化，遍历所有 bag |

### 4.2 Phase 1: 设备信息 (18_1x)

| 编号 | 用例名 | 验证内容 |
|------|--------|---------|
| 18_10 | `playback_device_info` | DeviceInfo 全字段非空: name, pid, vid, serialNumber, firmwareVersion, connectionType |
| 18_11 | `playback_sensor_list` | SensorList count>0; 至少含 DEPTH + COLOR; 类型在合法枚举范围 |
| 18_12 | `playback_stream_profile_list` | 每个 sensor 的 StreamProfileList 非空; VideoStreamProfile: width>0, height>0, fps>0 |

**与现有的关系**: 补充 18_03 仅检查 `getName()` 非 null 的不足

### 4.3 Phase 2: 帧数据 (18_2x)

| 编号 | 用例名 | 验证内容 |
|------|--------|---------|
| 18_20 | `playback_depth_frame` | type==DEPTH, width>0, height>0, data 非空, dataSize==w×h×pixelSize, timestamp>0 |
| 18_21 | `playback_color_frame` | type==COLOR, format 合法, data 非空 |
| 18_22 | `playback_ir_frame` | IR/IR_LEFT/IR_RIGHT 帧; 无 IR sensor 时 GTEST_SKIP |
| 18_23 | `playback_frame_metadata` | `hasMetadata(TIMESTAMP)==true`; 遍历 34 种 metadata type 记录覆盖情况 |
| 18_24 | `playback_frame_timestamps` | device/system/global timestamp > 0; 连续帧 device timestamp 单调递增 |
| 18_25 | `playback_frameset_multi_stream` | 启用全部 sensor → frameset count >= 2; frame type 不重复 |

**与现有的关系**: 18_03 仅验证 frameset 非 null, 不检查帧内容; 11_06 仅用合成帧测 metadata

### 4.4 Phase 3: 标定参数 (18_3x)

| 编号 | 用例名 | 验证内容 |
|------|--------|---------|
| 18_30 | `playback_depth_intrinsic` | fx,fy>0; cx∈(0,width), cy∈(0,height) |
| 18_31 | `playback_color_intrinsic` | 同上，对 color sensor |
| 18_32 | `playback_depth_distortion` | model 在合法范围; k1-k6, p1-p2 是有限数 |
| 18_33 | `playback_depth_to_color_extrinsic` | 旋转矩阵近似正交 (R·Rᵀ ≈ I); 平移分量有限 |
| 18_34 | `playback_calibration_camera_param_list` | count>0; 每个 CameraParam 的 intrinsic 有效 |

**与现有的关系**: TC_CPP_20 使用合成参数; 这里用 playback 读出的真实参数验证

### 4.5 Phase 4: Filter (18_4x)

| 编号 | 用例名 | 覆盖 API | 核心验证 |
|------|--------|---------|---------|
| 18_40 | `playback_decimation_filter` | `setScaleValue/getScaleValue/getScaleRange` | scale=2 → 输出 w/h 减半 |
| 18_41 | `playback_threshold_filter` | `setValueRange/getMinRange/getMaxRange` | 设范围 → 输出非空 |
| 18_42 | `playback_format_converter` | `setFormatConvertType(OBConvertFormat)` | 输出 format 变化 |
| 18_43 | `playback_point_cloud_filter` | `setCreatePointFormat/setCoordinateDataScaled/setDecimationFactor/getDecimationFactorRange` | 输出 `is<PointsFrame>()` |
| 18_44 | `playback_align_filter` | `getAlignToStreamType/setMatchTargetResolution/setAlignToStreamProfile` | depth-to-color, color-to-depth 对齐 |
| 18_45 | `playback_recommended_filters` | `Sensor::createRecommendedFilters()` | 列表非空, 遍历 process + reset |
| 18_46 | `playback_filter_chain` | 多 filter 串联 + 异步 `pushFrame/setCallBack` + `FilterFactory` | 二/三级链输出正确 |
| 18_47 | `playback_hdr_merge` | 基类 API: `getName()/getConfigSchemaVec()/process()` | 公有filter，HDRMerge 名称正确, process 不崩溃 |
| 18_48 | `playback_sequence_id_filter` | `selectSequenceId/getSelectSequenceId/getSequenceIdList/getSequenceIdListSize` | 公有filter，id 读写一致, list size >= 2 |

**与现有的关系**: TC_CPP_13 验证 filter 的创建/enable/config/type_check, 但**不用真实帧 process**; 18_4x 用 playback 帧做端到端验证

### 4.6 Phase 5: 私有 Filter (18_49~18_56)

> **策略**: 私有 filter 需 activation key, 创建失败时 `GTEST_SKIP`。成功创建时验证特有 API 和 process 效果。

| 编号 | 用例名 | 覆盖 API | 核心验证 |
|------|--------|---------|---------|
| 18_49 | `playback_spatial_advanced_filter` | `getAlphaRange/getDispDiffRange/getRadiusRange/getMagnitudeRange/getFilterParams/setFilterParams` | range 值域合法; 参数读写一致; process 输出非空 |
| 18_50 | `playback_spatial_fast_filter` | `getRadiusRange/getFilterParams/setFilterParams` | range 合法; params 读写一致; process 输出非空 |
| 18_51 | `playback_spatial_moderate_filter` | `getMagnitudeRange/getRadiusRange/getDispDiffRange/getFilterParams/setFilterParams` | 三个 range 合法; params 读写一致 |
| 18_52 | `playback_hole_filling_filter` | `setFilterMode(OBHoleFillingMode)/getFilterMode` | 设 TOP/NEAREST/FAREST 后 get 一致; process 输出非空 |
| 18_53 | `playback_noise_removal_filter` | `setFilterParams/getFilterParams/getDispDiffRange/getMaxSizeRange` | range 合法; params 读写一致; process 输出非空 |
| 18_54 | `playback_temporal_filter` | `getDiffScaleRange/setDiffScale/getWeightRange/setWeight` | range 合法; set→getConfigValue 一致; process 输出非空 |
| 18_55 | `playback_false_positive_filter` | 14 个 range getter (fpEdgeBleed/fptsf/fppaf 系列) | 所有 range: min <= max, step > 0; process 输出非空 |
| 18_56 | `playback_disparity_transform` | 基类 API: `getName()/process()` | 名称正确; process 不崩溃 |

### 4.7 Phase 6: 坐标变换 (18_6x)

| 编号 | 用例名 | 验证内容 |
|------|--------|---------|
| 18_60 | `playback_coord_transform_2d_to_3d` | 用 playback 真实 intrinsic + depth 帧中心深度值做 2D→3D |
| 18_61 | `playback_coord_transform_3d_to_2d` | 上一步的逆变换, 验证回到原像素坐标附近 |

**与现有的关系**: TC_CPP_20 用合成参数; 18_6x 用真实设备参数和帧数据

---

## 5. 重构建议

### 5.1 Fixture 整合

| 现状 | 建议 |
|------|------|
| `TC_CPP_01_Context` / `TC_CPP_02_Discovery` / `TC_CPP_14_Property` 都用 `ContextTest` 但作为独立 class | 可直接用 `ContextTest` 减少空类声明 |
| 各 test group class 只是 `class X : public SDKTestBase {};` | 用 `using TC_CPP_10_Frame = SDKTestBase;` 或直接测试组注释 |

### 5.2 C API vs C++ API 覆盖

| 领域 | 当前覆盖 | 建议 |
|------|---------|------|
| Frame 创建 | 仅 C API (`ob_create_frame`) | 补充 C++ `FrameFactory` 包装 |
| Metadata | 仅 C API (`ob_frame_update_metadata`) | 补充 `frame->hasMetadata()` / `getMetadataValue()` (18_23) |
| Filter config | 仅 C API (`ob_filter_*`) | 13_06 已覆盖; 18_4x 用 C++ 包装 |
| Logger | 仅 C API (`ob_set_logger_*`) | 可考虑补 C++ logger 接口 |

### 5.3 断言强度分级

| 级别 | 当前用例 | 说明 |
|------|---------|------|
| 仅"不崩溃" | 02_02, 02_06, 14_16, 24_03, 24_06 | 最弱——建议加效果断言 |
| 状态一致性 | 13_05, 18_06 | 中——验证 API 返回的状态值 |
| 数据正确性 | 12_03, 20_01-20_04, 22_02 | 强——验证具体数值 |
| 端到端效果 | (缺失，规划在 18_4x) | 最强——真实数据进出 |

### 5.4 编号空缺

| 编号段 | 已用 | 空缺 |
|--------|------|------|
| 01_xx | 01-06 | 无 |
| 02_xx | 02, 06 | 01, 03-05 |
| 10_xx | 04, 13 | 01-03, 05-12 |
| 11_xx | 06 | 01-05 |
| 12_xx | 01, 03, 04 | 02 |
| 13_xx | 01, 02, 05, 06, 07, 17 | 03, 04, 08-16 |
| 14_xx | 16 | 01-15 |
| 18_xx | 03-07 | 01-02, 08-09 |
| 20_xx | 01-04 | 无 |
| 22_xx | 01-03 | 无 |
| 23_xx | 01-05 | 无 |
| 24_xx | 01-03, 06-07 | 04-05 |
| 25_xx | 05-07 | 01-04 |

> 空缺编号对应可能存在于 `hw_full_test` 中的硬件用例，或后续补充。

### 5.5 稳定性风险

| 用例 | 风险 | 建议 |
|------|------|------|
| 18_06, 18_07 | `sleep_for` 200-500ms, CI 下可能不足 | 改用条件变量 + 超时等待 |
| 23_02 | 文件系统 I/O 依赖 | 确保 test cleanup 在 assertion failure 时也执行 |
| 23_04, 23_05 | callback 依赖异步线程 + 100ms sleep | 改用 `std::promise/future` |

---

## 6. 完整用例索引

### 现有 (46 个)

| # | 编号 | 名称 | Fixture | 行号 |
|---|------|------|---------|------|
| 1 | 01_01 | `context_default_create_destroy` | ContextTest | ~30 |
| 2 | 01_02 | `context_config_path` | ContextTest | ~38 |
| 3 | 01_03 | `context_repeated_create_destroy` | ContextTest | ~56 |
| 4 | 01_04 | `free_idle_memory` | ContextTest | ~66 |
| 5 | 01_05 | `uvc_backend` | ContextTest | ~77 |
| 6 | 01_06 | `extension_plugin_directory` | ContextTest | ~134 |
| 7 | 02_02 | `net_device_enum_toggle` | ContextTest | ~153 |
| 8 | 02_06 | `clock_sync` | ContextTest | ~160 |
| 9 | 10_04 | `frame_ref_count` | SDKTestBase | ~168 |
| 10 | 10_13 | `frameset_push_frame` | SDKTestBase | ~184 |
| 11 | 11_06 | `update_metadata_c_api` | SDKTestBase | ~211 |
| 12 | 12_01 | `create_frame_and_video_frame` | SDKTestBase | ~239 |
| 13 | 12_03 | `create_frame_from_buffer` | SDKTestBase | ~261 |
| 14 | 12_04 | `create_empty_frameset` | SDKTestBase | ~293 |
| 15 | 13_01 | `create_all_builtin_filters` | SDKTestBase | ~320 |
| 16 | 13_02 | `create_invalid_filter` | SDKTestBase | ~376 |
| 17 | 13_05 | `filter_enable_disable` | SDKTestBase | ~386 |
| 18 | 13_06 | `filter_reset_and_config` | SDKTestBase | ~407 |
| 19 | 13_07 | `filter_type_check` | SDKTestBase | ~446 |
| 20 | 13_17 | `private_filter` | SDKTestBase | ~457 |
| 21 | 14_16 | `sdk_level_property` | ContextTest | ~475 |
| 22 | 18_03 | `playback_device_create_and_play` | TC_CPP_18_Playback | ~536 |
| 23 | 18_04 | `playback_duration_position` | TC_CPP_18_Playback | ~567 |
| 24 | 18_05 | `playback_seek_and_rate` | TC_CPP_18_Playback | ~578 |
| 25 | 18_06 | `playback_pause_resume_status` | TC_CPP_18_Playback | ~591 |
| 26 | 18_07 | `playback_status_callback` | TC_CPP_18_Playback | ~623 |
| 27 | 20_01 | `3d_to_3d` | SDKTestBase | ~662 |
| 28 | 20_02 | `2d_depth_to_3d` | SDKTestBase | ~688 |
| 29 | 20_03 | `3d_to_2d` | SDKTestBase | ~710 |
| 30 | 20_04 | `2d_to_2d` | SDKTestBase | ~735 |
| 31 | 22_01 | `full_version` | SDKTestBase | ~762 |
| 32 | 22_02 | `major_minor_patch` | SDKTestBase | ~768 |
| 33 | 22_03 | `stage_version` | SDKTestBase | ~782 |
| 34 | 23_01 | `log_severity_set` | SDKTestBase | ~800 |
| 35 | 23_02 | `log_to_file` | SDKTestBase | ~815 |
| 36 | 23_03 | `log_to_console` | SDKTestBase | ~845 |
| 37 | 23_04 | `log_callback` | SDKTestBase | ~853 |
| 38 | 23_05 | `external_message` | SDKTestBase | ~876 |
| 39 | 24_01 | `exception_type_info` | SDKTestBase | ~902 |
| 40 | 24_02 | `invalid_value_exception` | SDKTestBase | ~918 |
| 41 | 24_03 | `wrong_api_sequence` | SDKTestBase | ~935 |
| 42 | 24_06 | `filter_null_frame` | SDKTestBase | ~948 |
| 43 | 24_07 | `all_exception_types` | SDKTestBase | ~962 |
| 44 | 25_05 | `config_mode_structs` | SDKTestBase | ~985 |
| 45 | 25_06 | `hdr_roi_point_imu_structs` | SDKTestBase | ~1012 |
| 46 | 25_07 | `property_range_structs` | SDKTestBase | ~1043 |

### 规划新增 (33 个)

| # | 编号 | 名称 | Phase | Fixture |
|---|------|------|-------|---------|
| 47 | 18_10 | `playback_device_info` | 1 | TC_CPP_18_PlaybackParam |
| 48 | 18_11 | `playback_sensor_list` | 1 | TC_CPP_18_PlaybackParam |
| 49 | 18_12 | `playback_stream_profile_list` | 1 | TC_CPP_18_PlaybackParam |
| 50 | 18_20 | `playback_depth_frame` | 2 | TC_CPP_18_PlaybackParam |
| 51 | 18_21 | `playback_color_frame` | 2 | TC_CPP_18_PlaybackParam |
| 52 | 18_22 | `playback_ir_frame` | 2 | TC_CPP_18_PlaybackParam |
| 53 | 18_23 | `playback_frame_metadata` | 2 | TC_CPP_18_PlaybackParam |
| 54 | 18_24 | `playback_frame_timestamps` | 2 | TC_CPP_18_PlaybackParam |
| 55 | 18_25 | `playback_frameset_multi_stream` | 2 | TC_CPP_18_PlaybackParam |
| 56 | 18_30 | `playback_depth_intrinsic` | 3 | TC_CPP_18_PlaybackParam |
| 57 | 18_31 | `playback_color_intrinsic` | 3 | TC_CPP_18_PlaybackParam |
| 58 | 18_32 | `playback_depth_distortion` | 3 | TC_CPP_18_PlaybackParam |
| 59 | 18_33 | `playback_depth_to_color_extrinsic` | 3 | TC_CPP_18_PlaybackParam |
| 60 | 18_34 | `playback_calibration_camera_param_list` | 3 | TC_CPP_18_PlaybackParam |
| 61 | 18_40 | `playback_decimation_filter` | 4 | TC_CPP_18_PlaybackParam |
| 62 | 18_41 | `playback_threshold_filter` | 4 | TC_CPP_18_PlaybackParam |
| 63 | 18_42 | `playback_format_converter` | 4 | TC_CPP_18_PlaybackParam |
| 64 | 18_43 | `playback_point_cloud_filter` | 4 | TC_CPP_18_PlaybackParam |
| 65 | 18_44 | `playback_align_filter` | 4 | TC_CPP_18_PlaybackParam |
| 66 | 18_45 | `playback_recommended_filters` | 4 | TC_CPP_18_PlaybackParam |
| 67 | 18_46 | `playback_filter_chain` | 4 | TC_CPP_18_PlaybackParam |
| 68 | 18_47 | `playback_hdr_merge` | 4 | TC_CPP_18_PlaybackParam |
| 69 | 18_48 | `playback_sequence_id_filter` | 4 | TC_CPP_18_PlaybackParam |
| 70 | 18_49 | `playback_spatial_advanced_filter` | 5 | TC_CPP_18_PlaybackParam |
| 71 | 18_50 | `playback_spatial_fast_filter` | 5 | TC_CPP_18_PlaybackParam |
| 72 | 18_51 | `playback_spatial_moderate_filter` | 5 | TC_CPP_18_PlaybackParam |
| 73 | 18_52 | `playback_hole_filling_filter` | 5 | TC_CPP_18_PlaybackParam |
| 74 | 18_53 | `playback_noise_removal_filter` | 5 | TC_CPP_18_PlaybackParam |
| 75 | 18_54 | `playback_temporal_filter` | 5 | TC_CPP_18_PlaybackParam |
| 76 | 18_55 | `playback_false_positive_filter` | 5 | TC_CPP_18_PlaybackParam |
| 77 | 18_56 | `playback_disparity_transform` | 5 | TC_CPP_18_PlaybackParam |
| 78 | 18_60 | `playback_coord_transform_2d_to_3d` | 6 | TC_CPP_18_PlaybackParam |
| 79 | 18_61 | `playback_coord_transform_3d_to_2d` | 6 | TC_CPP_18_PlaybackParam |

---

**总计: 46 现有 + 33 规划 = 79 个测试用例**
