# TC_CPP_13 — 滤波器（Filter）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 17 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 13：滤波器（Filter）** 的全部功能点。

---

## TC_CPP_13_01 — Filter 按名称创建全类型

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_01 |
| 名称 | Filter 按名称创建全类型 |
| 目的 | 验证可按名称创建所有内置 Filter 类型 |
| 覆盖功能点 | 13.1-010 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P0 |

### 测试步骤

1. 依次按名称创建所有 Filter：
   - "PointCloudFilter"
   - "Align"
   - "FormatConverter"
   - "HDRMerge"
   - "SequenceIdFilter"
   - "DecimationFilter"
   - "ThresholdFilter"
   - "SpatialAdvancedFilter"
   - "SpatialFastFilter"
   - "SpatialModerateFilter"
   - "HoleFillingFilter"
   - "NoiseRemovalFilter"
   - "TemporalFilter"
   - "FalsePositiveFilter"
   - "DisparityTransform"

### 检查项

- ✅ 所有已知 Filter 名称均可成功创建
- ✅ 创建的 Filter 对象非空
- ✅ 无异常抛出

### 预期结果

所有内置 Filter 可按名称成功创建。

---

## TC_CPP_13_02 — Filter 创建无效名称

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_02 |
| 名称 | Filter 创建无效名称 |
| 目的 | 验证创建无效名称 Filter 时抛出异常 |
| 覆盖功能点 | 13.1-010 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P0 |

### 测试步骤

1. 调用 `Filter("NonExistent")`

### 检查项

- ✅ 抛出 `ob::Error` 异常
- ✅ 异常状态码非零
- ✅ **不崩溃**

### 预期结果

无效 Filter 名称触发异常，SDK 安全处理。

---

## TC_CPP_13_03 — Filter 同步处理流程

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_03 |
| 名称 | Filter 同步处理流程 |
| 目的 | 验证 Filter 同步处理接口 |
| 覆盖功能点 | 13.1-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 启动深度流获取一帧
2. 创建 Filter（如 PointCloudFilter）
3. 调用 `filter->process(frame)`
4. 验证输出帧

### 检查项

- ✅ `process()` 返回非空帧
- ✅ 输出帧格式符合 Filter 定义
- ✅ 处理过程不崩溃

### 预期结果

Filter 同步处理正常工作，输入输出符合预期。

---

## TC_CPP_13_04 — Filter 异步回调处理

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_04 |
| 名称 | Filter 异步回调处理 |
| 目的 | 验证 Filter 异步回调处理接口 |
| 覆盖功能点 | 13.1-040 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 Filter
2. 调用 `filter->setCallback(callback)` 设置回调
3. 调用 `filter->pushFrame(frame)` 推入帧
4. 等待回调触发

### 检查项

- ✅ `setCallback()` 调用成功
- ✅ `pushFrame()` 调用成功
- ✅ 回调被触发，收到处理后的帧
- ✅ 输出帧格式正确

### 预期结果

Filter 异步回调处理正常工作。

---

## TC_CPP_13_05 — Filter enable/disable 开关

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_05 |
| 名称 | Filter enable/disable 开关 |
| 目的 | 验证 Filter 启用/禁用开关功能 |
| 覆盖功能点 | 13.1-050, 13.1-060 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 Filter
2. 检查 `isEnabled()` 默认状态
3. 调用 `enable(false)` 禁用
4. 验证 `isEnabled()` 返回 false
5. `process()` 应原样输出
6. 调用 `enable(true)` 启用
7. 验证 `isEnabled()` 返回 true

### 检查项

- ✅ 默认状态符合预期
- ✅ `enable(false)` 后 `isEnabled()` 为 false
- ✅ 禁用后处理直通（原样输出）
- ✅ `enable(true)` 后 `isEnabled()` 为 true
- ✅ 启用后正常处理

### 预期结果

Filter 启用/禁用开关状态正确，行为符合状态。

---

## TC_CPP_13_06 — Filter reset 与配置

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_06 |
| 名称 | Filter reset 与配置 |
| 目的 | 验证 Filter reset 和配置接口 |
| 覆盖功能点 | 13.1-070~13.1-100 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 Filter
2. 调用 `reset()`
3. 调用 `getConfigSchema()` / `getConfigSchemaVec()` 获取配置 schema
4. 调用 `getConfigValue(name)` 读取默认值
5. 调用 `setConfigValue(name, val)` 设置新值
6. 调用 `getConfigValue(name)` 验证新值

### 检查项

- ✅ `reset()` 调用不崩溃
- ✅ `getConfigSchema()` 返回非空
- ✅ `getConfigValue()` 返回默认值
- ✅ `setConfigValue()` 调用成功
- ✅ 设置后 `getConfigValue()` 返回新值

### 预期结果

Filter reset 和配置接口工作正常。

---

## TC_CPP_13_07 — Filter 类型检查与转换

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_07 |
| 名称 | Filter 类型检查与转换 |
| 目的 | 验证 Filter 类型检查和转换接口 |
| 覆盖功能点 | 13.1-110 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 PointCloudFilter
2. 调用 `filter->is<PointCloudFilter>()`
3. 调用 `filter->as<PointCloudFilter>()`
4. 对其他 Filter 类型重复

### 检查项

- ✅ `is<PointCloudFilter>()` 返回 true
- ✅ `as<PointCloudFilter>()` 返回有效对象
- ✅ 错误类型转换返回 null 或抛异常

### 预期结果

Filter 类型检查和转换接口工作正常。

---

## TC_CPP_13_08 — PointCloudFilter XYZ/XYZRGB

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_08 |
| 名称 | PointCloudFilter XYZ/XYZRGB |
| 目的 | 验证 PointCloudFilter 生成 XYZ 和 XYZRGB 点云 |
| 覆盖功能点 | 13.2-010 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 PointCloudFilter
2. 设置 `setCreatePointFormat(OB_FORMAT_POINT)` XYZ 模式
3. 输入深度帧，`process()` 生成点云
4. 设置 `setCreatePointFormat(OB_FORMAT_RGB_POINT)` XYZRGB 模式
5. 输入对齐的 FrameSet，`process()` 生成彩色点云

### 检查项

- ✅ XYZ 模式：输出为 PointsFrame
- ✅ XYZRGB 模式：输出为 PointsFrame，包含颜色
- ✅ 点云坐标有效

### 预期结果

PointCloudFilter 可生成 XYZ 和 XYZRGB 两种格式的点云。

---

## TC_CPP_13_09 — Align Filter D2C/C2D

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_09 |
| 名称 | Align Filter D2C/C2D |
| 目的 | 验证 Align Filter 的 D2C 和 C2D 对齐 |
| 覆盖功能点 | 13.2-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 Align Filter，`Align(OB_STREAM_COLOR)` D2C
2. 输入深度帧，验证输出分辨率与彩色一致
3. 创建 Align Filter，`Align(OB_STREAM_DEPTH)` C2D
4. 输入彩色帧，验证输出分辨率与深度一致

### 检查项

- ✅ D2C：输出深度分辨率与彩色一致
- ✅ C2D：输出彩色分辨率与深度一致
- ✅ 对齐后数据有效

### 预期结果

Align Filter 可实现 D2C 和 C2D 两种对齐。

---

## TC_CPP_13_10 — FormatConverter 格式转换

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_10 |
| 名称 | FormatConverter 格式转换 |
| 目的 | 验证 FormatConverter 的各种格式转换 |
| 覆盖功能点 | 13.2-030, 13.3-010~13.3-220 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 FormatConverter
2. 测试常见转换：
   - YUYV → RGB
   - MJPG → RGB
   - MJPG → BGR
3. 验证输出格式

### 检查项

- ✅ 各种格式转换调用成功
- ✅ 输出帧格式与目标格式一致
- ✅ 输出图像内容合理

### 预期结果

FormatConverter 可完成各种像素格式转换。

---

## TC_CPP_13_11 — HDRMerge + SequenceIdFilter

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_11 |
| 名称 | HDRMerge + SequenceIdFilter |
| 目的 | 验证 HDRMerge 和 SequenceIdFilter 功能 |
| 覆盖功能点 | 13.2-040, 13.2-050 |
| 前置条件 | 至少一台设备通过 USB 连接，支持 HDR |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 SequenceIdFilter，设置 `selectSequenceId(0)`
2. 推入不同序列 ID 的帧，验证仅通过对应 ID 帧
3. 创建 HDRMerge
4. 输入多曝光序列帧，验证合并输出

### 检查项

- ✅ SequenceIdFilter 仅通过选定 ID 的帧
- ✅ HDRMerge 合并多曝光序列为单帧
- ✅ 输出 HDR 帧质量提升

### 预期结果

HDRMerge 和 SequenceIdFilter 正常工作。

---

## TC_CPP_13_12 — DecimationFilter 降采样

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_12 |
| 名称 | DecimationFilter 降采样 |
| 目的 | 验证 DecimationFilter 降采样功能 |
| 覆盖功能点 | 13.2-060 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 DecimationFilter
2. 调用 `setScaleValue(2)`
3. 输入深度帧
4. 验证输出分辨率

### 检查项

- ✅ 输出分辨率为输入的 1/2
- ✅ 深度数据有效

### 预期结果

DecimationFilter 按设置比例降采样。

---

## TC_CPP_13_13 — ThresholdFilter 深度阈值

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_13 |
| 名称 | ThresholdFilter 深度阈值 |
| 目的 | 验证 ThresholdFilter 深度值范围过滤 |
| 覆盖功能点 | 13.2-070 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 ThresholdFilter
2. 调用 `setValueRange(100, 5000)`
3. 输入深度帧
4. 验证输出深度值范围

### 检查项

- ✅ 输出深度值在 [100, 5000] 范围内
- ✅ 超出范围值被过滤（设为 0 或特定值）

### 预期结果

ThresholdFilter 按设置范围过滤深度值。

---

## TC_CPP_13_14 — 空间滤波器组

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_14 |
| 名称 | 空间滤波器组 |
| 目的 | 验证空间滤波器组功能 |
| 覆盖功能点 | 13.2-080~13.2-100 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 依次创建各空间滤波器：
   - SpatialAdvancedFilter（需密钥）
   - SpatialFastFilter
   - SpatialModerateFilter
2. 输入深度帧处理
3. 验证输出

### 检查项

- ✅ 各滤波器创建成功
- ✅ 处理后的深度帧有效
- ✅ 滤波效果明显

### 预期结果

空间滤波器组各滤波器工作正常。

---

## TC_CPP_13_15 — 时域/空洞/降噪滤波器组

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_15 |
| 名称 | 时域/空洞/降噪滤波器组 |
| 目的 | 验证 TemporalFilter、HoleFillingFilter、NoiseRemovalFilter |
| 覆盖功能点 | 13.2-110~13.2-130 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建并测试各滤波器：
   - HoleFillingFilter：输入有空洞的深度帧
   - NoiseRemovalFilter：输入有噪点的深度帧
   - TemporalFilter：连续输入多帧
2. 观察处理效果

### 检查项

- ✅ HoleFillingFilter 减少深度图空洞
- ✅ NoiseRemovalFilter 减少噪点
- ✅ TemporalFilter 实现时域平滑

### 预期结果

时域、空洞填充、降噪滤波器工作正常。

---

## TC_CPP_13_16 — FalsePositiveFilter + DisparityTransform

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_16 |
| 名称 | FalsePositiveFilter + DisparityTransform |
| 目的 | 验证 FalsePositiveFilter 和 DisparityTransform |
| 覆盖功能点 | 13.2-140, 13.2-150 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 FalsePositiveFilter，输入有虚假深度的帧
2. 验证虚假深度被去除
3. 创建 DisparityTransform
4. 深度 → 视差 → 深度 往返转换

### 检查项

- ✅ FalsePositiveFilter 去除虚假深度值
- ✅ DisparityTransform 深度↔视差转换正确
- ✅ 往返后深度值接近原始值

### 预期结果

FalsePositiveFilter 和 DisparityTransform 工作正常。

---

## TC_CPP_13_17 — 私有 Filter 创建

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_13_17 |
| 名称 | 私有 Filter 创建 |
| 目的 | 验证私有 Filter 创建接口 |
| 覆盖功能点 | 13.1-020 |
| 前置条件 | SDK 可用，有有效的激活密钥 |
| 硬件依赖 | 否 |
| 优先级 | P2 |

### 测试步骤

1. 调用 `FilterFactory::createPrivateFilter(name, validKey)` 有效密钥
2. 验证创建成功
3. 调用 `createPrivateFilter(name, invalidKey)` 无效密钥
4. 验证抛出异常

### 检查项

- ✅ 有效密钥创建成功
- ✅ 无效密钥抛出异常
- ✅ **不崩溃**

### 预期结果

私有 Filter 创建需要有效激活密钥。
