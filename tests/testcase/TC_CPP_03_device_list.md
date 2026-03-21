# TC_CPP_03 — 设备列表查询

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-16 | — | 初始版本，包含 6 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 03：设备列表查询** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 03-010 | 设备数量 | `ob_device_list_get_count()` | `deviceCount()` |
| 03-020 | 按索引获取 | `ob_device_list_get_device(list, idx)` | `getDevice(idx)` |
| 03-030 | 按索引+访问模式获取 | `ob_device_list_get_device_ex(list, idx, mode)` | — |
| 03-040 | 按序列号查找 | `ob_device_list_get_device_by_serial_number()` | `getDeviceBySN()` |
| 03-050 | 按 UID 查找 | `ob_device_list_get_device_by_uid()` | `getDeviceByUid()` |
| 03-060 | 设备名称查询 | `ob_device_list_get_device_name()` | — |
| 03-070 | PID/VID 查询 | `ob_device_list_get_device_pid/vid()` | — |
| 03-080 | 连接类型查询 | `ob_device_list_get_device_connection_type()` | — |
| 03-090 | 网络信息查询 | `ob_device_list_get_device_ip_address/subnet_mask/gateway/local_mac/local_ip()` | — |

---

## TC_CPP_03_01 — 设备数量与索引访问

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_01 |
| 名称 | 设备数量与索引访问 |
| 目的 | 验证设备列表数量查询和按索引遍历设备的正确性 |
| 覆盖功能点 | 03-010, 03-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.queryDeviceList()` 获取设备列表
3. 读取 `deviceList->deviceCount()`
4. 遍历索引 0 到 count-1，调用 `deviceList->getDevice(i)` 获取每个设备
5. 对每个设备调用基本信息查询验证设备有效性

### 检查项

- ✅ `deviceCount()` > 0
- ✅ `deviceCount()` 返回值与实际连接设备数一致
- ✅ 遍历每个 `getDevice(i)` 返回非空 Device 对象
- ✅ 每个返回的 Device 可查询基本信息（不抛异常）
- ✅ C API：`ob_device_list_get_count(list, &error)` > 0，error 为 NULL
- ✅ C API：`ob_device_list_get_device(list, i, &error)` 返回非 NULL

### 预期结果

成功获取设备数量并通过索引遍历所有设备，每个设备对象有效可用。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `uint32_t ob::DeviceList::deviceCount()` |
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index)` |
| C | `uint32_t ob_device_list_get_count(ob_device_list* list, ob_error** error)` |
| C | `ob_device* ob_device_list_get_device(ob_device_list* list, uint32_t index, ob_error** error)` |

### 备注

- 多设备连接时应能枚举到所有设备
- 设备索引从 0 开始

---

## TC_CPP_03_02 — 带访问模式获取设备

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_02 |
| 名称 | 带访问模式获取设备 |
| 目的 | 验证以不同访问模式（独占/共享）从设备列表获取设备的功能 |
| 覆盖功能点 | 03-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 C API `ob_device_list_get_device_ex(list, 0, OB_DEVICE_ACCESS_EXCLUSIVE, &error)` 以独占模式获取首设备
3. 验证返回有效设备
4. 释放设备
5. 调用 C API `ob_device_list_get_device_ex(list, 0, OB_DEVICE_ACCESS_SHARED, &error)` 以共享模式获取首设备
6. 验证返回有效设备
7. 释放设备

### 检查项

- ✅ `EXCLUSIVE_ACCESS` 模式获取设备返回非 NULL
- ✅ 独占模式设备可执行基本查询操作
- ✅ `SHARED_ACCESS` 模式获取设备返回非 NULL
- ✅ 共享模式设备可执行基本查询操作
- ✅ C API：`ob_device_list_get_device_ex(list, idx, mode, &error)` 返回非 NULL，error 为 NULL

### 预期结果

以独占和共享两种模式均可成功获取设备。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C | `ob_device* ob_device_list_get_device_ex(ob_device_list* list, uint32_t index, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 此 API 为 C 接口专有，C++ 接口通过模块 05 的 Device 访问模式覆盖
- 独占模式下第二次获取同一设备应失败

---

## TC_CPP_03_03 — 按序列号/UID 查找

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_03 |
| 名称 | 按序列号/UID 查找 |
| 目的 | 验证按设备序列号和唯一 ID 定位设备的功能 |
| 覆盖功能点 | 03-040, 03-050 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 获取首设备 `device0 = deviceList->getDevice(0)`
3. 读取首设备的序列号 `sn = deviceInfo->serialNumber()` 和 UID `uid = deviceInfo->uid()`
4. 调用 `deviceList->getDeviceBySN(sn)` 按序列号查找
5. 验证返回设备的 SN 与原始 SN 一致
6. 调用 `deviceList->getDeviceByUid(uid)` 按 UID 查找
7. 验证返回设备的 UID 与原始 UID 一致

### 检查项

- ✅ `getDeviceBySN(sn)` 返回非空 Device 对象
- ✅ 按 SN 查找返回的设备序列号与原始 SN 完全一致
- ✅ `getDeviceByUid(uid)` 返回非空 Device 对象
- ✅ 按 UID 查找返回的设备 UID 与原始 UID 完全一致
- ✅ 使用不存在的 SN 查找应返回 null 或抛异常
- ✅ 使用不存在的 UID 查找应返回 null 或抛异常
- ✅ C API：`ob_device_list_get_device_by_serial_number(list, sn, &error)` 返回非 NULL
- ✅ C API：`ob_device_list_get_device_by_uid(list, uid, &error)` 返回非 NULL

### 预期结果

按 SN 和 UID 均可准确定位到目标设备，返回的设备信息与预期一致。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDeviceBySN(const char* sn)` |
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDeviceByUid(const char* uid)` |
| C | `ob_device* ob_device_list_get_device_by_serial_number(ob_device_list* list, const char* sn, ob_error** error)` |
| C | `ob_device* ob_device_list_get_device_by_uid(ob_device_list* list, const char* uid, ob_error** error)` |

### 备注

- SN 是设备出厂写入的唯一序列号
- UID 是 SDK 内部生成的唯一标识（通常包含 USB 总线信息）
- 多设备场景下此功能用于定位特定设备

---

## TC_CPP_03_04 — 设备基本信息字段

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_04 |
| 名称 | 设备基本信息字段 |
| 目的 | 验证通过设备列表查询设备名称、PID、VID、连接类型等基本信息 |
| 覆盖功能点 | 03-060, 03-070, 03-080 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 对首设备（index=0）调用以下 C API 查询：
   - `ob_device_list_get_device_name(list, 0, &error)` 获取设备名称
   - `ob_device_list_get_device_pid(list, 0, &error)` 获取 PID
   - `ob_device_list_get_device_vid(list, 0, &error)` 获取 VID
   - `ob_device_list_get_device_connection_type(list, 0, &error)` 获取连接类型

### 检查项

- ✅ `getDeviceName()` 返回非空字符串（如 "Gemini 335"）
- ✅ `getDevicePid()` > 0（Gemini 335 系列有对应 PID）
- ✅ `getDeviceVid()` == 0x2BC5（Orbbec 厂商 ID）
- ✅ `getDeviceConnectionType()` 返回 `"USB"` 或 `"Ethernet"`
- ✅ USB 连接设备的 connectionType 为 `"USB"`
- ✅ 所有查询 error 均为 NULL

### 预期结果

各基本信息字段返回合理值：名称非空、PID/VID 有效、连接类型正确。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C | `const char* ob_device_list_get_device_name(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `int ob_device_list_get_device_pid(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `int ob_device_list_get_device_vid(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `const char* ob_device_list_get_device_connection_type(ob_device_list* list, uint32_t index, ob_error** error)` |

### 备注

- 这些为 C API 专用的设备列表级别查询，无需先打开设备即可获取信息
- VID 0x2BC5 为 Orbbec 在 USB-IF 注册的厂商 ID
- C++ 接口通过 DeviceInfo 对象获取同等信息（见模块 04）

---

## TC_CPP_03_05 — 网络设备信息字段

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_05 |
| 名称 | 网络设备信息字段 |
| 目的 | 验证网络设备的 IP 地址、子网掩码、网关、MAC 和本地 IP 信息查询 |
| 覆盖功能点 | 03-090 |
| 前置条件 | 一台 Gemini 335Le 通过以太网连接并已被 SDK 发现 |
| 硬件依赖 | 335Le |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象，启用网络设备枚举
2. 查询设备列表，定位网络设备（connectionType == "Ethernet"）
3. 对网络设备调用以下 C API 查询：
   - `ob_device_list_get_device_ip_address(list, idx, &error)`
   - `ob_device_list_get_device_subnet_mask(list, idx, &error)`
   - `ob_device_list_get_device_gateway(list, idx, &error)`
   - `ob_device_list_get_device_local_mac(list, idx, &error)`
   - `ob_device_list_get_device_local_ip(list, idx, &error)`

### 检查项

- ✅ `getDeviceIpAddress()` 返回非空字符串，格式为有效 IPv4（如 `"192.168.1.10"`）
- ✅ `getDeviceSubnetMask()` 返回非空字符串，格式为有效子网掩码（如 `"255.255.255.0"`）
- ✅ `getDeviceGateway()` 返回非空字符串，格式为有效 IPv4
- ✅ `getDeviceLocalMac()` 返回非空字符串，格式为 MAC 地址（如 `"AA:BB:CC:DD:EE:FF"`）
- ✅ `getDeviceLocalIp()` 返回非空字符串，格式为有效 IPv4（本机网卡 IP）
- ✅ IP 地址格式验证：匹配 `^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$`，每段 0-255
- ✅ MAC 地址格式验证：匹配 `^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$`
- ✅ 所有查询 error 均为 NULL

### 预期结果

网络设备的各网络信息字段均非空且格式合法。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C | `const char* ob_device_list_get_device_ip_address(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `const char* ob_device_list_get_device_subnet_mask(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `const char* ob_device_list_get_device_gateway(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `const char* ob_device_list_get_device_local_mac(ob_device_list* list, uint32_t index, ob_error** error)` |
| C | `const char* ob_device_list_get_device_local_ip(ob_device_list* list, uint32_t index, ob_error** error)` |

### 备注

- 仅适用于网络设备（Gemini 335Le），对 USB 设备调用这些 API 可能返回空或抛异常
- 需要 335Le 专用 runner 和网络环境
- `local_mac` 和 `local_ip` 指的是主机端网卡信息，非设备端

---

## TC_CPP_03_06 — 越界索引安全

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_03_06 |
| 名称 | 越界索引安全 |
| 目的 | 验证使用越界索引访问设备列表时 SDK 的安全处理 |
| 覆盖功能点 | 03-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 读取 `count = deviceList->deviceCount()`
3. 调用 `deviceList->getDevice(count)` 使用等于 count 的越界索引
4. 捕获异常或检查返回值
5. 调用 `deviceList->getDevice(count + 100)` 使用远超范围的索引
6. 捕获异常或检查返回值
7. 调用 `deviceList->getDevice(UINT32_MAX)` 使用最大值索引
8. 捕获异常或检查返回值

### 检查项

- ✅ `getDevice(count)` 抛出异常或返回 nullptr，**不崩溃**
- ✅ `getDevice(count + 100)` 抛出异常或返回 nullptr，**不崩溃**
- ✅ `getDevice(UINT32_MAX)` 抛出异常或返回 nullptr，**不崩溃**
- ✅ 异常类型为 `ob::Error`，异常消息包含有意义的描述
- ✅ 越界访问后设备列表仍可正常使用（`getDevice(0)` 仍返回有效设备）
- ✅ C API：`ob_device_list_get_device(list, count, &error)` 返回 NULL 且 error 非 NULL

### 预期结果

越界索引安全处理，抛异常或返回 null，不导致崩溃或未定义行为。越界后列表仍可正常使用。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index)` |
| C | `ob_device* ob_device_list_get_device(ob_device_list* list, uint32_t index, ob_error** error)` |

### 备注

- 此用例为边界安全用例，属于 P0 级别
- 确保 SDK 不会因用户传入无效索引而导致段错误
- 与模块 24（错误处理）有交叉，验证异常类型可选
