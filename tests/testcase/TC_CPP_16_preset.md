# TC_CPP_16 — 设备预设（Preset）

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-17 | — | 初始版本，包含 5 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 16：设备预设（Preset）** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 16-010 | 获取当前预设名称 | `ob_device_get_current_preset_name()` | `getCurrentPresetName()` |
| 16-020 | 枚举预设列表 | `ob_device_get_available_preset_list()` | `getAvailablePresetList()` |
| 16-030 | 加载内置预设 | `ob_device_load_preset(dev, name)` | `loadPreset(name)` |
| 16-040 | 从 JSON 文件加载 | `ob_device_load_preset_from_json_file()` | `loadPresetFromJsonFile()` |
| 16-050 | 从 JSON 数据加载 | `ob_device_load_preset_from_json_data()` | `loadPresetFromJsonData()` |
| 16-060 | 导出为 JSON 文件 | `ob_device_export_current_settings_as_preset_json_file()` | `exportCurrentSettingsAsPresetJsonFile()` |
| 16-070 | 导出为 JSON 数据 | `ob_device_export_current_settings_as_preset_json_data()` | `exportCurrentSettingsAsPresetJsonData()` |
| 16-080 | 查询预设是否存在 | `ob_device_preset_list_has_preset()` | `hasPreset()` |

---

## TC_CPP_16_01 — 获取当前 Preset 与枚举列表

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_16_01 |
| 名称 | 获取当前 Preset 与枚举列表 |
| 目的 | 验证可获取当前预设名称和枚举所有可用预设 |
| 覆盖功能点 | 16-010, 16-020 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 获取设备
2. 调用 `getCurrentPresetName()`
3. 调用 `getAvailablePresetList()`
4. 遍历预设列表

### 检查项

- ✅ 当前预设名称非空
- ✅ 预设列表 count > 0
- ✅ 列表包含多个预设选项
- ✅ 当前预设名在列表中

### 预期结果

当前预设和可用预设列表正确获取。

---

## TC_CPP_16_02 — 加载内置 Preset

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_16_02 |
| 名称 | 加载内置 Preset |
| 目的 | 验证可加载内置预设 |
| 覆盖功能点 | 16-030, 16-080 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P0 |

### 测试步骤

1. 获取设备，枚举预设列表
2. 调用 `hasPreset(name)` 验证预设存在
3. 调用 `loadPreset(name)`
4. 验证当前预设名称已变更

### 检查项

- ✅ `hasPreset()` 对存在的预设返回 true
- ✅ `loadPreset()` 调用成功
- ✅ 加载后 `getCurrentPresetName()` 返回新名称

### 预期结果

内置预设可成功加载。

---

## TC_CPP_16_03 — 导出为 JSON 文件/数据

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_16_03 |
| 名称 | 导出为 JSON 文件/数据 |
| 目的 | 验证可将当前设置导出为 JSON |
| 覆盖功能点 | 16-060, 16-070 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `exportCurrentSettingsAsPresetJsonFile(path)`
3. 验证文件创建且非空
4. 调用 `exportCurrentSettingsAsPresetJsonData()`
5. 验证返回的数据非空

### 检查项

- ✅ 文件导出成功
- ✅ JSON 文件非空且格式正确
- ✅ 数据导出成功
- ✅ 返回的 buffer 非空

### 预期结果

当前设置可导出为 JSON 文件或数据。

---

## TC_CPP_16_04 — 从 JSON 文件/数据加载

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_16_04 |
| 名称 | 从 JSON 文件/数据加载 |
| 目的 | 验证可从 JSON 文件或数据加载预设 |
| 覆盖功能点 | 16-040, 16-050 |
| 前置条件 | 至少一台设备通过 USB 连接，有有效的 JSON 预设 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备
2. 调用 `loadPresetFromJsonFile(path)`
3. 或调用 `loadPresetFromJsonData(name, data, size)`
4. 验证预设加载成功

### 检查项

- ✅ JSON 文件加载成功
- ✅ JSON 数据加载成功
- ✅ 加载后设备设置更新

### 预期结果

可从 JSON 文件或数据加载预设。

---

## TC_CPP_16_05 — Preset 切换后属性变化

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-17 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_16_05 |
| 名称 | Preset 切换后属性变化 |
| 目的 | 验证切换预设后设备属性相应变化 |
| 覆盖功能点 | 16-030 |
| 前置条件 | 至少一台设备通过 USB 连接 |
| 硬件依赖 | 是 |
| 优先级 | P1 |

### 测试步骤

1. 获取设备，记录当前 laser/exposure 等属性值
2. 加载不同的预设
3. 再次读取属性值
4. 比较差异

### 检查项

- ✅ 切换预设后属性值发生变化
- ✅ 变化符合预设定义

### 预期结果

切换预设后设备属性相应更新。
