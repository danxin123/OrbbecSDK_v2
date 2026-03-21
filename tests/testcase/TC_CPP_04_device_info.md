# TC_CPP_04 — 设备信息

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 5 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 04：设备信息** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 04-010 | 设备名称 | `ob_device_info_get_name()` | `name()` |
| 04-020 | 序列号 | `ob_device_info_get_serial_number()` | `serialNumber()` |
| 04-030 | 固件版本 | `ob_device_info_get_firmware_version()` | `firmwareVersion()` |
| 04-040 | 硬件版本 | `ob_device_info_get_hardware_version()` | `hardwareVersion()` |
| 04-050 | PID/VID/UID | `ob_device_info_get_pid/vid/uid()` | `pid()` / `vid()` / `uid()` |
| 04-060 | 连接类型 | `ob_device_info_get_connection_type()` | `connectionType()` |
| 04-070 | IP 地址信息 | `ob_device_info_get_ip_address/subnet_mask/gateway()` | `ipAddress()` 等 |
| 04-080 | ASIC 名称 | `ob_device_info_get_asicName()` | `asicName()` |
| 04-090 | 设备类型 | `ob_device_info_get_device_type()` | `deviceType()` |
| 04-100 | 最低 SDK 版本 | `ob_device_info_get_supported_min_sdk_version()` | `supportedMinSdkVersion()` |
| 04-110 | 扩展信息 | `ob_device_is_extension_info_exist()` / `ob_device_get_extension_info()` | 对应方法 |

---

## TC_CPP_04_01 — 基本信息字段

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_04_01 |
| 名称 | 基本信息字段 |
| 目的 | 验证设备基本信息字段（名称、序列号、固件版本、硬件版本）可正确读取且格式合法 |
| 覆盖功能点 | 04-010, 04-020, 04-030, 04-040 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0)` 获取首设备
3. 调用 `device->getDeviceInfo()` 获取 DeviceInfo 对象
4. 读取以下字段：
   - `deviceInfo->name()` 获取设备名称
   - `deviceInfo->serialNumber()` 获取序列号
   - `deviceInfo->firmwareVersion()` 获取固件版本
   - `deviceInfo->hardwareVersion()` 获取硬件版本
5. 验证各字段格式和内容

### 检查项

- ✅ `name()` 返回非空字符串（如 "Gemini 335"）
- ✅ `serialNumber()` 返回非空字符串，为唯一设备标识
- ✅ `firmwareVersion()` 返回非空字符串，格式匹配 `x.y.z` 或 `x.y.z.w`（如 "1.2.3"）
- ✅ `hardwareVersion()` 返回非空字符串
- ✅ 固件版本字符串可通过正则表达式 `^\d+\.\d+\.\d+` 验证
- ✅ C API：`ob_device_info_get_name/info)` 返回非 NULL 字符串
- ✅ C API：`ob_device_info_get_serial_number(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_firmware_version(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_hardware_version(info)` 返回非 NULL

### 预期结果

各基本信息字段均返回非空有效字符串，固件版本符合版本号格式规范。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `const char* ob::DeviceInfo::name()` |
| C++ | `const char* ob::DeviceInfo::serialNumber()` |
| C++ | `const char* ob::DeviceInfo::firmwareVersion()` |
| C++ | `const char* ob::DeviceInfo::hardwareVersion()` |
| C | `const char* ob_device_info_get_name(ob_device_info* info)` |
| C | `const char* ob_device_info_get_serial_number(ob_device_info* info)` |
| C | `const char* ob_device_info_get_firmware_version(ob_device_info* info)` |
| C | `const char* ob_device_info_get_hardware_version(ob_device_info* info)` |

### 备注

- 设备名称通常为产品型号，如 "Gemini 335"、"Gemini 335Le" 等
- 序列号为出厂烧录的唯一标识，用于设备追溯
- 固件版本格式为主版本.次版本.补丁版本（如 1.2.3）
- 该用例为 P0 门禁用例，发版前必须通过

---

## TC_CPP_04_02 — 标识字段

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_04_02 |
| 名称 | 标识字段 |
| 目的 | 验证设备标识字段（PID、VID、UID、连接类型）可正确读取且值合法 |
| 覆盖功能点 | 04-050, 04-060 |
| 前置条件 | 至少一台设备通过 USB 或以太网连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0)` 获取首设备
3. 调用 `device->getDeviceInfo()` 获取 DeviceInfo 对象
4. 读取以下字段：
   - `deviceInfo->pid()` 获取产品 ID
   - `deviceInfo->vid()` 获取厂商 ID
   - `deviceInfo->uid()` 获取唯一标识符
   - `deviceInfo->connectionType()` 获取连接类型
5. 验证各字段值

### 检查项

- ✅ `pid()` > 0（Gemini 335 系列有对应 PID）
- ✅ `vid()` == 0x2BC5（Orbbec 厂商 ID）
- ✅ `uid()` 返回非空字符串，唯一标识设备实例
- ✅ `connectionType()` 返回 `"USB"` 或 `"Ethernet"`
- ✅ USB 连接设备的 connectionType 为 `"USB"`
- ✅ 网络连接设备的 connectionType 为 `"Ethernet"`
- ✅ C API：`ob_device_info_get_pid(info)` 返回 > 0
- ✅ C API：`ob_device_info_get_vid(info)` 返回 0x2BC5
- ✅ C API：`ob_device_info_get_uid(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_connection_type(info)` 返回非 NULL

### 预期结果

各标识字段返回合法值：PID/VID 有效、UID 非空、连接类型与物理连接一致。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `int ob::DeviceInfo::pid()` |
| C++ | `int ob::DeviceInfo::vid()` |
| C++ | `const char* ob::DeviceInfo::uid()` |
| C++ | `const char* ob::DeviceInfo::connectionType()` |
| C | `int ob_device_info_get_pid(ob_device_info* info)` |
| C | `int ob_device_info_get_vid(ob_device_info* info)` |
| C | `const char* ob_device_info_get_uid(ob_device_info* info)` |
| C | `const char* ob_device_info_get_connection_type(ob_device_info* info)` |

### 备注

- VID 0x2BC5 为 Orbbec 在 USB-IF 注册的厂商 ID
- UID 是 SDK 内部生成的唯一标识，通常包含 USB 总线/端口信息
- PID 因设备型号而异（Gemini 335 系列不同型号有不同 PID）
- 该用例为 P0 门禁用例，发版前必须通过

---

## TC_CPP_04_03 — 网络设备 IP 信息

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_04_03 |
| 名称 | 网络设备 IP 信息 |
| 目的 | 验证网络设备的 IP 地址、子网掩码、网关信息可正确读取且格式合法 |
| 覆盖功能点 | 04-070 |
| 前置条件 | 一台 Gemini 335Le 通过以太网连接并已被 SDK 发现 |
| 硬件依赖 | 335Le |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象，启用网络设备枚举
2. 查询设备列表，定位网络设备（connectionType == "Ethernet"）
3. 调用 `device->getDeviceInfo()` 获取 DeviceInfo 对象
4. 读取以下字段：
   - `deviceInfo->ipAddress()` 获取 IP 地址
   - `deviceInfo->subnetMask()` 获取子网掩码
   - `deviceInfo->gateway()` 获取网关地址
5. 验证各字段格式

### 检查项

- ✅ `ipAddress()` 返回非空字符串，格式为有效 IPv4（如 `"192.168.1.10"`）
- ✅ `subnetMask()` 返回非空字符串，格式为有效子网掩码（如 `"255.255.255.0"`）
- ✅ `gateway()` 返回非空字符串，格式为有效 IPv4
- ✅ IP 地址格式验证：匹配 `^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$`，每段 0-255
- ✅ C API：`ob_device_info_get_ip_address(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_subnet_mask(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_gateway(info)` 返回非 NULL
- ✅ 所有 C API 调用 error 均为 NULL

### 预期结果

网络设备的各 IP 信息字段均非空且格式合法，符合 IPv4 地址规范。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `const char* ob::DeviceInfo::ipAddress()` |
| C++ | `const char* ob::DeviceInfo::subnetMask()` |
| C++ | `const char* ob::DeviceInfo::gateway()` |
| C | `const char* ob_device_info_get_ip_address(ob_device_info* info)` |
| C | `const char* ob_device_info_get_subnet_mask(ob_device_info* info)` |
| C | `const char* ob_device_info_get_gateway(ob_device_info* info)` |

### 备注

- 仅适用于网络设备（Gemini 335Le），对 USB 设备调用这些 API 可能返回空或抛异常
- 需要 335Le 专用 runner 和网络环境
- 若设备通过 USB 连接，此用例应跳过或返回预期的不支持结果

---

## TC_CPP_04_04 — 芯片与类型信息

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_04_04 |
| 名称 | 芯片与类型信息 |
| 目的 | 验证 ASIC 名称、设备类型、最低 SDK 版本等信息可正确读取 |
| 覆盖功能点 | 04-080, 04-090, 04-100 |
| 前置条件 | 至少一台设备通过 USB 或以太网连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0)` 获取首设备
3. 调用 `device->getDeviceInfo()` 获取 DeviceInfo 对象
4. 读取以下字段：
   - `deviceInfo->asicName()` 获取 ASIC 芯片名称
   - `deviceInfo->deviceType()` 获取设备类型枚举
   - `deviceInfo->supportedMinSdkVersion()` 获取最低 SDK 版本
5. 验证各字段值

### 检查项

- ✅ `asicName()` 返回非空字符串且不抛异常（如 "MX6000"、"MX6600" 等）
- ✅ `deviceType()` 返回有效 `OBDeviceType` 枚举值（如 `OB_DEVICE_GEMINI_335`）
- ✅ `supportedMinSdkVersion()` 返回非空字符串且不抛异常
- ✅ ASIC 名称与设备型号对应关系合理
- ✅ C API：`ob_device_info_get_asic_name(info)` 返回非 NULL
- ✅ C API：`ob_device_info_get_device_type(info)` 返回有效枚举值
- ✅ C API：`ob_device_info_get_supported_min_sdk_version(info)` 返回非 NULL
- ✅ 所有 C API 调用 error 均为 NULL

### 预期结果

各芯片与类型信息字段返回有效值，不抛异常。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `const char* ob::DeviceInfo::asicName()` |
| C++ | `OBDeviceType ob::DeviceInfo::deviceType()` |
| C++ | `const char* ob::DeviceInfo::supportedMinSdkVersion()` |
| C | `const char* ob_device_info_get_asic_name(ob_device_info* info)` |
| C | `ob_device_type ob_device_info_get_device_type(ob_device_info* info)` |
| C | `const char* ob_device_info_get_supported_min_sdk_version(ob_device_info* info)` |

### 备注

- ASIC 名称为设备主芯片型号，不同设备型号使用不同芯片
- 设备类型枚举定义了已知的 Orbbec 设备型号
- 最低 SDK 版本指示设备固件所需的最低 SDK 版本要求

---

## TC_CPP_04_05 — 扩展信息查询

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_04_05 |
| 名称 | 扩展信息查询 |
| 目的 | 验证设备扩展信息（键值对形式）的查询接口工作正常 |
| 覆盖功能点 | 04-110 |
| 前置条件 | 至少一台设备通过 USB 或以太网连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并查询设备列表
2. 调用 `deviceList->getDevice(0)` 获取首设备
3. 调用 `device->isExtensionInfoExist(key)` 对已知扩展信息 key 进行存在性检查
4. 若存在，调用 `device->getExtensionInfo(key)` 读取扩展信息值
5. 对未知的扩展信息 key 进行存在性检查
6. 验证返回结果

### 检查项

- ✅ 对已知 key（如 `"ProductName"`、`"ChipType"` 等，具体 key 依设备而定）`isExtensionInfoExist(key)` 返回 true
- ✅ 对已存在的 key，`getExtensionInfo(key)` 返回非空字符串
- ✅ 对未知 key，`isExtensionInfoExist(key)` 返回 false
- ✅ 对未知 key，`getExtensionInfo(key)` 返回空字符串或抛出异常，不崩溃
- ✅ C API：`ob_device_is_extension_info_exist(dev, key)` 返回布尔值
- ✅ C API：`ob_device_get_extension_info(dev, key)` 返回字符串或 NULL
- ✅ 所有 C API 调用 error 均为 NULL 或为预期错误

### 预期结果

扩展信息查询接口正常工作：存在的 key 返回有效值，不存在的 key 安全处理。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `bool ob::Device::isExtensionInfoExist(const char* key)` |
| C++ | `const char* ob::Device::getExtensionInfo(const char* key)` |
| C | `bool ob_device_is_extension_info_exist(ob_device* dev, const char* key, ob_error** error)` |
| C | `const char* ob_device_get_extension_info(ob_device* dev, const char* key, ob_error** error)` |

### 备注

- 扩展信息为设备提供的额外键值对属性，key 和 value 均为字符串
- 常见的扩展信息 key 可能包括：ProductName、ChipType、SensorType 等
- 扩展信息的存在性和具体 key 因设备型号和固件版本而异
- 测试时可使用已知的通用 key，若不存在则跳过相关检查
