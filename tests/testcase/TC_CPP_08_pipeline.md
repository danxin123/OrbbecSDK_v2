# TC_CPP_08 — 流水线（Pipeline）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 11 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 08：流水线（Pipeline）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 08-010 | 默认创建 | `ob_create_pipeline()` | `Pipeline()` |
| 08-020 | 指定设备创建 | `ob_create_pipeline_with_device(dev)` | `Pipeline(device)` |
| 08-030 | 默认开流 | `ob_pipeline_start(pipe)` | `start()` |
| 08-040 | 配置开流（轮询） | `ob_pipeline_start_with_config(pipe, config)` | `start(config)` |
| 08-050 | 配置开流（回调） | `ob_pipeline_start_with_callback(pipe, config, cb)` | `start(config, callback)` |
| 08-060 | 停流 | `ob_pipeline_stop(pipe)` | `stop()` |
| 08-070 | 轮询取帧 | `ob_pipeline_wait_for_frameset(pipe, timeout_ms)` | `waitForFrames(timeout)` |
| 08-080 | 运行时切换配置 | `ob_pipeline_switch_config(pipe, config)` | `switchConfig(config)` |
| 08-090 | 获取绑定设备 | `ob_pipeline_get_device(pipe)` | `getDevice()` |
| 08-100 | 获取 StreamProfile | `ob_pipeline_get_stream_profile_list(pipe, sensorType)` | `getStreamProfileList(sensorType)` |
| 08-110 | 帧同步开关 | `ob_pipeline_enable/disable_frame_sync()` | `enableFrameSync()` / `disableFrameSync()` |
| 08-120 | D2C 兼容 Profile | `ob_get_d2c_depth_profile_list()` | `getD2CDepthProfileList()` |
| 08-130 | 获取相机参数 | `ob_pipeline_get_camera_param()` | `getCameraParam()` |
| 08-140 | 获取指定分辨率参数 | `ob_pipeline_get_camera_param_with_profile()` | `getCameraParamWithProfile()` |
| 08-150 | 获取完整标定参数 | `ob_pipeline_get_calibration_param()` | `getCalibrationParam()` |

---

## TC_CPP_08_01 — Pipeline 默认/指定设备构造

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_01 |
| 名称 | Pipeline 默认/指定设备构造 |
| 目的 | 验证 Pipeline 默认构造自动绑定首设备，以及显式指定设备构造 |
| 覆盖功能点 | 08-010, 08-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `Pipeline()` 默认构造
3. 验证 Pipeline 绑定的设备
4. 获取指定设备
5. 调用 `Pipeline(device)` 显式构造
6. 验证 Pipeline 绑定的设备与传入设备一致

### 检查项

- ✅ `Pipeline()` 默认构造成功，自动绑定首设备
- ✅ `pipeline->getDevice()` 返回有效设备
- ✅ `Pipeline(device)` 显式构造成功
- ✅ 构造的 Pipeline 绑定传入的设备
- ✅ C API：`ob_create_pipeline()` 创建成功
- ✅ C API：`ob_create_pipeline_with_device(dev)` 创建成功

### 预期结果

Pipeline 可通过默认方式或显式指定设备两种方式构造，均能正确绑定设备。

---

## TC_CPP_08_02 — Pipeline 默认开流与停流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_02 |
| 名称 | Pipeline 默认开流与停流 |
| 目的 | 验证 Pipeline 使用默认配置启动和停止流传输 |
| 覆盖功能点 | 08-030, 08-060 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 调用 `pipeline->start()` 默认开流
3. 调用 `pipeline->waitForFrames(3000)` 等待帧
4. 验证收到的帧数据
5. 调用 `pipeline->stop()` 停流
6. 验证停流后无法再获取帧

### 检查项

- ✅ `start()` 调用成功，不抛异常
- ✅ `waitForFrames()` 返回非空 FrameSet
- ✅ FrameSet 包含有效帧数据
- ✅ `stop()` 调用成功，不抛异常
- ✅ 停流后 `waitForFrames()` 超时或返回空
- ✅ C API：`ob_pipeline_start(pipe, &error)` 成功

### 预期结果

Pipeline 可正常默认开流、取帧、停流，流程完整。

---

## TC_CPP_08_03 — Pipeline 配置开流（轮询）

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_03 |
| 名称 | Pipeline 配置开流（轮询） |
| 目的 | 验证 Pipeline 使用 Config 配置轮询方式开流 |
| 覆盖功能点 | 08-040, 08-070 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 创建 `Config` 对象并启用深度流
3. 调用 `pipeline->start(config)` 配置开流
4. 循环 10 次调用 `pipeline->waitForFrames(3000)` 取帧
5. 统计帧率和数据完整性
6. 调用 `pipeline->stop()` 停流

### 检查项

- ✅ `start(config)` 调用成功
- ✅ `waitForFrames()` 每次均返回非空 FrameSet
- ✅ FrameSet 中包含启用的流类型（深度帧）
- ✅ 帧时间戳递增正常
- ✅ C API：`ob_pipeline_start_with_config(pipe, config, &error)` 成功

### 预期结果

通过 Config 配置轮询方式开流正常，可稳定获取配置的流数据。

---

## TC_CPP_08_04 — Pipeline 配置开流（回调）

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_04 |
| 名称 | Pipeline 配置开流（回调） |
| 目的 | 验证 Pipeline 使用 Config 配置回调方式开流 |
| 覆盖功能点 | 08-050 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 创建 `Config` 对象并启用深度和彩色流
3. 注册帧回调函数
4. 调用 `pipeline->start(config, callback)` 配置开流
5. 运行 3 秒，统计回调触发次数和帧数
6. 调用 `pipeline->stop()` 停流

### 检查项

- ✅ `start(config, callback)` 调用成功
- ✅ 回调函数被触发，收到 FrameSet
- ✅ 3 秒内收到约 90 帧（30fps × 3s）
- ✅ FrameSet 中包含启用的流类型
- ✅ `stop()` 后回调不再触发
- ✅ C API：`ob_pipeline_start_with_callback(pipe, config, callback, user_data, &error)` 成功

### 预期结果

通过 Config 配置回调方式开流正常，可实时接收 FrameSet。

---

## TC_CPP_08_05 — Pipeline 运行时切换配置

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_05 |
| 名称 | Pipeline 运行时切换配置 |
| 目的 | 验证 Pipeline 在开流状态下可动态切换配置 |
| 覆盖功能点 | 08-080 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 创建 Config A（640x480 深度）并启动
3. 获取几帧，验证分辨率
4. 创建 Config B（1280x720 深度）
5. 调用 `pipeline->switchConfig(newConfig)` 切换配置
6. 获取几帧，验证新分辨率
7. 停流

### 检查项

- ✅ `switchConfig()` 调用成功
- ✅ 切换后继续收到帧，不中断
- ✅ 切换后帧分辨率与新的 Profile 一致
- ✅ 时间戳连续性保持（或重置后递增）
- ✅ C API：`ob_pipeline_switch_config(pipe, config, &error)` 成功

### 预期结果

Pipeline 可在运行时动态切换流配置，切换过程平稳。

---

## TC_CPP_08_06 — Pipeline 获取绑定设备

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_06 |
| 名称 | Pipeline 获取绑定设备 |
| 目的 | 验证 Pipeline 可获取当前绑定的设备对象 |
| 覆盖功能点 | 08-090 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备对象并记录序列号
2. 使用设备创建 `Pipeline(device)`
3. 调用 `pipeline->getDevice()` 获取绑定设备
4. 验证设备信息

### 检查项

- ✅ `getDevice()` 返回非空 Device 对象
- ✅ 返回设备的序列号与构造时传入的设备一致
- ✅ 返回设备的 DeviceInfo 与预期一致
- ✅ C API：`ob_pipeline_get_device(pipe, &error)` 返回非 NULL

### 预期结果

Pipeline 正确返回绑定的设备对象，设备信息一致。

---

## TC_CPP_08_07 — Pipeline 获取 StreamProfile 列表

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_07 |
| 名称 | Pipeline 获取 StreamProfile 列表 |
| 目的 | 验证 Pipeline 可获取各传感器的 StreamProfile 列表 |
| 覆盖功能点 | 08-100 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 调用 `pipeline->getStreamProfileList(OB_SENSOR_DEPTH)` 获取深度 Profile 列表
3. 调用 `pipeline->getStreamProfileList(OB_SENSOR_COLOR)` 获取彩色 Profile 列表
4. 验证列表内容

### 检查项

- ✅ `getStreamProfileList(OB_SENSOR_DEPTH)` 返回非空列表
- ✅ `getStreamProfileList(OB_SENSOR_COLOR)` 返回非空列表
- ✅ 列表内容与直接通过 Sensor 获取的 Profile 列表一致
- ✅ 列表包含有效的 Profile 配置
- ✅ C API：`ob_pipeline_get_stream_profile_list(pipe, OB_SENSOR_DEPTH, &error)` 返回非 NULL

### 预期结果

Pipeline 可正确获取各传感器的 StreamProfile 列表，与 Sensor 级别一致。

---

## TC_CPP_08_08 — Pipeline 帧同步开关

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_08 |
| 名称 | Pipeline 帧同步开关 |
| 目的 | 验证 Pipeline 可启用/禁用多流帧同步 |
| 覆盖功能点 | 08-110 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 创建 Config 启用深度和彩色流
3. 启动 Pipeline
4. 获取几帧，记录 Depth/Color 时间戳差
5. 调用 `pipeline->enableFrameSync()` 启用帧同步
6. 获取几帧，验证时间戳差
7. 调用 `pipeline->disableFrameSync()` 禁用帧同步
8. 停流

### 检查项

- ✅ `enableFrameSync()` 调用不崩溃
- ✅ 启用帧同步后，Depth/Color 时间戳差 < 33ms（一帧时间）
- ✅ `disableFrameSync()` 调用不崩溃
- ✅ C API：`ob_pipeline_enable_frame_sync(pipe, &error)` 成功
- ✅ C API：`ob_pipeline_disable_frame_sync(pipe, &error)` 成功

### 预期结果

Pipeline 帧同步开关正常工作，启用后多流帧时间戳对齐。

---

## TC_CPP_08_09 — D2C 兼容 Depth Profile 列表

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_09 |
| 名称 | D2C 兼容 Depth Profile 列表 |
| 目的 | 验证可获取与指定 Color Profile 兼容的 Depth Profile 列表 |
| 覆盖功能点 | 08-120 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 获取彩色 Profile 列表，选择一个 Profile
3. 调用 `pipeline->getD2CDepthProfileList(colorProfile, alignMode)` 获取兼容 Depth Profile 列表
4. 验证返回的列表
5. 尝试不同的 alignMode（HW/SW）

### 检查项

- ✅ `getD2CDepthProfileList()` 返回非空列表
- ✅ 返回的 Depth Profile 与指定的 Color Profile D2C 兼容
- ✅ HW 模式和 SW 模式返回的列表可能不同
- ✅ 列表中的 Profile 可用于 D2C 对齐
- ✅ C API：`ob_get_d2c_depth_profile_list(pipe, color_profile, align_mode, &error)` 返回非 NULL

### 预期结果

可获取与指定彩色配置兼容的深度配置列表，用于 D2C 对齐。

---

## TC_CPP_08_10 — Pipeline 获取相机参数

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_10 |
| 名称 | Pipeline 获取相机参数 |
| 目的 | 验证 Pipeline 可获取相机内参、外参等标定参数 |
| 覆盖功能点 | 08-130, 08-140 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 启动流
3. 调用 `pipeline->getCameraParam()` 获取相机参数
4. 验证参数内容
5. 调用 `pipeline->getCameraParamWithProfile(depthProfile, colorProfile)` 获取指定分辨率参数
6. 停流

### 检查项

- ✅ `getCameraParam()` 返回有效的 OBCameraParam 结构体
- ✅ depth 内参 fx > 0, fy > 0, cx > 0, cy > 0
- ✅ color 内参 fx > 0, fy > 0, cx > 0, cy > 0
- ✅ 外参旋转矩阵和平移向量有效
- ✅ `getCameraParamWithProfile()` 返回与指定 Profile 对应的参数
- ✅ C API：`ob_pipeline_get_camera_param(pipe, &error)` 成功

### 预期结果

Pipeline 可正确获取当前或指定分辨率的相机标定参数。

---

## TC_CPP_08_11 — Pipeline 获取完整标定参数

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_08_11 |
| 名称 | Pipeline 获取完整标定参数 |
| 目的 | 验证 Pipeline 可获取完整的多传感器标定参数 |
| 覆盖功能点 | 08-150 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Pipeline` 对象
2. 启动流
3. 调用 `pipeline->getCalibrationParam()` 获取完整标定参数
4. 验证参数内容
5. 停流

### 检查项

- ✅ `getCalibrationParam()` 返回有效的 OBCalibrationParam 结构体
- ✅ 包含 depth、color、IR 等多个传感器的内参
- ✅ 包含各传感器之间的外参（旋转/平移）
- ✅ IMU 标定参数（若设备支持）
- ✅ 参数值在合理范围内
- ✅ C API：`ob_pipeline_get_calibration_param(pipe, &error)` 成功

### 预期结果

Pipeline 可正确获取完整的多传感器标定参数，用于高级算法处理。
