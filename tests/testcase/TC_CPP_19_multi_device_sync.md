# TC_CPP_19 — 多设备同步

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 7 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 19：多设备同步** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 19-010 | 支持模式查询 | `ob_device_get_supported_multi_device_sync_mode_bitmap()` | `getSupportedMultiDeviceSyncModeBitmap()` |
| 19-020 | 设置同步配置 | `ob_device_set_multi_device_sync_config()` | `setMultiDeviceSyncConfig()` |
| 19-030 | 获取同步配置 | `ob_device_get_multi_device_sync_config()` | `getMultiDeviceSyncConfig()` |
| 19-040 | 软件触发捕获 | `ob_device_trigger_capture()` | `triggerCapture()` |
| 19-050 | 时间戳重置配置 | `ob_device_set/get_timestamp_reset_config()` | `setTimestampResetConfig()` 等 |
| 19-060 | 时间戳重置 | `ob_device_timestamp_reset()` | `timestampReset()` |
| 19-070 | Host 时间同步 | `ob_device_timer_sync_with_host()` | `timerSyncWithHost()` |

---

## TC_CPP_19_01 — 同步模式支持与配置读取

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_01 |
| 名称 | 同步模式支持与配置读取 |
| 目的 | 验证同步模式支持查询和配置读取 |
| 覆盖功能点 | 19-010, 19-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `getSupportedMultiDeviceSyncModeBitmap()`
3. 调用 `getMultiDeviceSyncConfig()`

### 检查项

- ✅ 返回非零 bitmap（支持至少一种模式）
- ✅ 返回有效的同步配置
- ✅ 配置字段可读取

### 预期结果

同步模式支持和当前配置正确获取。

---

## TC_CPP_19_02 — 设置 FREE_RUN/STANDALONE 模式

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_02 |
| 名称 | 设置 FREE_RUN/STANDALONE 模式 |
| 目的 | 验证可设置 FREE_RUN 和 STANDALONE 同步模式 |
| 覆盖功能点 | 19-020, 19-100, 19-110 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 设置 `syncMode = FREE_RUN`
3. 调用 `setMultiDeviceSyncConfig(config)`
4. 验证设置成功
5. 设置 `syncMode = STANDALONE`
6. 重复验证

### 检查项

- ✅ FREE_RUN 模式设置成功
- ✅ STANDALONE 模式设置成功
- ✅ 设置后读取配置一致

### 预期结果

FREE_RUN 和 STANDALONE 模式可正常设置。

---

## TC_CPP_19_03 — PRIMARY/SECONDARY 配对

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_03 |
| 名称 | PRIMARY/SECONDARY 配对 |
| 目的 | 验证两台设备可配置为 PRIMARY 和 SECONDARY 配对同步 |
| 覆盖功能点 | 19-020, 19-120, 19-130 |
| 前置条件 | 两台设备通过 USB 连接 |
| 硬件依赖 | 2台 |
| 优先级 | P1 |

### 测试步骤

1. 获取两台设备
2. 设备 A 配置为 PRIMARY，`triggerOutEnable = true`
3. 设备 B 配置为 SECONDARY
4. 同步启动两台设备
5. 验证帧时间戳同步

### 检查项

- ✅ PRIMARY 配置成功
- ✅ SECONDARY 配置成功
- ✅ 两台设备帧时间戳同步
- ✅ 同步开流正常

### 预期结果

两台设备可配对同步，帧时间戳对齐。

---

## TC_CPP_19_04 — SOFTWARE_TRIGGERING 触发

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_04 |
| 名称 | SOFTWARE_TRIGGERING 触发 |
| 目的 | 验证软件触发模式下单次触发可获取帧 |
| 覆盖功能点 | 19-040, 19-150 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 配置为 SOFTWARE_TRIGGERING 模式
3. 调用 `triggerCapture()`
4. 验证收到帧

### 检查项

- ✅ 模式设置成功
- ✅ `triggerCapture()` 调用成功
- ✅ 触发后收到帧
- ✅ 多次触发多次收帧

### 预期结果

软件触发模式下可单次触发获取帧。

---

## TC_CPP_19_05 — 时间戳重置配置与执行

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_05 |
| 名称 | 时间戳重置配置与执行 |
| 目的 | 验证时间戳重置配置和执行 |
| 覆盖功能点 | 19-050, 19-060 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `setTimestampResetConfig(config)`
3. 调用 `getTimestampResetConfig()` 验证
4. 启动流
5. 调用 `timestampReset()`
6. 验证后续帧时间戳从小值开始

### 检查项

- ✅ 配置设置成功
- ✅ 配置读取一致
- ✅ 重置后帧时间戳重置

### 预期结果

时间戳重置配置和执行正常。

---

## TC_CPP_19_06 — Timer 与 Host 同步

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_06 |
| 名称 | Timer 与 Host 同步 |
| 目的 | 验证设备计时器与主机同步 |
| 覆盖功能点 | 19-070 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `timerSyncWithHost()`

### 检查项

- ✅ 调用成功
- ✅ 不崩溃

### 预期结果

设备计时器与主机同步接口正常。

---

## TC_CPP_19_07 — 同步配置字段验证

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_19_07 |
| 名称 | 同步配置字段验证 |
| 目的 | 验证同步配置各字段可正确设置和读取 |
| 覆盖功能点 | 19-200~19-260 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 设置完整的同步配置：
   - syncMode
   - depthDelayUs
   - colorDelayUs
   - trigger2ImageDelayUs
   - triggerOutDelayUs
   - framesPerTrigger
3. 读取验证

### 检查项

- ✅ 各字段可设置
- ✅ 设置后读取值一致

### 预期结果

同步配置各字段可正确设置和读取。
