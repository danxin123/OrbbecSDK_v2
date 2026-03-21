# TC_CPP_25 — 关键数据结构

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 7 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 25：关键数据结构** 的全部功能点：

| 编号 | 结构体 | 描述 |
|------|--------|------|
| 25-010 | `OBCameraIntrinsic` | 相机内参（fx, fy, cx, cy, width, height） |
| 25-020 | `OBCameraDistortion` | 畸变系数（k1-k6, p1, p2 Brown 模型） |
| 25-030 | `OBExtrinsic` | 外参（3×3 旋转 + 3×1 平移） |
| 25-040 | `OBCameraParam` | 深度 + 色彩相机内参外参 |
| 25-050 | `OBCalibrationParam` | 完整多传感器标定参数 |
| 25-060 | `OBAccelIntrinsic` / `OBGyroIntrinsic` | IMU 内参 |
| 25-070 | `OBDeviceTemperature` | 设备温度（CPU/IR/激光/TEC） |
| 25-080 | `OBDepthWorkMode` | 深度工作模式（名称 + 校验和） |
| 25-090 | `OBMultiDeviceSyncConfig` | 多设备同步配置 |
| 25-100 | `OBNetIpConfig` | 网络 IP 配置（地址/掩码/网关） |
| 25-110 | `OBHdrConfig` | HDR 序列配置 |
| 25-120 | `OBRegionOfInterest` | AE ROI 矩形 |
| 25-130 | `OBPoint3f` / `OBColorPoint` | 3D 点 / XYZRGB 点 |
| 25-140 | `OBAccelValue` / `OBGyroValue` | IMU 三轴浮点向量 |
| 25-150 | `OBIntPropertyRange` / `OBFloatPropertyRange` | 属性值范围（min/max/step/def/cur） |
| 25-160 | `OBFilterConfigSchemaItem` | Filter 配置项描述 |

---

## TC_CPP_25_01 — 相机内参/畸变/外参结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_01 |
| 名称 | 相机内参/畸变/外参结构体 |
| 目的 | 验证相机标定数据结构体字段完整有效 |
| 覆盖功能点 | 25-010~25-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 获取设备相机参数
2. 读取内参、畸变、外参结构体
3. 验证各字段

### 检查项

- ✅ OBCameraIntrinsic: fx > 0, fy > 0, cx > 0, cy > 0
- ✅ OBCameraDistortion: 各系数可读
- ✅ OBExtrinsic: 旋转矩阵正交

### 预期结果

相机标定数据结构体字段完整有效。

---

## TC_CPP_25_02 — CameraParam/CalibrationParam

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_02 |
| 名称 | CameraParam/CalibrationParam |
| 目的 | 验证完整相机参数结构体 |
| 覆盖功能点 | 25-040, 25-050 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 获取 OBCameraParam
2. 获取 OBCalibrationParam
3. 验证内容

### 检查项

- ✅ OBCameraParam 含 depth+color 完整内参外参
- ✅ OBCalibrationParam 含多传感器标定数据

### 预期结果

相机参数结构体数据完整。

---

## TC_CPP_25_03 — IMU 内参结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_03 |
| 名称 | IMU 内参结构体 |
| 目的 | 验证 IMU 内参结构体 |
| 覆盖功能点 | 25-060 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取 OBAccelIntrinsic 和 OBGyroIntrinsic
2. 验证各字段

### 检查项

- ✅ 加速度内参：偏移、比例因子可读
- ✅ 陀螺仪内参：偏移、比例因子可读

### 预期结果

IMU 内参结构体数据有效。

---

## TC_CPP_25_04 — 设备温度结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_04 |
| 名称 | 设备温度结构体 |
| 目的 | 验证设备温度结构体 |
| 覆盖功能点 | 25-070 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 获取 OBDeviceTemperature
2. 验证各温度值

### 检查项

- ✅ CPU/IR/Laser/TEC 温度在 0~80°C 范围

### 预期结果

设备温度数据在合理范围。

---

## TC_CPP_25_05 — 配置与模式结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_05 |
| 名称 | 配置与模式结构体 |
| 目的 | 验证深度工作模式、同步配置、IP 配置结构体 |
| 覆盖功能点 | 25-080~25-100 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 验证 OBDepthWorkMode 结构
2. 验证 OBMultiDeviceSyncConfig 字段可读写
3. 验证 OBNetIpConfig IP 地址合法

### 检查项

- ✅ OBDepthWorkMode name 非空，checksum 有效
- ✅ OBMultiDeviceSyncConfig 字段可读写
- ✅ OBNetIpConfig IP 地址合法

### 预期结果

配置与模式结构体字段正确。

---

## TC_CPP_25_06 — HDR/ROI/点云/IMU 结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_06 |
| 名称 | HDR/ROI/点云/IMU 结构体 |
| 目的 | 验证各种功能相关的数据结构体 |
| 覆盖功能点 | 25-110~25-140 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 验证 OBHdrConfig 结构
2. 验证 OBRegionOfInterest 矩形合理
3. 验证 OBPoint3f/OBColorPoint 坐标
4. 验证 OBAccelValue/OBGyroValue 三轴

### 检查项

- ✅ OBHdrConfig 序列配置有效
- ✅ OBRegionOfInterest 矩形合理
- ✅ 点云坐标可读
- ✅ IMU 三轴值有效

### 预期结果

各类功能结构体数据有效。

---

## TC_CPP_25_07 — 属性范围与配置描述结构体

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_25_07 |
| 名称 | 属性范围与配置描述结构体 |
| 目的 | 验证属性范围和 Filter 配置描述结构体 |
| 覆盖功能点 | 25-150, 25-160 |
| 前置条件 | SDK 可用 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 验证 OBIntPropertyRange/OBFloatPropertyRange 字段
2. 验证 OBFilterConfigSchemaItem 字段

### 检查项

- ✅ PropertyRange: min/max/step/def/cur 完整
- ✅ FilterConfigSchemaItem: name/type/min/max/step/def 有效

### 预期结果

属性范围和配置描述结构体字段完整有效。
