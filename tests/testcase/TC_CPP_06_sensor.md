# TC_CPP_06 — 传感器枚举

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 8 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 06：传感器枚举** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 06-010 | 传感器列表 | `ob_device_get_sensor_list()` | `getSensorList()` |
| 06-020 | 按类型获取传感器 | `ob_device_get_sensor(dev, type)` | `getSensor(type)` |
| 06-030 | 传感器类型 | `ob_sensor_get_type()` | `type()` |
| 06-100 | DEPTH | 深度传感器 | — | `DEPTH_SENSOR` |
| 06-110 | COLOR | 彩色相机 | — | `COLOR_SENSOR` |
| 06-120 | IR | 红外传感器 | — | `IR_SENSOR` |
| 06-130 | IR_LEFT | 左红外（双目立体） | — | `IR_LEFT_SENSOR` |
| 06-140 | IR_RIGHT | 右红外（双目立体） | — | `IR_RIGHT_SENSOR` |
| 06-150 | ACCEL | 加速度计 | — | `ACCEL_SENSOR` |
| 06-160 | GYRO | 陀螺仪 | — | `GYRO_SENSOR` |
| 06-190 | LIDAR | LiDAR | — | `LIDAR_SENSOR` |

---

## TC_CPP_06_01 — 传感器列表完整性

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_01 |
| 名称 | 传感器列表完整性 |
| 目的 | 验证设备传感器列表包含所有预期的传感器类型 |
| 覆盖功能点 | 06-010, 06-030 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 调用 `device->getSensorList()` 获取传感器列表
3. 读取 `sensorList->count()` 获取传感器数量
4. 遍历列表，调用 `sensorList->getSensor(i)` 获取每个传感器
5. 调用 `sensor->type()` 获取传感器类型
6. 统计各类型传感器的存在性

### 检查项

- ✅ `getSensorList()` 返回非空 SensorList 对象
- ✅ `count()` > 0（至少有一个传感器）
- ✅ 对于 Gemini 335：至少包含 Depth + Color 传感器
- ✅ 每个 `getSensor(i)` 返回非空 Sensor 对象
- ✅ 每个 `sensor->type()` 返回有效的 OBSensorType 枚举值
- ✅ 传感器类型不重复（除非设备有多个相同类型的传感器）

### 预期结果

传感器列表包含设备支持的所有传感器，数量和内容符合设备规格。

---

## TC_CPP_06_02 — 按类型获取核心传感器

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_02 |
| 名称 | 按类型获取核心传感器 |
| 目的 | 验证可通过类型直接获取 Depth、Color、IR 核心传感器 |
| 覆盖功能点 | 06-020, 06-100~06-120 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 调用 `device->getSensor(OB_SENSOR_DEPTH)` 获取深度传感器
3. 调用 `device->getSensor(OB_SENSOR_COLOR)` 获取彩色传感器
4. 调用 `device->getSensor(OB_SENSOR_IR)` 获取红外传感器
5. 验证各传感器对象

### 检查项

- ✅ `getSensor(OB_SENSOR_DEPTH)` 返回非空 Sensor 对象
- ✅ `getSensor(OB_SENSOR_COLOR)` 返回非空 Sensor 对象
- ✅ `getSensor(OB_SENSOR_IR)` 返回非空 Sensor 对象（若设备支持）
- ✅ 各传感器的 `type()` 与请求的类型一致
- ✅ C API：`ob_device_get_sensor(dev, OB_SENSOR_DEPTH)` 返回非 NULL

### 预期结果

可通过类型直接获取各核心传感器，返回的传感器对象有效。

---

## TC_CPP_06_03 — IMU 传感器获取

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_03 |
| 名称 | IMU 传感器获取 |
| 目的 | 验证可获取加速度计和陀螺仪传感器（Gemini 335 含 IMU） |
| 覆盖功能点 | 06-020, 06-150, 06-160 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 调用 `device->getSensor(OB_SENSOR_ACCEL)` 获取加速度计
3. 调用 `device->getSensor(OB_SENSOR_GYRO)` 获取陀螺仪
4. 验证传感器对象和类型

### 检查项

- ✅ `getSensor(OB_SENSOR_ACCEL)` 返回非空 Sensor 对象
- ✅ `getSensor(OB_SENSOR_GYRO)` 返回非空 Sensor 对象
- ✅ 加速度计 `type()` 返回 `OB_SENSOR_ACCEL`
- ✅ 陀螺仪 `type()` 返回 `OB_SENSOR_GYRO`
- ✅ C API：`ob_device_get_sensor(dev, OB_SENSOR_ACCEL)` 返回非 NULL

### 预期结果

IMU 传感器可正常获取，Gemini 335 支持加速度计和陀螺仪。

---

## TC_CPP_06_04 — 双目 IR 传感器

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_04 |
| 名称 | 双目 IR 传感器 |
| 目的 | 验证可获取左右红外传感器（若设备支持双目 IR） |
| 覆盖功能点 | 06-020, 06-130, 06-140 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 尝试调用 `device->getSensor(OB_SENSOR_IR_LEFT)` 获取左红外
3. 尝试调用 `device->getSensor(OB_SENSOR_IR_RIGHT)` 获取右红外
4. 若存在，验证传感器类型

### 检查项

- ✅ 若返回非空，传感器类型为 `OB_SENSOR_IR_LEFT` / `OB_SENSOR_IR_RIGHT`
- ✅ 若设备不支持双目 IR，返回 null 或抛异常（不崩溃）
- ✅ Gemini 335 系列通常有独立的左/右 IR 传感器
- ✅ C API 行为一致

### 预期结果

若设备支持双目 IR，可正常获取左右传感器；不支持时安全返回。

---

## TC_CPP_06_05 — 不存在传感器类型安全

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_05 |
| 名称 | 不存在传感器类型安全 |
| 目的 | 验证对 Gemini 335 不支持的传感器类型（如 LIDAR）请求时 SDK 安全处理 |
| 覆盖功能点 | 06-020, 06-190 |
| 前置条件 | 至少一台 Gemini 335 设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 调用 `device->getSensor(OB_SENSOR_LIDAR)` 获取 LiDAR 传感器
3. 观察返回结果或异常
4. 尝试获取其他 Gemini 335 不支持的传感器类型

### 检查项

- ✅ `getSensor(OB_SENSOR_LIDAR)` 返回 null 或抛出可捕获异常
- ✅ **不崩溃**、不导致未定义行为
- ✅ 异常消息包含有意义描述
- ✅ 后续对其他有效传感器的操作不受影响
- ✅ C API：`ob_device_get_sensor(dev, OB_SENSOR_LIDAR)` 返回 NULL

### 预期结果

对不支持的传感器类型请求安全处理，返回 null 或抛异常，不崩溃。

---

## TC_CPP_06_06 — Sensor 类型一致性

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_06 |
| 名称 | Sensor 类型一致性 |
| 目的 | 验证通过 getSensor 获取的传感器其 type() 与请求类型一致 |
| 覆盖功能点 | 06-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 调用 `device->getSensor(OB_SENSOR_DEPTH)` 获取深度传感器
3. 调用 `sensor->type()` 验证类型
4. 对其他可用传感器类型重复测试

### 检查项

- ✅ `getSensor(OB_SENSOR_DEPTH)->type()` 返回 `OB_SENSOR_DEPTH`
- ✅ `getSensor(OB_SENSOR_COLOR)->type()` 返回 `OB_SENSOR_COLOR`
- ✅ `getSensor(OB_SENSOR_ACCEL)->type()` 返回 `OB_SENSOR_ACCEL`
- ✅ `getSensor(OB_SENSOR_GYRO)->type()` 返回 `OB_SENSOR_GYRO`

### 预期结果

通过类型获取的传感器，其类型标识与请求类型一致。

---

## TC_CPP_06_07 — Sensor 级别 callback 开流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_07 |
| 名称 | Sensor 级别 callback 开流 |
| 目的 | 验证可通过 Sensor 级别的回调方式开流取帧 |
| 覆盖功能点 | 06-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取深度传感器 `device->getSensor(OB_SENSOR_DEPTH)`
3. 获取传感器支持的 StreamProfile 列表
4. 选择默认的 Profile
5. 注册帧回调函数 `sensor->start(profile, callback)`
6. 运行 3 秒，统计收到的帧数
7. 调用 `sensor->stop()` 停止

### 检查项

- ✅ `start(profile, callback)` 调用成功
- ✅ 回调函数被触发，收到 Frame 对象
- ✅ 收到的帧格式与 Profile 配置一致
- ✅ 3 秒内收到约 90 帧（30fps）
- ✅ `stop()` 调用后回调不再触发
- ✅ C API：`ob_sensor_start(sensor, profile, callback, user_data, &error)` 成功

### 预期结果

Sensor 级别的回调开流正常工作，可实时接收帧数据。

---

## TC_CPP_06_08 — Sensor 级别重复开关流

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_08 |
| 名称 | Sensor 级别重复开关流 |
| 目的 | 验证 Sensor 级别 start/stop 循环多次均正常工作 |
| 覆盖功能点 | 06-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 获取深度传感器
3. 循环 5 次：
   - 选择 Profile 并 `start(profile, callback)`
   - 等待 1 秒，确认收到帧
   - `stop()` 停止
   - 等待 100ms
4. 检查每次循环的帧接收情况

### 检查项

- ✅ 5 次 `start()` 均调用成功
- ✅ 每次运行期间都收到有效帧
- ✅ 5 次 `stop()` 均调用成功，无崩溃
- ✅ 无资源泄漏或异常

### 预期结果

Sensor 级别开关流可重复执行，每次都能正常收帧。

---

## TC_CPP_06_09 — 设备重启后传感器恢复（使用 reboot 模拟）

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_06_09 |
| 名称 | 设备重启后传感器恢复（使用 reboot 模拟） |
| 目的 | 验证设备重启后传感器可重新枚举和正常工作 |
| 覆盖功能点 | 06-010, 06-020 |
| 前置条件 | 至少一台 Gemini 335 设备已连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象并获取设备
2. 记录当前传感器列表（数量和类型）
3. 获取深度传感器并测试开流取帧（验证传感器工作正常）
4. 调用 `device->reboot()` 重启设备
5. 等待设备重新上线（约 5-10 秒）
6. 重新获取设备（通过热插拔回调或重新枚举）
7. 再次获取传感器列表
8. 对比重启前后的传感器列表
9. 重新获取深度传感器并测试开流取帧

### 检查项

- ✅ 重启前传感器列表完整，各传感器可正常工作
- ✅ `reboot()` 调用成功，设备正常重启
- ✅ 设备重新上线后可重新获取
- ✅ 重启后传感器数量与重启前一致
- ✅ 重启后传感器类型与重启前一致
- ✅ 重启后传感器可正常开流取帧
- ✅ 无内存泄漏或资源未释放

### 预期结果

设备重启后传感器可完全恢复，枚举和开流功能正常。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Device::reboot()` |
| C | `void ob_device_reboot(ob_device* dev, ob_error** error)` |

### 备注

- 使用 reboot 模拟设备断开/重连，验证传感器在设备恢复后的状态
- 此测试与 TC_CPP_02_05（热插拔回调）可配合使用
- 网络设备（335Le）同样适用此测试方法
