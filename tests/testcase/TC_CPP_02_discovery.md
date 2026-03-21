# TC_CPP_02 — 设备发现与枚举

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-16 | — | 初始版本，包含 6 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 02：设备发现与枚举** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 02-010 | USB 设备枚举 | `ob_query_device_list(ctx)` | `queryDeviceList()` |
| 02-020 | 网络设备发现 | `ob_enable_net_device_enumeration(ctx, enable)` | `enableNetDeviceEnumeration(bool)` |
| 02-030 | 网络设备直连 | `ob_create_net_device(ctx, addr, port)` | `createNetDevice(addr, port)` |
| 02-040 | 网络设备直连（访问模式） | `ob_create_net_device_ex(ctx, addr, port, mode)` | `createNetDevice(addr, port, mode)` |
| 02-050 | 强制 IP 配置 | `ob_force_ip_config(mac, config)` | `forceIp(mac, config)` |
| 02-060 | 设备热插拔回调 | `ob_register_device_changed_callback()` / `ob_unregister_device_changed_callback()` | `registerDeviceChangedCallback()` / `unregisterDeviceChangedCallback()` |
| 02-070 | 时钟同步 | `ob_enable_device_clock_sync(ctx, interval_ms)` | `enableDeviceClockSync(interval)` |

---

## TC_CPP_02_01 — USB 设备枚举

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_01 |
| 名称 | USB 设备枚举 |
| 目的 | 验证 SDK 能正确枚举已连接的 USB 深度相机设备 |
| 覆盖功能点 | 02-010 |
| 前置条件 | 至少一台 Gemini 335/335Le/335Lg 通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.queryDeviceList()` 获取设备列表
3. 读取 `deviceList->deviceCount()`
4. 遍历 `deviceList->getDevice(i)`，i = 0 到 count-1

### 检查项

- ✅ `queryDeviceList()` 返回非空 DeviceList 对象
- ✅ `deviceCount()` > 0
- ✅ 每个 `getDevice(i)` 返回非空 Device 对象
- ✅ C API：`ob_query_device_list(ctx, &error)` 返回非 NULL，error 为 NULL
- ✅ C API：`ob_device_list_get_count(list, &error)` > 0

### 预期结果

成功枚举到至少一台已连接的 USB 设备。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<DeviceList> ob::Context::queryDeviceList()` |
| C++ | `uint32_t ob::DeviceList::deviceCount()` |
| C++ | `std::shared_ptr<Device> ob::DeviceList::getDevice(uint32_t index)` |
| C | `ob_device_list* ob_query_device_list(ob_context* ctx, ob_error** error)` |
| C | `uint32_t ob_device_list_get_count(ob_device_list* list, ob_error** error)` |

### 备注

- 需要物理设备连接，CI 环境需配备自托管 runner
- 若无设备连接，此用例应 skip 而非 fail

---

## TC_CPP_02_02 — 网络设备自动发现开关

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_02 |
| 名称 | 网络设备自动发现开关 |
| 目的 | 验证启用/禁用 GVCP 网络自动发现功能不崩溃 |
| 覆盖功能点 | 02-020 |
| 前置条件 | SDK Context 已创建 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.enableNetDeviceEnumeration(true)` 启用网络发现
3. 调用 `ctx.enableNetDeviceEnumeration(false)` 禁用网络发现
4. 再次调用 `ctx.enableNetDeviceEnumeration(true)` 重新启用
5. 调用 `queryDeviceList()` 验证上下文仍可用
6. 销毁上下文

### 检查项

- ✅ `enableNetDeviceEnumeration(true)` 不崩溃、不抛异常
- ✅ `enableNetDeviceEnumeration(false)` 不崩溃、不抛异常
- ✅ 反复切换（true → false → true）不崩溃
- ✅ 切换后 `queryDeviceList()` 仍可正常调用
- ✅ C API：`ob_enable_net_device_enumeration(ctx, true/false, &error)` 后 error 为 NULL

### 预期结果

网络发现开关可安全切换，不影响 USB 设备枚举功能。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::enableNetDeviceEnumeration(bool enable)` |
| C | `void ob_enable_net_device_enumeration(ob_context* ctx, bool enable, ob_error** error)` |

### 备注

- 启用网络发现后，SDK 会监听 GVCP 协议自动发现局域网内的网络设备（如 Gemini 335Le）
- 无网络设备时启用此功能不应报错

---

## TC_CPP_02_03 — 网络设备直连

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_03 |
| 名称 | 网络设备直连 |
| 目的 | 验证按 IP 地址和端口直接连接网络设备，以及带访问模式的连接 |
| 覆盖功能点 | 02-030, 02-040 |
| 前置条件 | 一台 Gemini 335Le 通过以太网连接且 IP 已知 |
| 硬件依赖 | 335Le |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.createNetDevice(ipAddr, port)` 连接网络设备
3. 验证返回有效 Device 对象
4. 读取设备基本信息（名称、SN）
5. 销毁设备
6. 调用 `ctx.createNetDevice(ipAddr, port, OB_ACCESS_SHARED)` 带访问模式连接
7. 验证返回有效 Device 对象
8. 销毁设备和上下文

### 检查项

- ✅ `createNetDevice(addr, port)` 返回非空 Device 对象
- ✅ 返回的 Device 可查询设备名称（非空）
- ✅ 返回的 Device 可查询序列号（非空）
- ✅ `createNetDevice(addr, port, OB_ACCESS_SHARED)` 返回非空 Device 对象
- ✅ 带访问模式连接的设备同样可查询基本信息
- ✅ C API：`ob_create_net_device(ctx, addr, port, &error)` 返回非 NULL
- ✅ C API：`ob_create_net_device_ex(ctx, addr, port, mode, &error)` 返回非 NULL

### 预期结果

成功通过 IP 直连网络设备，获取有效设备句柄。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `std::shared_ptr<Device> ob::Context::createNetDevice(const char* addr, uint16_t port)` |
| C++ | `std::shared_ptr<Device> ob::Context::createNetDevice(const char* addr, uint16_t port, OBDeviceAccessMode mode)` |
| C | `ob_device* ob_create_net_device(ob_context* ctx, const char* addr, uint16_t port, ob_error** error)` |
| C | `ob_device* ob_create_net_device_ex(ob_context* ctx, const char* addr, uint16_t port, ob_device_access_mode mode, ob_error** error)` |

### 备注

- 需要 335Le 网络设备，CI 需专用 runner
- 默认端口通常为 8090
- 无效 IP 或设备离线时应抛异常而非挂起

---

## TC_CPP_02_04 — 强制 IP 配置

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_04 |
| 名称 | 强制 IP 配置 |
| 目的 | 验证按 MAC 地址为网络设备配置静态 IP 的功能 |
| 覆盖功能点 | 02-050 |
| 前置条件 | 一台 Gemini 335Le 通过以太网连接，MAC 地址已知 |
| 硬件依赖 | 335Le |
| 优先级 | P2 |

### 测试步骤

1. 创建 `Context` 对象
2. 获取目标设备当前 MAC 地址
3. 准备新的 IP 配置（`OBNetIpConfig`）：IP 地址、子网掩码、网关
4. 调用 `ctx.forceIp(macAddr, ipConfig)` 设置静态 IP
5. 等待设备重新上线（约 3-5 秒）
6. 通过新 IP 地址连接设备验证配置生效
7. （清理）恢复原始 IP 配置

### 检查项

- ✅ `forceIp(mac, config)` 调用不崩溃、不抛异常
- ✅ 设置后设备可通过新 IP 地址连接
- ✅ 连接后设备信息（SN）与配置前一致
- ✅ C API：`ob_force_ip_config(mac, config, &error)` 后 error 为 NULL

### 预期结果

成功为网络设备配置静态 IP，设备可通过新 IP 访问。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::forceIp(const char* mac, OBNetIpConfig config)` |
| C | `void ob_force_ip_config(const char* mac, ob_net_ip_config config, ob_error** error)` |

### 备注

- **此操作会修改设备网络配置**，测试后务必恢复原始 IP
- 建议在隔离网络环境中执行，避免 IP 冲突
- MAC 地址格式通常为 `"AA:BB:CC:DD:EE:FF"`

---

## TC_CPP_02_05 — 设备热插拔回调（使用 reboot 模拟）

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |
| 1.1 | 2026-03-17 | 更新使用 reboot 模拟热插拔事件 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_05 |
| 名称 | 设备热插拔回调（使用 reboot 模拟） |
| 目的 | 验证设备连接变化回调的注册、触发和注销功能；使用 reboot 模拟设备断开/重连事件 |
| 覆盖功能点 | 02-060 |
| 前置条件 | 至少一台 Gemini 335/335Le/335Lg 设备已连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象
2. 定义回调函数，记录设备添加/移除事件：
   ```cpp
   std::vector<std::string> events;
   auto callback = [&](const std::shared_ptr<DeviceList>& removedList, 
                       const std::shared_ptr<DeviceList>& addedList) {
       if (removedList && removedList->deviceCount() > 0) {
           events.push_back("removed:" + std::string(removedList->getDevice(0)->getDeviceInfo()->serialNumber()));
       }
       if (addedList && addedList->deviceCount() > 0) {
           events.push_back("added:" + std::string(addedList->getDevice(0)->getDeviceInfo()->serialNumber()));
       }
   };
   ```
3. 调用 `ctx.registerDeviceChangedCallback(callback)` 注册回调
4. 记录当前设备序列号
5. **模拟设备断开**：调用 `device->reboot()` 重启设备
6. 等待设备离线（约 2-3 秒），验证 `removed` 回调触发
7. 等待设备重新上线（约 5-10 秒），验证 `added` 回调触发
8. 验证重连后设备序列号与原设备一致
9. 调用 `ctx.unregisterDeviceChangedCallback()` 注销回调
10. 再次调用 reboot，验证回调不再触发
11. 销毁上下文

### 检查项

- ✅ `registerDeviceChangedCallback()` 注册不崩溃
- ✅ 设备 reboot 后 `removed` 回调被触发
- ✅ `removedList->deviceCount()` > 0，且包含断开设备的 DeviceInfo
- ✅ 设备重新上线后 `added` 回调被触发
- ✅ `addedList->deviceCount()` > 0，且包含新增设备的 DeviceInfo
- ✅ 重连设备的序列号与原设备一致
- ✅ `unregisterDeviceChangedCallback()` 注销不崩溃
- ✅ 注销后设备变化不再触发回调
- ✅ C API：`ob_register_device_changed_callback(ctx, cb, userData, &error)` 后 error 为 NULL
- ✅ C API：`ob_unregister_device_changed_callback(ctx, &error)` 后 error 为 NULL

### 预期结果

使用 reboot 可模拟设备热插拔事件，回调正确触发并返回设备列表；注销后回调不再触发。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::registerDeviceChangedCallback(DeviceChangedCallback callback)` |
| C++ | `void ob::Context::unregisterDeviceChangedCallback()` |
| C++ | `void ob::Device::reboot()` |
| C | `void ob_register_device_changed_callback(ob_context* ctx, ob_device_changed_callback cb, void* user_data, ob_error** error)` |
| C | `void ob_unregister_device_changed_callback(ob_context* ctx, ob_error** error)` |
| C | `void ob_device_reboot(ob_device* dev, ob_error** error)` |

### 备注

- **reboot 是模拟热插拔的推荐方式**，在自动化测试中无需物理拔插设备
- reboot 会触发设备先断开（removed）再连接（added）的完整流程
- 网络设备（335Le）也可用同样的 reboot 方式测试热插拔回调
- 回调在 SDK 内部线程中触发，注意线程安全
- 若需测试物理热插拔，可手动拔插 USB 网线缆

---

## TC_CPP_02_06 — 时钟同步

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_02_06 |
| 名称 | 时钟同步 |
| 目的 | 验证主机-设备时钟同步功能的启用和禁用 |
| 覆盖功能点 | 02-070 |
| 前置条件 | SDK Context 已创建 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.enableDeviceClockSync(1000)` 启用时钟同步，间隔 1000ms
3. 验证调用不崩溃
4. 调用 `ctx.enableDeviceClockSync(0)` 禁用时钟同步
5. 验证调用不崩溃
6. 调用 `ctx.enableDeviceClockSync(500)` 使用不同间隔重新启用
7. 验证调用不崩溃
8. 销毁上下文

### 检查项

- ✅ `enableDeviceClockSync(1000)` 启用不崩溃、不抛异常
- ✅ `enableDeviceClockSync(0)` 禁用不崩溃、不抛异常
- ✅ 反复启用/禁用切换不崩溃
- ✅ 不同间隔值（500, 1000, 5000）均可接受
- ✅ C API：`ob_enable_device_clock_sync(ctx, interval, &error)` 后 error 为 NULL

### 预期结果

时钟同步启用/禁用均安全完成，不影响后续设备操作。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::enableDeviceClockSync(uint64_t repeatIntervalMs)` |
| C | `void ob_enable_device_clock_sync(ob_context* ctx, uint64_t repeat_interval_ms, ob_error** error)` |

### 备注

- 时钟同步主要用于多设备场景下对齐各设备帧时间戳
- 间隔值 0 表示禁用同步
- 启用后 SDK 会周期性向设备发送同步命令
