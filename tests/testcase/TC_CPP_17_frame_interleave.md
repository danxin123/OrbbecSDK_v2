# TC_CPP_17 — 帧交错（Frame Interleave）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 3 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 17：帧交错（Frame Interleave）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 17-010 | 支持查询 | `ob_device_is_frame_interleave_supported()` | `isFrameInterleaveSupported()` |
| 17-020 | 枚举模式列表 | `ob_device_get_available_frame_interleave_list()` | `getAvailableFrameInterleaveList()` |
| 17-030 | 加载交错模式 | `ob_device_load_frame_interleave()` | `loadFrameInterleave()` |
| 17-040 | 查询模式是否存在 | `ob_device_frame_interleave_list_has_frame_interleave()` | `hasFrameInterleave()` |

---

## TC_CPP_17_01 — 支持查询与模式枚举

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_17_01 |
| 名称 | 支持查询与模式枚举 |
| 目的 | 验证帧交错支持查询和模式枚举 |
| 覆盖功能点 | 17-010, 17-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `isFrameInterleaveSupported()`
3. 若支持，调用 `getAvailableFrameInterleaveList()`

### 检查项

- ✅ 返回是否支持帧交错
- ✅ 若支持，返回非空模式列表
- ✅ 列表中每项有有效名称

### 预期结果

帧交错支持状态和可用模式正确获取。

---

## TC_CPP_17_02 — 加载 Interleave 模式

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_17_02 |
| 名称 | 加载 Interleave 模式 |
| 目的 | 验证可加载帧交错模式 |
| 覆盖功能点 | 17-030, 17-040 |
| 前置条件 | 设备支持帧交错 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `hasFrameInterleave(name)` 验证模式存在
3. 调用 `loadFrameInterleave(name)`

### 检查项

- ✅ `hasFrameInterleave()` 返回 true
- ✅ `loadFrameInterleave()` 调用成功

### 预期结果

帧交错模式可成功加载。

---

## TC_CPP_17_03 — Interleave 开流验证

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_17_03 |
| 名称 | Interleave 开流验证 |
| 目的 | 验证加载帧交错模式后开流，帧交替输出 |
| 覆盖功能点 | 17-030 |
| 前置条件 | 设备支持帧交错 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 加载帧交错模式
3. 启动深度流
4. 获取多帧，检查 SequenceId 元数据

### 检查项

- ✅ 开流正常
- ✅ 帧的 SequenceId 元数据交替变化
- ✅ 不同 SequenceId 对应不同曝光/设置

### 预期结果

帧交错模式下，帧按序列 ID 交替输出。
