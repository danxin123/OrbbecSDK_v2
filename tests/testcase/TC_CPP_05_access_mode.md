# TC_CPP_05 — 设备访问模式

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 4 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 05：设备访问模式** 的全部功能点：

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 05-010 | DEFAULT_ACCESS | 默认访问模式 | — | `DEFAULT_ACCESS` |
| 05-020 | EXCLUSIVE_ACCESS | 独占访问，其他进程无法打开 | — | `EXCLUSIVE_ACCESS` |
| 05-030 | SHARED_ACCESS | 共享访问，多进程可同时打开 | — | `SHARED_ACCESS` |
| 05-040 | CONTROL_ONLY_ACCESS | 仅控制，不可流传输 | — | `CONTROL_ONLY_ACCESS` |

---

## TC_CPP_05_01 — DEFAULT_ACCESS 打开设备

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_05_01 |
| 名称 | DEFAULT_ACCESS 打开设备 |
| 目的 | 验证使用默认访问模式可正常打开设备并进行流传输 |
| 覆盖功能点 | 05-010, 03-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0, OB_DEFAULT_ACCESS)` 以默认模式获取设备
3. 创建设备 Pipeline
4. 启用深度流配置
5. 调用 `pipeline->start(config)` 开流
6. 调用 `pipeline->waitForFrames(3000)` 等待帧数据
7. 停止 Pipeline

### 检查项

- ✅ `getDevice(0, OB_DEFAULT_ACCESS)` 返回非空 Device 对象
- ✅ Pipeline 创建成功
- ✅ `start(config)` 调用成功，不抛异常
- ✅ `waitForFrames()` 返回非空 FrameSet
- ✅ FrameSet 中包含有效的深度帧
- ✅ 设备正常关闭，无崩溃

### 预期结果

默认访问模式下设备可正常打开、开流、取帧，功能完整。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index, OBDeviceAccessMode mode)` |
| C++ | `ob::Pipeline::Pipeline(std::shared_ptr<Device> device)` |
| C | `ob_device* ob_device_list_get_device_ex(ob_device_list* list, uint32_t index, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 默认访问模式通常等同于独占模式
- 该用例为 P0 门禁用例，验证设备基本可用性

---

## TC_CPP_05_02 — EXCLUSIVE_ACCESS 独占

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_05_02 |
| 名称 | EXCLUSIVE_ACCESS 独占 |
| 目的 | 验证独占访问模式下设备被当前进程独占，其他进程无法同时打开 |
| 覆盖功能点 | 05-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0, OB_EXCLUSIVE_ACCESS)` 以独占模式获取设备
3. 创建设备 Pipeline 并启动流
4. 尝试在另一个进程或同进程使用 `getDevice(0)` 再次获取同一设备
5. 观察第二次获取的结果
6. 停止第一个 Pipeline 并释放设备
7. 再次尝试获取设备

### 检查项

- ✅ 第一次 `getDevice(0, OB_EXCLUSIVE_ACCESS)` 返回有效设备
- ✅ Pipeline 正常启动，可获取帧
- ✅ 第二次获取同一设备应失败或阻塞（返回 null/抛异常/超时）
- ✅ 释放第一个设备后，第二次获取成功
- ✅ C API：`ob_device_list_get_device_ex(list, 0, OB_DEVICE_ACCESS_EXCLUSIVE)` 行为一致

### 预期结果

独占模式下设备被当前持有者独占，其他尝试访问应失败；释放后可重新获取。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index, OBDeviceAccessMode mode)` |
| C | `ob_device* ob_device_list_get_device_ex(ob_device_list* list, uint32_t index, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 独占模式确保设备资源不被其他进程占用，适合高性能应用场景
- 第二次获取的行为取决于 SDK 实现：可能立即失败、阻塞等待或抛异常

---

## TC_CPP_05_03 — SHARED_ACCESS 共享

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_05_03 |
| 名称 | SHARED_ACCESS 共享 |
| 目的 | 验证共享访问模式下多个进程可同时读取设备属性 |
| 覆盖功能点 | 05-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0, OB_SHARED_ACCESS)` 以共享模式获取设备
3. 记录设备序列号等信息
4. 再次调用 `deviceList->getDevice(0, OB_SHARED_ACCESS)` 获取第二个设备句柄
5. 通过第二个句柄读取设备信息
6. 释放两个设备句柄

### 检查项

- ✅ 第一次 `getDevice(0, OB_SHARED_ACCESS)` 返回有效设备
- ✅ 第二次 `getDevice(0, OB_SHARED_ACCESS)` 返回有效设备（同一设备）
- ✅ 两个设备句柄均可读取设备属性（序列号、固件版本等）
- ✅ 两个句柄读取的属性值一致
- ✅ 释放句柄后设备正常关闭

### 预期结果

共享模式下多个进程/句柄可同时持有设备，均可读取属性。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index, OBDeviceAccessMode mode)` |
| C | `ob_device* ob_device_list_get_device_ex(ob_device_list* list, uint32_t index, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 共享模式允许多个进程同时读取设备信息，但通常只有一个进程能进行流传输
- 具体并发行为可能因设备型号和 SDK 版本而异

---

## TC_CPP_05_04 — CONTROL_ONLY_ACCESS 仅控制

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_05_04 |
| 名称 | CONTROL_ONLY_ACCESS 仅控制 |
| 目的 | 验证仅控制模式下可读写设备属性但不可开流 |
| 覆盖功能点 | 05-040 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0, OB_CONTROL_ONLY_ACCESS)` 以仅控制模式获取设备
3. 尝试读写设备属性（如激光开关）
4. 尝试创建 Pipeline 并配置流
5. 尝试启动流
6. 释放设备

### 检查项

- ✅ `getDevice(0, OB_CONTROL_ONLY_ACCESS)` 返回有效设备
- ✅ 设备属性可正常读写
- ✅ Pipeline 创建成功
- ✅ `start()` 调用应失败或抛出异常（流被禁用）
- ✅ 或 `start()` 成功但 `waitForFrames()` 超时/失败
- ✅ 释放设备后无崩溃

### 预期结果

仅控制模式下可进行设备控制，但无法获取流数据。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index, OBDeviceAccessMode mode)` |
| C | `ob_device* ob_device_list_get_device_ex(ob_device_list* list, uint32_t index, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 仅控制模式用于设备配置场景，不需要视频流
- 与其他模式组合使用时，CONTROL_ONLY 进程可进行参数调优，EXCLUSIVE/SHARED 进程负责取流
