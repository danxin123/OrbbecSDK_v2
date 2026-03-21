# TC_CPP_12 — 帧创建（FrameFactory）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 4 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 12：帧创建（FrameFactory）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 12-010 | 创建空帧 | `ob_create_frame(type, format, size)` | `FrameFactory::createFrame()` |
| 12-020 | 创建视频帧 | `ob_create_video_frame(type, fmt, w, h, stride)` | `FrameFactory::createVideoFrame()` |
| 12-030 | 克隆帧 | `ob_create_frame_from_other_frame(other, copy)` | `FrameFactory::createFrameFromOtherFrame()` |
| 12-040 | 从 Profile 创建 | `ob_create_frame_from_stream_profile()` | `FrameFactory::createFrameFromStreamProfile()` |
| 12-050 | 外部 Buffer 包装 | `ob_create_frame_from_buffer(...)` | `FrameFactory::createFrameFromBuffer()` |
| 12-060 | 外部 Buffer 包装(视频) | `ob_create_video_frame_from_buffer(...)` | `FrameFactory::createVideoFrameFromBuffer()` |
| 12-070 | 创建空 FrameSet | `ob_create_frameset()` | `FrameFactory::createFrameSet()` |

---

## TC_CPP_12_01 — 创建空帧与视频帧

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_12_01 |
| 名称 | 创建空帧与视频帧 |
| 目的 | 验证 FrameFactory 可创建空帧和视频帧 |
| 覆盖功能点 | 12-010, 12-020 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 调用 `FrameFactory::createFrame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640*480*2)`
2. 验证帧属性
3. 调用 `FrameFactory::createVideoFrame(OB_FRAME_DEPTH, OB_FORMAT_Y16, 640, 480, 640*2)`
4. 验证视频帧属性

### 检查项

- ✅ `createFrame()` 返回非空 Frame
- ✅ 帧类型和格式与创建参数一致
- ✅ `createVideoFrame()` 返回非空 VideoFrame
- ✅ 宽高和 stride 与创建参数一致

### 预期结果

可成功创建空帧和视频帧，属性与参数匹配。

---

## TC_CPP_12_02 — 克隆帧与从 Profile 创建

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_12_02 |
| 名称 | 克隆帧与从 Profile 创建 |
| 目的 | 验证帧克隆和从 Profile 创建帧功能 |
| 覆盖功能点 | 12-030, 12-040 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 启动 Pipeline 获取一帧
2. 调用 `FrameFactory::createFrameFromOtherFrame(frame, true)` 克隆帧
3. 验证克隆帧数据与原始帧一致
4. 获取 StreamProfile
5. 调用 `FrameFactory::createFrameFromStreamProfile(profile)`
6. 验证帧格式与 Profile 匹配

### 检查项

- ✅ 克隆帧返回非空 Frame
- ✅ 克隆帧数据与原始帧一致（copy=true）
- ✅ 从 Profile 创建的帧格式与 Profile 一致
- ✅ 创建的帧可正常使用

### 预期结果

帧克隆和从 Profile 创建功能正常工作。

---

## TC_CPP_12_03 — 外部 Buffer 包装

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_12_03 |
| 名称 | 外部 Buffer 包装 |
| 目的 | 验证可将外部内存缓冲区包装为 Frame |
| 覆盖功能点 | 12-050, 12-060 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 分配外部内存缓冲区
2. 定义 destroy callback 函数
3. 调用 `FrameFactory::createFrameFromBuffer(...)`
4. 使用 Frame
5. 释放 Frame，验证 callback 被调用

### 检查项

- ✅ `createFrameFromBuffer()` 返回非空 Frame
- ✅ Frame 数据指向外部缓冲区
- ✅ Frame 释放时 destroy callback 被调用
- ✅ `createVideoFrameFromBuffer()` 同样工作正常

### 预期结果

可将外部缓冲区安全包装为 Frame，生命周期管理正确。

---

## TC_CPP_12_04 — 创建空 FrameSet

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_12_04 |
| 名称 | 创建空 FrameSet |
| 目的 | 验证可创建空的 FrameSet 并动态添加帧 |
| 覆盖功能点 | 12-070 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 调用 `FrameFactory::createFrameSet()`
2. 验证 `frameCount() == 0`
3. 创建一帧
4. 调用 `pushFrame()` 添加
5. 验证 `frameCount() == 1`

### 检查项

- ✅ `createFrameSet()` 返回非空 FrameSet
- ✅ 初始帧计数为 0
- ✅ `pushFrame()` 后帧计数增加
- ✅ 可添加多帧

### 预期结果

可创建空 FrameSet 并动态添加帧。
