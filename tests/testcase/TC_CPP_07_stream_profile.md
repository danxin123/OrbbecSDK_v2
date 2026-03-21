# TC_CPP_07 — 流配置（StreamProfile）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 5 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 07：流配置（StreamProfile）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 07-010 | Profile 列表查询 | `ob_sensor_get_stream_profile_list()` | `getStreamProfileList()` |
| 07-020 | 按参数筛选 | `ob_stream_profile_list_get_video_stream_profile()` | `getVideoStreamProfile(w,h,fmt,fps)` |
| 07-030 | 视频流属性 | 宽、高、帧率、格式 | `width()` / `height()` / `fps()` / `format()` |
| 07-040 | 加速度计属性 | 量程、采样率 | `fullScaleRange()` / `sampleRate()` |
| 07-050 | 陀螺仪属性 | 量程、采样率 | `fullScaleRange()` / `sampleRate()` |
| 07-060 | 类型检查/转换 | — | `is<T>()` / `as<T>()` |

---

## TC_CPP_07_01 — Depth/Color Profile 列表

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_07_01 |
| 名称 | Depth/Color Profile 列表 |
| 目的 | 验证可获取 Depth 和 Color 传感器的 StreamProfile 列表并验证各 Profile 属性 |
| 覆盖功能点 | 07-010, 07-030 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取深度传感器和彩色传感器
3. 调用 `sensor->getStreamProfileList()` 获取 Profile 列表
4. 读取 `profileList->count()` 获取数量
5. 遍历列表，获取每个 Profile 并验证属性

### 检查项

- ✅ `getStreamProfileList()` 返回非空列表
- ✅ `count()` > 0
- ✅ 每个 Profile 的 `width() > 0`
- ✅ 每个 Profile 的 `height() > 0`
- ✅ 每个 Profile 的 `fps() > 0`
- ✅ 每个 Profile 的 `format()` 返回有效的 OBFormat 枚举值
- ✅ Profile 列表按分辨率/帧率合理排序

### 预期结果

各传感器支持的 StreamProfile 列表正确返回，属性值合理有效。

---

## TC_CPP_07_02 — 按参数筛选 VideoStreamProfile

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_07_02 |
| 名称 | 按参数筛选 VideoStreamProfile |
| 目的 | 验证可通过指定参数（宽、高、格式、帧率）筛选 Profile |
| 覆盖功能点 | 07-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取深度传感器
3. 调用 `profileList->getVideoStreamProfile(640, 480, OB_FORMAT_Y16, 30)`
4. 尝试其他参数组合（如 1280x720, 30fps）
5. 验证返回的 Profile 属性

### 检查项

- ✅ 对支持的参数组合返回非空 Profile
- ✅ 返回 Profile 的 `width()` 与请求值一致
- ✅ 返回 Profile 的 `height()` 与请求值一致
- ✅ 返回 Profile 的 `format()` 与请求值一致
- ✅ 返回 Profile 的 `fps()` 与请求值一致
- ✅ 对不支持的参数组合返回 null 或抛异常

### 预期结果

可通过精确参数筛选到匹配的 StreamProfile。

---

## TC_CPP_07_03 — AccelStreamProfile 属性

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_07_03 |
| 名称 | AccelStreamProfile 属性 |
| 目的 | 验证加速度计 StreamProfile 的量程和采样率属性可正确读取 |
| 覆盖功能点 | 07-040 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取加速度计传感器
3. 调用 `sensor->getStreamProfileList()` 获取 Profile 列表
4. 获取首个 Profile 并转换为 AccelStreamProfile
5. 读取 `fullScaleRange()` 和 `sampleRate()`

### 检查项

- ✅ Profile 可转换为 AccelStreamProfile
- ✅ `fullScaleRange()` 返回有效的 OBAccelFullScaleRange 枚举值
- ✅ `sampleRate()` 返回有效的 OBAccelSampleRate 枚举值
- ✅ 量程值在合理范围内（如 ±2g, ±4g, ±8g, ±16g）
- ✅ 采样率值合理（如 100Hz, 200Hz, 400Hz 等）

### 预期结果

加速度计 Profile 的量程和采样率属性正确可读。

---

## TC_CPP_07_04 — GyroStreamProfile 属性

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_07_04 |
| 名称 | GyroStreamProfile 属性 |
| 目的 | 验证陀螺仪 StreamProfile 的量程和采样率属性可正确读取 |
| 覆盖功能点 | 07-050 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取陀螺仪传感器
3. 调用 `sensor->getStreamProfileList()` 获取 Profile 列表
4. 获取首个 Profile 并转换为 GyroStreamProfile
5. 读取 `fullScaleRange()` 和 `sampleRate()`

### 检查项

- ✅ Profile 可转换为 GyroStreamProfile
- ✅ `fullScaleRange()` 返回有效的 OBGyroFullScaleRange 枚举值
- ✅ `sampleRate()` 返回有效的 OBGyroSampleRate 枚举值
- ✅ 量程值在合理范围内（如 ±250dps, ±500dps 等）
- ✅ 采样率值合理（如 100Hz, 200Hz 等）

### 预期结果

陀螺仪 Profile 的量程和采样率属性正确可读。

---

## TC_CPP_07_05 — StreamProfile 类型检查与转换

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_07_05 |
| 名称 | StreamProfile 类型检查与转换 |
| 目的 | 验证 StreamProfile 的类型检查（is）和转换（as）接口工作正常 |
| 覆盖功能点 | 07-060 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取深度传感器及其 Profile 列表
3. 获取首个 Profile
4. 调用 `profile->is<VideoStreamProfile>()` 检查类型
5. 调用 `profile->as<VideoStreamProfile>()` 进行转换
6. 对加速度和陀螺仪传感器重复测试

### 检查项

- ✅ 深度 Profile 的 `is<VideoStreamProfile>()` 返回 true
- ✅ 深度 Profile 的 `as<VideoStreamProfile>()` 返回有效对象
- ✅ 加速度 Profile 的 `is<AccelStreamProfile>()` 返回 true
- ✅ 加速度 Profile 的 `as<AccelStreamProfile>()` 返回有效对象
- ✅ 陀螺仪 Profile 的 `is<GyroStreamProfile>()` 返回 true
- ✅ 陀螺仪 Profile 的 `as<GyroStreamProfile>()` 返回有效对象
- ✅ 错误类型转换返回 null 或抛异常（如 AccelProfile 转 VideoProfile）

### 预期结果

StreamProfile 的类型检查和转换接口工作正常，可安全地进行类型操作。
