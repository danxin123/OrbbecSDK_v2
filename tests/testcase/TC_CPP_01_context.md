# TC_CPP_01 — SDK 初始化与全局管理

## 文档版本记录

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|---------|
| 1.0 | 2026-03-16 | — | 初始版本，包含 6 个测试用例 |

---

## 关联功能点

本模块覆盖 [sdk_feature_matrix.md](../sdk_feature_matrix.md) 中 **模块 01：SDK 初始化与全局管理** 的全部功能点：

| 编号 | 功能点 | C API | C++ API |
|------|--------|-------|---------|
| 01-010 | 默认初始化 | `ob_create_context()` | `Context()` |
| 01-020 | 配置文件初始化 | `ob_create_context_with_config(path)` | `Context(configPath)` |
| 01-030 | 上下文销毁 | `ob_delete_context(ctx)` | 析构函数 |
| 01-040 | 空闲内存释放 | `ob_free_idle_memory(ctx)` | `freeIdleMemory()` |
| 01-050 | UVC 后端选择 | `ob_set_uvc_backend_type(ctx, type)` | `setUvcBackendType(type)` |
| 01-060 | 扩展插件目录 | `ob_set_extensions_directory(dir)` | — |

---

## TC_CPP_01_01 — Context 默认构造与析构

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_01 |
| 名称 | Context 默认构造与析构 |
| 目的 | 验证 SDK 上下文能以默认方式创建并正确销毁，无崩溃或资源泄漏 |
| 覆盖功能点 | 01-010, 01-030 |
| 前置条件 | SDK 库已正确安装/链接 |
| 硬件依赖 | 否 |
| 优先级 | P0 |

### 测试步骤

1. 调用 `Context()` 默认构造函数创建上下文对象
2. 调用 `ctx.queryDeviceList()` 获取设备列表
3. 销毁 Context 对象（C++：离开作用域自动析构；C：调用 `ob_delete_context()`）

### 检查项

- ✅ `Context()` 构造不抛异常，返回有效对象
- ✅ `queryDeviceList()` 可调用且不抛异常（设备列表可为空）
- ✅ 析构过程不崩溃、不抛异常
- ✅ C API：`ob_create_context(&error)` 返回非 NULL，error 为 NULL
- ✅ C API：`ob_delete_context(ctx, &error)` 后 error 为 NULL

### 预期结果

Context 对象正常创建，可执行基本查询操作，析构过程平稳无异常。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `ob::Context::Context()` |
| C++ | `ob::Context::~Context()` |
| C++ | `std::shared_ptr<DeviceList> ob::Context::queryDeviceList()` |
| C | `ob_context* ob_create_context(ob_error** error)` |
| C | `void ob_delete_context(ob_context* ctx, ob_error** error)` |
| C | `ob_device_list* ob_query_device_list(ob_context* ctx, ob_error** error)` |

### 备注

- 无平台差异，所有支持平台行为一致
- 该用例为 P0 门禁用例，发版前必须通过

---

## TC_CPP_01_02 — Context 配置文件构造

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_02 |
| 名称 | Context 配置文件构造 |
| 目的 | 验证使用有效配置文件可正常创建上下文，使用无效路径可正确处理错误 |
| 覆盖功能点 | 01-020 |
| 前置条件 | 准备有效的 SDK 配置文件（OrbbecSDKConfig.xml）和一个不存在的路径 |
| 硬件依赖 | 否 |
| 优先级 | P0 |

### 测试步骤

1. 准备一个有效的 SDK 配置文件路径 `validPath`
2. 调用 `Context(validPath)` 创建上下文
3. 验证创建成功并可执行基本操作
4. 销毁上下文
5. 调用 `Context("nonexistent.xml")` 使用不存在的路径创建上下文
6. 捕获异常或观察降级行为

### 检查项

- ✅ `Context(validPath)` 构造不抛异常，创建成功
- ✅ 使用有效配置文件创建的上下文可以正常调用 `queryDeviceList()`
- ✅ `Context("nonexistent.xml")` 抛出异常或以默认配置降级运行（取决于 SDK 实现）
- ✅ 析构有效配置上下文不崩溃
- ✅ C API：`ob_create_context_with_config(validPath, &error)` 返回非 NULL
- ✅ C API：`ob_create_context_with_config("nonexistent.xml", &error)` 的 error 非 NULL 或返回有效降级上下文

### 预期结果

有效路径正常创建上下文；无效路径抛出异常或降级到默认行为，不崩溃。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `ob::Context::Context(const char* configPath)` |
| C | `ob_context* ob_create_context_with_config(const char* config_path, ob_error** error)` |

### 备注

- 配置文件通常为 `OrbbecSDKConfig.xml`，包含日志级别、USB 后端设置等
- 无效路径的行为取决于 SDK 版本：早期版本可能崩溃，目标版本应抛异常或降级

---

## TC_CPP_01_03 — Context 重复创建销毁

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_03 |
| 名称 | Context 重复创建销毁 |
| 目的 | 验证 Context 在多次创建/销毁循环中不会出现资源泄漏或崩溃 |
| 覆盖功能点 | 01-010, 01-030 |
| 前置条件 | SDK 库已正确安装/链接 |
| 硬件依赖 | 否 |
| 优先级 | P0 |

### 测试步骤

1. 循环 10 次：
   1. 调用 `Context()` 创建上下文
   2. 调用 `queryDeviceList()` 执行基本操作
   3. 销毁上下文（C++：离开作用域；C：调用 `ob_delete_context()`）
2. 记录循环开始和结束时的进程内存占用（必须）

### 检查项

- ✅ 10 次循环全部完成，无崩溃
- ✅ 每次 `Context()` 构造均成功
- ✅ 每次析构均正常完成
- ✅ 无异常抛出
- ✅ （必须）10 次循环后进程内存增量 < 1MB

### 预期结果

10 次创建/销毁循环全部顺利完成，无资源泄漏、无崩溃，且进程内存增量小于 1MB。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `ob::Context::Context()` |
| C++ | `ob::Context::~Context()` |
| C | `ob_context* ob_create_context(ob_error** error)` |
| C | `void ob_delete_context(ob_context* ctx, ob_error** error)` |

### 备注

- 此用例用于检测 SDK 内部资源管理的健壮性
- 内存检查为必检项，推荐在 nightly 测试中配合 Valgrind/ASan 进一步定位潜在泄漏

---

## TC_CPP_01_04 — 空闲内存释放

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_04 |
| 名称 | 空闲内存释放 |
| 目的 | 验证 `freeIdleMemory()` 调用安全，多次调用幂等 |
| 覆盖功能点 | 01-040 |
| 前置条件 | SDK Context 已创建 |
| 硬件依赖 | 否 |
| 优先级 | P1 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.freeIdleMemory()` 第一次
3. 调用 `ctx.freeIdleMemory()` 第二次（幂等验证）
4. 调用 `ctx.freeIdleMemory()` 第三次
5. 调用 `queryDeviceList()` 验证上下文仍然可用
6. 销毁上下文

### 检查项

- ✅ 第一次 `freeIdleMemory()` 调用不崩溃、不抛异常
- ✅ 连续多次调用 `freeIdleMemory()` 均不崩溃（幂等性）
- ✅ 调用后上下文仍可正常使用（`queryDeviceList()` 不报错）
- ✅ C API：`ob_free_idle_memory(ctx, &error)` 后 error 为 NULL

### 预期结果

`freeIdleMemory()` 可安全调用任意次数，不影响 Context 后续功能。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::freeIdleMemory()` |
| C | `void ob_free_idle_memory(ob_context* ctx, ob_error** error)` |

### 备注

- 此函数主要用于在长时间运行场景中主动释放帧内存池的空闲缓冲区
- 在无开流状态下调用效果有限，但不应出错

---

## TC_CPP_01_05 — UVC 后端选择

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_05 |
| 名称 | UVC 后端选择 |
| 目的 | 验证 UVC 后端类型设置接口在各平台上不崩溃 |
| 覆盖功能点 | 01-050 |
| 前置条件 | SDK Context 已创建 |
| 硬件依赖 | 否 |
| 优先级 | P2 |

### 测试步骤

1. 创建 `Context` 对象
2. 调用 `ctx.setUvcBackendType(OB_UVC_BACKEND_AUTO)`
3. 调用 `ctx.setUvcBackendType(OB_UVC_BACKEND_LIBUVC)`
4. 调用 `ctx.setUvcBackendType(OB_UVC_BACKEND_V4L2)`
5. 销毁上下文

### 检查项

- ✅ `setUvcBackendType(OB_UVC_BACKEND_AUTO)` 不崩溃、不抛异常
- ✅ `setUvcBackendType(OB_UVC_BACKEND_LIBUVC)` 不崩溃（Linux 有效，Windows 可能忽略或报警告）
- ✅ `setUvcBackendType(OB_UVC_BACKEND_V4L2)` 不崩溃（Linux 有效，Windows 忽略或报警告）
- ✅ C API：`ob_set_uvc_backend_type(ctx, type, &error)` 各类型调用后 error 为 NULL 或为非致命错误

### 预期结果

各后端类型设置调用均安全完成，不崩溃。Linux 下 V4L2 和 LIBUVC 切换有效；Windows/macOS 下非原生后端可能被忽略或降级到默认。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C++ | `void ob::Context::setUvcBackendType(OBUvcBackendType type)` |
| C | `void ob_set_uvc_backend_type(ob_context* ctx, ob_uvc_backend_type type, ob_error** error)` |

### 备注

- **平台差异**：V4L2 仅 Linux 有效；MSMF 仅 Windows 有效；LIBUVC 为跨平台后备
- 在 Windows/macOS 上设置 V4L2 不应崩溃，SDK 应忽略或降级
- AUTO 模式会自动选择当前平台最优后端

---

## TC_CPP_01_06 — 扩展插件目录设置

### 用例版本记录

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0 | 2026-03-16 | 初始版本 |

### 基本信息

| 字段 | 值 |
|------|-----|
| 编号 | TC_CPP_01_06 |
| 名称 | 扩展插件目录设置 |
| 目的 | 验证设置 Filter 插件加载路径接口对有效/无效路径均不崩溃 |
| 覆盖功能点 | 01-060 |
| 前置条件 | 准备一个有效的目录路径和一个不存在的路径 |
| 硬件依赖 | 否 |
| 优先级 | P2 |

### 测试步骤

1. 调用 `ob_set_extensions_directory(validDir)` 使用有效目录
2. 验证调用不崩溃
3. 调用 `ob_set_extensions_directory("/nonexistent/path")` 使用无效目录
4. 验证调用不崩溃
5. 调用 `ob_set_extensions_directory("")` 使用空字符串
6. 验证调用不崩溃
7. 调用 `ob_set_extensions_directory(NULL)` 使用 NULL 指针
8. 验证调用不崩溃或抛出可捕获异常

### 检查项

- ✅ 有效目录路径设置不崩溃、不抛异常
- ✅ 不存在的路径设置不崩溃（SDK 可记录警告日志）
- ✅ 空字符串不崩溃
- ✅ NULL 指针不崩溃（应抛异常或忽略）
- ✅ 设置后 SDK 基本功能（如 Context 创建、Filter 创建）不受影响

### 预期结果

各种路径输入均不导致崩溃，SDK 对无效路径进行安全处理。

### 关联 API

| 语言 | API 签名 |
|------|----------|
| C | `void ob_set_extensions_directory(const char* directory)` |

### 备注

- 此函数为全局 C API，无对应 C++ 接口
- 仅影响后续的私有 Filter 插件加载行为
- 通常在 Context 创建之前调用
