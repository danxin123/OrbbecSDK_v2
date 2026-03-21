# TC_CPP_09 — 流水线配置（Config）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 9 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 09：流水线配置（Config）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 09-010 | 按类型启用流 | `ob_config_enable_stream(config, type)` | `enableStream(type)` |
| 09-020 | 按 Profile 启用流 | `ob_config_enable_stream_with_stream_profile(config, profile)` | `enableStream(profile)` |
| 09-030 | 参数化启用视频流 | `ob_config_enable_video_stream(config, type, w, h, fps, fmt)` | `enableVideoStream(...)` |
| 09-040 | 启用加速度计 | `ob_config_enable_accel_stream(config, fullScale, sampleRate)` | `enableAccelStream(...)` |
| 09-050 | 启用陀螺仪 | `ob_config_enable_gyro_stream(config, fullScale, sampleRate)` | `enableGyroStream(...)` |
| 09-060 | 启用 LiDAR | `ob_config_enable_lidar_stream(config, scanRate, format)` | `enableLidarStream(...)` |
| 09-070 | 启用所有流 | `ob_config_enable_all_stream(config)` | `enableAllStream()` |
| 09-080 | 禁用特定流 | `ob_config_disable_stream(config, type)` | `disableStream(type)` |
| 09-090 | 禁用所有流 | `ob_config_disable_all_stream(config)` | `disableAllStream()` |
| 09-100 | 查询已启用 Profile | `ob_config_get_enabled_stream_profile_list()` | `getEnabledStreamProfileList()` |
| 09-110 | D2C 对齐模式 | `ob_config_set_align_mode(config, mode)` | `setAlignMode(mode)` |
| 09-120 | 对齐后深度缩放 | `ob_config_set_depth_scale_after_align_require()` | `setDepthScaleAfterAlignRequire()` |
| 09-130 | 帧聚合输出模式 | `ob_config_set_frame_aggregate_output_mode()` | `setFrameAggregateOutputMode()` |

---

## TC_CPP_09_01 — 按类型/Profile 启用流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_01 |
| 名称 | 按类型/Profile 启用流 |
| 目的 | 验证 Config 可通过流类型或指定 Profile 启用流 |
| 覆盖功能点 | 09-010, 09-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Config` 对象
2. 调用 `config->enableStream(OB_STREAM_DEPTH)` 按类型启用深度流
3. 获取深度 Profile，调用 `config->enableStream(profile)` 按 Profile 启用
4. 使用 Config 启动 Pipeline
5. 验证收到的流

### 检查项

- ✅ `enableStream(OB_STREAM_DEPTH)` 调用成功
- ✅ `enableStream(profile)` 调用成功
- ✅ Pipeline 启动后收到对应流的帧
- ✅ 按 Profile 启用的流分辨率与 Profile 一致
- ✅ C API：`ob_config_enable_stream(config, OB_STREAM_DEPTH, &error)` 成功

### 预期结果

Config 可通过类型或 Profile 两种方式启用流，均正常工作。

---

## TC_CPP_09_02 — 参数化启用视频流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_02 |
| 名称 | 参数化启用视频流 |
| 目的 | 验证可通过指定参数（宽、高、帧率、格式）启用视频流 |
| 覆盖功能点 | 09-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Config` 对象
2. 调用 `config->enableVideoStream(OB_STREAM_DEPTH, 640, 480, 30, OB_FORMAT_Y16)`
3. 使用 Config 启动 Pipeline
4. 获取帧，验证分辨率
5. 尝试其他参数组合

### 检查项

- ✅ `enableVideoStream()` 调用成功
- ✅ Pipeline 启动后收到深度帧
- ✅ 帧分辨率 640x480 与配置一致
- ✅ 帧格式 Y16 与配置一致
- ✅ C API：`ob_config_enable_video_stream(config, type, w, h, fps, fmt, &error)` 成功

### 预期结果

可通过精确参数启用视频流，开流后帧数据与配置匹配。

---

## TC_CPP_09_03 — 启用 IMU 流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_03 |
| 名称 | 启用 IMU 流 |
| 目的 | 验证可启用加速度计和陀螺仪数据流 |
| 覆盖功能点 | 09-040, 09-050 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Config` 对象
2. 调用 `config->enableAccelStream(OB_ACCEL_FULL_SCALE_RANGE_4G, OB_ACCEL_SAMPLE_RATE_200HZ)`
3. 调用 `config->enableGyroStream(OB_GYRO_FULL_SCALE_RANGE_500DPS, OB_GYRO_SAMPLE_RATE_200HZ)`
4. 使用 Config 启动 Pipeline
5. 获取 IMU 帧数据

### 检查项

- ✅ `enableAccelStream()` 调用成功
- ✅ `enableGyroStream()` 调用成功
- ✅ 收到 AccelFrame，数据有效
- ✅ 收到 GyroFrame，数据有效
- ✅ 帧数据量程与配置一致

### 预期结果

IMU 流可正常启用，收到加速度和陀螺仪数据。

---

## TC_CPP_09_04 — 启用/禁用所有流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_04 |
| 名称 | 启用/禁用所有流 |
| 目的 | 验证 enableAllStream、disableStream、disableAllStream 功能 |
| 覆盖功能点 | 09-070, 09-080, 09-090 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Config` 对象
2. 调用 `config->enableAllStream()` 启用所有流
3. 启动 Pipeline，验证多流输出
4. 停止 Pipeline
5. 调用 `config->disableStream(OB_STREAM_COLOR)` 禁用彩色流
6. 启动 Pipeline，验证无彩色帧
7. 停止 Pipeline
8. 调用 `config->disableAllStream()` 再启用深度流
9. 启动 Pipeline，验证仅深度帧

### 检查项

- ✅ `enableAllStream()` 启用所有支持的流
- ✅ `disableStream(OB_STREAM_COLOR)` 后无彩色输出
- ✅ `disableAllStream()` 后所有流停止
- ✅ 禁用后可重新 `enableStream()` 启用特定流

### 预期结果

流的启用/禁用控制正常工作，可灵活配置输出。

---

## TC_CPP_09_05 — 查询已启用 Profile 列表

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_05 |
| 名称 | 查询已启用 Profile 列表 |
| 目的 | 验证可查询 Config 中已启用的 StreamProfile 列表 |
| 覆盖功能点 | 09-100 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Config` 对象
2. 启用深度和彩色流
3. 调用 `config->getEnabledStreamProfileList()` 获取已启用列表
4. 验证列表内容

### 检查项

- ✅ 返回非空 StreamProfile 列表
- ✅ 列表数量与启用的流数量一致
- ✅ 列表中包含深度和彩色 Profile
- ✅ 各 Profile 参数正确

### 预期结果

可正确查询 Config 中已启用的 StreamProfile 列表。

---

## TC_CPP_09_06 — D2C 对齐模式

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_06 |
| 名称 | D2C 对齐模式 |
| 目的 | 验证可设置 D2C 对齐模式（HW/SW/DISABLE） |
| 覆盖功能点 | 09-110, 09-200~09-220 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Config` 对象，启用深度和彩色流
2. 调用 `config->setAlignMode(ALIGN_D2C_HW_MODE)` 硬件对齐
3. 启动 Pipeline，获取帧
4. 停止 Pipeline
5. 设置 `ALIGN_D2C_SW_MODE` 软件对齐，重复测试
6. 设置 `ALIGN_DISABLE` 禁用对齐，重复测试

### 检查项

- ✅ `setAlignMode(ALIGN_D2C_HW_MODE)` 调用成功
- ✅ `setAlignMode(ALIGN_D2C_SW_MODE)` 调用成功
- ✅ `setAlignMode(ALIGN_DISABLE)` 调用成功
- ✅ 各模式下开流正常，深度帧与彩色帧对齐
- ✅ 对齐后深度分辨率与彩色分辨率一致（D2C 模式）

### 预期结果

三种 D2C 对齐模式均可设置，开流后深度与彩色帧对齐。

---

## TC_CPP_09_07 — 对齐后深度缩放

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_07 |
| 名称 | 对齐后深度缩放 |
| 目的 | 验证可设置对齐后深度值是否缩放 |
| 覆盖功能点 | 09-120 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Config` 对象，启用 D2C 对齐
2. 调用 `config->setDepthScaleAfterAlignRequire(true)`
3. 启动 Pipeline，获取深度帧
4. 检查深度值比例
5. 设置 `false`，重复测试

### 检查项

- ✅ `setDepthScaleAfterAlignRequire(true)` 调用成功
- ✅ 对齐后深度值比例正确
- ✅ `setDepthScaleAfterAlignRequire(false)` 调用成功
- ✅ 深度值保持原始比例

### 预期结果

对齐后深度缩放开关可正常设置，深度值比例符合配置。

---

## TC_CPP_09_08 — 帧聚合输出模式

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_08 |
| 名称 | 帧聚合输出模式 |
| 目的 | 验证可设置帧聚合输出模式，控制多流帧输出策略 |
| 覆盖功能点 | 09-130, 09-300~09-330 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Config` 对象，启用多流
2. 分别设置各模式并测试：
   - `setFrameAggregateOutputMode(ALL_TYPE_FRAME_REQUIRE)`
   - `setFrameAggregateOutputMode(COLOR_FRAME_REQUIRE)`
   - `setFrameAggregateOutputMode(ANY_SITUATION)`
   - `setFrameAggregateOutputMode(DISABLE)`
3. 观察各模式下 FrameSet 的输出行为

### 检查项

- ✅ ALL_TYPE_FRAME_REQUIRE：所有流有帧时才输出 FrameSet
- ✅ COLOR_FRAME_REQUIRE：彩色帧可用时即输出
- ✅ ANY_SITUATION：任何情况下都输出
- ✅ DISABLE：禁用帧聚合
- ✅ 各模式设置调用成功

### 预期结果

各帧聚合输出模式可正常设置，输出行为符合模式定义。

---

## TC_CPP_09_09 — 启用 LiDAR 流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_09_09 |
| 名称 | 启用 LiDAR 流 |
| 目的 | 验证可启用 LiDAR 数据流（需 LiDAR 设备支持） |
| 覆盖功能点 | 09-060 |
| 前置条件 | 具备 LiDAR 功能的 Orbbec 设备 |
| 硬件依赖 | LiDAR设备 |
| 优先级 | P2 |

### 测试步骤

1. 创建 `Config` 对象
2. 调用 `config->enableLidarStream(scanRate, format)`
3. 启动 Pipeline
4. 获取 LiDAR 帧数据

### 检查项

- ✅ `enableLidarStream()` 调用成功（若设备支持）
- ✅ 收到 LiDAR 帧数据
- ✅ 扫描频率和格式与配置一致
- ✅ 若设备不支持，安全返回错误

### 预期结果

LiDAR 流可在支持的设备上启用，收到有效的 LiDAR 数据。
