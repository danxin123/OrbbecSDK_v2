# C++ SDK 测试用例清单

本文档列出 C++ SDK 全覆盖测试用例，基于 [SDK 功能矩阵](../../sdk_feature_matrix.md) 的功能点编号。

**编号规则**：`TC_CPP_MM_NN`，MM 对应功能矩阵模块号（01~25），NN 为模块内测试序号。  
**覆盖功能点**：直接引用 `sdk_feature_matrix.md` 中的编号，便于追溯。

---

## 模块列表

| 模块 | 模块名称 | 用例数 | 覆盖功能点数 |
|------|---------|--------|------------|
| 01 | SDK 初始化与全局管理 | 6 | 6 |
| 02 | 设备发现与枚举 | 6 | 7 |
| 03 | 设备列表查询 | 6 | 9 |
| 04 | 设备信息 | 5 | 11 |
| 05 | 设备访问模式 | 4 | 4 |
| 06 | 传感器枚举 | 9 | 16 |
| 07 | 流配置 | 5 | 6 |
| 08 | 流水线 | 11 | 15 |
| 09 | 流水线配置 | 9 | 20 |
| 10 | 帧数据访问 | 14 | 30 |
| 11 | 帧元数据 | 6 | 14 |
| 12 | 帧创建 | 4 | 7 |
| 13 | 滤波器 | 17 | 37 |
| 14 | 设备属性控制 | 18 | 100+ |
| 15 | 深度工作模式 | 5 | 5 |
| 16 | 设备预设 | 5 | 8 |
| 17 | 帧交错 | 3 | 4 |
| 18 | 录制与回放 | 8 | 13 |
| 19 | 多设备同步 | 7 | 23 |
| 20 | 坐标变换 | 6 | 6 |
| 21 | 固件管理 | 9 | 11 |
| 22 | 版本查询 | 3 | 5 |
| 23 | 日志系统 | 5 | 12 |
| 24 | 错误处理 | 7 | 13 |
| 25 | 关键数据结构 | 7 | 16 |
| **合计** | | **183** | **380+** |

---

## 模块 01：SDK 初始化与全局管理

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_01_01 | Context 默认构造与析构 | 01-010, 01-030 | 否 | `Context()` 创建成功，`queryDeviceList()` 可调用，析构无崩溃 |
| TC_CPP_01_02 | Context 配置文件构造 | 01-020 | 否 | `Context(validPath)` 成功；`Context("nonexistent.xml")` 应抛异常或降级 |
| TC_CPP_01_03 | Context 重复创建销毁 | 01-010, 01-030 | 否 | 循环创建销毁 10 次，无资源泄漏、无崩溃，内存增量 < 1MB（必检） |
| TC_CPP_01_04 | 空闲内存释放 | 01-040 | 否 | `freeIdleMemory()` 调用不崩溃，多次调用幂等 |
| TC_CPP_01_05 | UVC 后端选择 | 01-050 | 否 | `setUvcBackendType(OB_UVC_BACKEND_AUTO/LIBUVC/V4L2)` 不崩溃（Linux 有效） |
| TC_CPP_01_06 | 扩展插件目录设置 | 01-060 | 否 | `ob_set_extensions_directory(dir)` 有效/无效路径均不崩溃 |

---

## 模块 02：设备发现与枚举

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_02_01 | USB 设备枚举 | 02-010 | 是 | `queryDeviceList()` 返回非空列表，`count() > 0` |
| TC_CPP_02_02 | 网络设备自动发现开关 | 02-020 | 否 | `enableNetDeviceEnumeration(true/false)` 切换无崩溃 |
| TC_CPP_02_03 | 网络设备直连 | 02-030, 02-040 | 335Le | `createNetDevice(addr, port)` 连接成功；`createNetDevice(addr, port, accessMode)` 带访问模式连接 |
| TC_CPP_02_04 | 强制 IP 配置 | 02-050 | 335Le | `forceIp(mac, ipConfig)` 设置静态 IP |
| TC_CPP_02_05 | 设备热插拔回调 | 02-060 | 是 | `registerDeviceChangedCallback(cb)` 注册 → `unregisterDeviceChangedCallback()` 注销，回调触发正常 |
| TC_CPP_02_06 | 时钟同步 | 02-070 | 否 | `enableDeviceClockSync(1000)` 启用，`enableDeviceClockSync(0)` 禁用 |

---

## 模块 03：设备列表查询

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_03_01 | 设备数量与索引访问 | 03-010, 03-020 | 是 | `deviceCount() > 0`，遍历 `getDevice(i)` 均非空 |
| TC_CPP_03_02 | 带访问模式获取设备 | 03-030 | 是 | `getDeviceEx(idx, EXCLUSIVE_ACCESS/SHARED_ACCESS)` 正常获取 |
| TC_CPP_03_03 | 按序列号/UID 查找 | 03-040, 03-050 | 是 | 获取首设备 SN/UID → `getDeviceBySN(sn)` / `getDeviceByUid(uid)` 返回一致设备 |
| TC_CPP_03_04 | 设备基本信息字段 | 03-060, 03-070, 03-080 | 是 | `getDeviceName()` 非空，`getDevicePid() > 0`，`getDeviceVid() == 0x2bc5`，`getDeviceConnectionType()` 为 "USB"/"Ethernet" |
| TC_CPP_03_05 | 网络设备信息字段 | 03-090 | 335Le | `getDeviceIpAddress()`, `getDeviceSubnetMask()`, `getDeviceGateway()`, `getDeviceLocalMac()`, `getDeviceLocalIp()` 均非空且格式合法 |
| TC_CPP_03_06 | 越界索引安全 | 03-020 | 是 | `getDevice(count)` 应抛异常或返回 null，不崩溃 |

---

## 模块 04：设备信息

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_04_01 | 基本信息字段 | 04-010, 04-020, 04-030, 04-040 | 是 | `name()`, `serialNumber()`, `firmwareVersion()`, `hardwareVersion()` 非空，固件版本匹配 x.y.z 格式 |
| TC_CPP_04_02 | 标识字段 | 04-050, 04-060 | 是 | `pid() > 0`, `vid() > 0`, `uid()` 非空，`connectionType()` 为 "USB"/"Ethernet" |
| TC_CPP_04_03 | 网络设备 IP 信息 | 04-070 | 335Le | `ipAddress()`, `subnetMask()`, `gateway()` 非空且格式合法 |
| TC_CPP_04_04 | 芯片与类型信息 | 04-080, 04-090, 04-100 | 是 | `asicName()` 不抛异常，`deviceType()` 返回有效枚举，`supportedMinSdkVersion()` 不抛异常 |
| TC_CPP_04_05 | 扩展信息查询 | 04-110 | 是 | `isExtensionInfoExist(key)` 对已知/未知 key 返回合理值；已知 key 的 `getExtensionInfo()` 可读 |

---

## 模块 05：设备访问模式

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_05_01 | 独占模式获取设备 | 05-010 | 是 | `getDeviceEx(idx, EXCLUSIVE_ACCESS)` → `getDeviceState() == EXCLUSIVE_ACCESS` |
| TC_CPP_05_02 | 共享模式获取设备 | 05-020 | 是 | `getDeviceEx(idx, SHARED_ACCESS)` → `getDeviceState() == SHARED_ACCESS` |
| TC_CPP_05_03 | 访问模式冲突处理 | 05-010, 05-020 | 2台 | 设备 A 以独占模式打开后，再尝试打开应抛异常或返回 BUSY |
| TC_CPP_05_04 | 状态查询 | 05-030 | 是 | 获取设备前后检查 `getDeviceState()` 返回值变化 |

---

## 模块 06：传感器枚举

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_06_01 | 传感器列表非空 | 06-010 | 是 | `getSensorList()` → `count() >= 2`（Depth + Color） |
| TC_CPP_06_02 | 按类型获取传感器 | 06-020 | 是 | `getSensor(OB_SENSOR_DEPTH)` / `getSensor(OB_SENSOR_COLOR)` 非空 |
| TC_CPP_06_03 | 传感器类型完整性 | 06-010 | 是 | 验证设备支持的所有传感器类型均存在于列表中 |
| TC_CPP_06_04 | 无效传感器类型处理 | 06-020 | 是 | `getSensor((OBSensorType)999)` 应抛异常或返回 null |
| TC_CPP_06_05 | IR 左右传感器查询 | 06-030, 06-040 | 是 | 对支持双目 IR 的设备，验证 `getSensor(OB_SENSOR_IR_LEFT)` / `getSensor(OB_SENSOR_IR_RIGHT)` |
| TC_CPP_06_06 | 陀螺仪与加速度计传感器 | 06-050, 06-060 | 是 | IMU 设备：`getSensor(OB_SENSOR_ACCEL)` / `getSensor(OB_SENSOR_GYRO)` 非空 |
| TC_CPP_06_07 | 传感器名称与数量 | 06-070 | 是 | `getSensorName()` 返回可读字符串，`getSensorCount()` >= 2 |
| TC_CPP_06_08 | 迭代器遍历传感器 | 06-070 | 是 | 使用迭代器遍历所有传感器，确认无重复、无遗漏 |
| TC_CPP_06_09 | 传感器枚举设备重启恢复 | 06-010 | 是 | 设备重启后 `getSensorList()` 能正确枚举所有传感器 |

---

## 模块 07：流配置

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_07_01 | 流配置列表非空 | 07-010, 07-020 | 是 | `getStreamProfileList(sensor)` → `count() > 0` |
| TC_CPP_07_02 | VideoStreamProfile 属性 | 07-030 | 是 | 每个 profile：`getWidth()`, `getHeight()`, `getFps()`, `getFormat()` 均有效 |
| TC_CPP_07_03 | 按参数获取 Profile | 07-040 | 是 | `getVideoStreamProfile(640, 480, YUYV, 30)` 返回匹配项或 null |
| TC_CPP_07_04 | 无效参数处理 | 07-040 | 是 | 查询不存在分辨率（如 99999x99999）应返回 null 或抛异常 |
| TC_CPP_07_05 | 多种格式支持验证 | 07-030 | 是 | Depth 支持 Y16/Y14/Y11；Color 支持 YUYV/MJPG/RGB/RGBA/BGR/BGRA/Y16 |

---

## 模块 08：流水线

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_08_01 | Pipeline 默认构造 | 08-010, 08-020 | 是 | `Pipeline(device)` 构造成功，`getDevice()` 返回同一设备 |
| TC_CPP_08_02 | Pipeline 获取流配置 | 08-030 | 是 | `getStreamProfileList(DEPTH)` / `getStreamProfileList(COLOR)` 非空 |
| TC_CPP_08_03 | 启动并等待帧集 | 08-040, 08-050 | 是 | `start(config)` → `waitForFrames(timeout)` 返回有效 FrameSet |
| TC_CPP_08_04 | 停止后释放资源 | 08-060 | 是 | `stop()` 后重复停止不崩溃，Pipeline 析构无泄漏 |
| TC_CPP_08_05 | 帧回调模式 | 08-050 | 是 | `start(config, callback)` → 回调持续收到帧 → `stop()` 回调停止 |
| TC_CPP_08_06 | 动态切换配置 | 08-070 | 是 | 运行中 `stop()` → 修改 Config → `start()` 新配置生效 |
| TC_CPP_08_07 | 获取相机参数 | 08-080 | 是 | `getCameraParam()` 返回有效内参外参（fx/fy/cx/cy 非零） |
| TC_CPP_08_08 | 获取标定参数 | 08-090 | 是 | `getCalibrationParam()` 返回完整标定结构 |
| TC_CPP_08_09 | 未启动时 waitForFrames | 08-040 | 否 | 未调用 start 直接 waitForFrames 应抛异常 |
| TC_CPP_08_10 | Pipeline 多实例 | 08-010 | 2台 | 两个设备各自创建 Pipeline，独立运行 |
| TC_CPP_08_11 | 无 Config 启动 | 08-040 | 是 | `start()` 不带 Config，应使用默认配置启动 |

---

## 模块 09：流水线配置

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_09_01 | Config 启用深度流 | 09-010 | 是 | `enableStream(DEPTH, profile)` 成功 |
| TC_CPP_09_02 | Config 启用彩色流 | 09-010 | 是 | `enableStream(COLOR, profile)` 成功 |
| TC_CPP_09_03 | Config 启用多流 | 09-010, 09-020 | 是 | 同时启用 DEPTH + COLOR + IR，验证流数量 |
| TC_CPP_09_04 | Config 禁用指定流 | 09-030 | 是 | `enableAllStreams()` → `disableStream(COLOR)` → 无 Color 输出 |
| TC_CPP_09_05 | Config 对齐模式 | 09-040 | 是 | `setAlignMode(ALIGN_D2C_HW/SW/DISABLE)` |
| TC_CPP_09_06 | Config D2C 距离阈值 | 09-050 | 是 | `setDepthScaleRequire(true)` |
| TC_CPP_09_07 | Config 帧聚合模式 | 09-060 | 是 | `setFrameAggregateOutputMode()` 各模式 |
| TC_CPP_09_08 | Config 获取已启用列表 | 09-070 | 是 | `getEnabledStreamProfileList()` 与配置一致 |
| TC_CPP_09_09 | Config 克隆与复制 | 09-080 | 否 | Config 对象可复制，副本与原配置独立 |

---

## 模块 10：帧数据访问

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_10_01 | Frame 基本属性 | 10-010 | 是 | `getIndex()`, `getFrameType()`, `getDataSize()` 有效 |
| TC_CPP_10_02 | 时间戳三元组 | 10-020 | 是 | `getTimestampUs()`, `getSystemTimestampUs()`, `getGlobalTimestampUs()` >= 0 |
| TC_CPP_10_03 | 时间戳单调递增 | 10-020 | 是 | 连续帧时间戳严格递增（容差 < 1ms） |
| TC_CPP_10_04 | VideoFrame 宽高与格式 | 10-030 | 是 | `getWidth()`, `getHeight()`, `getFormat()` 与开流配置一致 |
| TC_CPP_10_05 | DepthFrame 深度值 | 10-040 | 是 | `getValueScale()`, `getPixel(100, 100)` 返回合理深度值 |
| TC_CPP_10_06 | DepthFrame 非全零 | 10-040 | 是 | 深度数据至少 10% 像素非零 |
| TC_CPP_10_07 | ColorFrame 数据非全零 | 10-050 | 是 | 色彩数据非全零 |
| TC_CPP_10_08 | IRFrame 数据非全零 | 10-060 | 是 | IR 数据非全零 |
| TC_CPP_10_09 | 帧数据缓冲区访问 | 10-070 | 是 | `getData()` 返回非空指针，大小与 `getDataSize()` 一致 |
| TC_CPP_10_10 | AccelFrame 值读取 | 10-080 | 是 | `getValue()` 返回 (x,y,z)，值在合理范围 |
| TC_CPP_10_11 | GyroFrame 值读取 | 10-090 | 是 | 同上 |
| TC_CPP_10_12 | FrameSet 帧计数 | 10-100 | 是 | `getFrameCount()` 与启用的流数量一致 |
| TC_CPP_10_13 | FrameSet 按类型获取帧 | 10-110 | 是 | `getFrame(OB_FRAME_DEPTH)` / `getFrame(OB_FRAME_COLOR)` 非空 |
| TC_CPP_10_14 | FrameSet 深度帧与彩色帧配对 | 10-110 | 是 | 同时启用 D+C，验证 FrameSet 包含两种帧 |

---

## 模块 11：帧元数据

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_11_01 | 元数据存在性检查 | 11-010 | 是 | `hasMetadata(OB_FRAME_METADATA_TYPE_TIMESTAMP)` 等 |
| TC_CPP_11_02 | 时间戳元数据 | 11-020 | 是 | `getMetadataValue(TIMESTAMP)` 与 `getTimestampUs()` 一致 |
| TC_CPP_11_03 | 曝光时间元数据 | 11-030 | 是 | `getMetadataValue(EXPOSURE)` 在合理范围 |
| TC_CPP_11_04 | 增益元数据 | 11-040 | 是 | `getMetadataValue(GAIN)` 有效 |
| TC_CPP_11_05 | 白平衡元数据 | 11-050 | 是 | `getMetadataValue(WHITE_BALANCE)` 有效 |
| TC_CPP_11_06 | 帧号元数据 | 11-060 | 是 | `getMetadataValue(FRAME_NUMBER)` 严格递增 |

---

## 模块 12：帧创建

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_12_01 | Frame 工厂创建 | 12-010 | 否 | `FrameFactory::createFrame()` 创建空帧 |
| TC_CPP_12_02 | VideoFrame 创建 | 12-020 | 否 | 创建指定格式/宽高的 VideoFrame |
| TC_CPP_12_03 | 帧数据填充与读取 | 12-030 | 否 | 向帧写入数据，读取验证一致 |
| TC_CPP_12_04 | 帧生命周期管理 | 12-040 | 否 | 帧引用计数正确，无内存泄漏 |

---

## 模块 13：滤波器

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_13_01 | PointCloudFilter 创建 | 13-010 | 是 | `PointCloudFilter()` 创建成功 |
| TC_CPP_13_02 | PointCloudFilter 点云生成 | 13-020 | 是 | 输入 DepthFrame → 输出 PointsFrame |
| TC_CPP_13_03 | PointCloudFilter RGB 点云 | 13-020 | 是 | 对齐 FrameSet 输入 → XYZRGB 点云 |
| TC_CPP_13_04 | Align Filter D2C | 13-030 | 是 | `Align(OB_STREAM_COLOR)` → Depth 对齐到 Color |
| TC_CPP_13_05 | Align Filter C2D | 13-030 | 是 | `Align(OB_STREAM_DEPTH)` → Color 对齐到 Depth |
| TC_CPP_13_06 | FormatConvertFilter | 13-040 | 是 | YUYV→RGB / MJPG→RGB 转换 |
| TC_CPP_13_07 | DecimationFilter | 13-050 | 是 | `setScaleValue(2)` → 输出分辨率减半 |
| TC_CPP_13_08 | ThresholdFilter | 13-060 | 是 | `setValueRange(100, 5000)` → 超出范围值被过滤 |
| TC_CPP_13_09 | SpatialFastFilter | 13-070 | 是 | 快速空间滤波处理 |
| TC_CPP_13_10 | SpatialModerateFilter | 13-080 | 是 | 中等空间滤波处理 |
| TC_CPP_13_11 | TemporalFilter | 13-090 | 是 | 时域平滑处理 |
| TC_CPP_13_12 | HoleFillingFilter | 13-100 | 是 | 空洞填充处理 |
| TC_CPP_13_13 | NoiseRemovalFilter | 13-110 | 是 | 噪点去除处理 |
| TC_CPP_13_14 | HdrMerge | 13-120 | 是 | HDR 序列合并（需设备支持 HDR） |
| TC_CPP_13_15 | SequenceIdFilter | 13-130 | 是 | `selectSequenceId(0)` → 仅通过对应 ID 帧 |
| TC_CPP_13_16 | DisparityTransform | 13-140 | 是 | Depth → Disparity → Depth 往返 |
| TC_CPP_13_17 | FalsePositiveFilter | 13-150 | 是 | 虚假深度去除 |

---

## 模块 14：设备属性控制

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_14_01 | 支持属性枚举 | 14-010 | 是 | `getSupportedPropertyCount() > 0`，遍历各项 |
| TC_CPP_14_02 | 属性支持查询 | 14-020 | 是 | `isPropertySupported(OB_PROP_LASER_BOOL)` 等 |
| TC_CPP_14_03 | Bool 属性读写 — Laser | 14-030 | 是 | 读 → 写 → 读回验证 |
| TC_CPP_14_04 | Int 属性读写 — Laser Power | 14-030 | 是 | range 内设置 |
| TC_CPP_14_05 | Int 属性 Range 查询 | 14-040 | 是 | `getIntPropertyRange()` min < max |
| TC_CPP_14_06 | Float 属性读写 | 14-030 | 是 | Laser Current 读写 |
| TC_CPP_14_07 | Float 属性 Range 查询 | 14-040 | 是 | min < max |
| TC_CPP_14_08 | Color AE 开关 | 14-050 | 是 | `OB_PROP_COLOR_AUTO_EXPOSURE_BOOL` 切换 |
| TC_CPP_14_09 | Color 曝光手动设置 | 14-060 | 是 | 关 AE → 设曝光 → 读回 |
| TC_CPP_14_10 | Color 增益手动设置 | 14-070 | 是 | 手动设置增益 |
| TC_CPP_14_11 | Color 白平衡手动设置 | 14-080 | 是 | 手动设置白平衡 |
| TC_CPP_14_12 | Depth/Color/IR Mirror | 14-090 | 是 | Mirror 属性开关 |
| TC_CPP_14_13 | Depth/Color/IR Flip | 14-100 | 是 | Flip 属性开关 |
| TC_CPP_14_14 | Depth Min/Max 阈值 | 14-110 | 是 | 设置深度最小/最大范围 |
| TC_CPP_14_15 | 不支持的属性读写 | 14-120 | 是 | 应抛异常不崩溃 |
| TC_CPP_14_16 | 属性超范围设置 | 14-130 | 是 | 超出 range 的值应抛异常 |
| TC_CPP_14_17 | 设备温度读取 | 14-140 | 是 | `OB_STRUCT_DEVICE_TEMPERATURE` |
| TC_CPP_14_18 | 基线标定参数 | 14-150 | 是 | `OB_STRUCT_BASELINE_CALIBRATION_PARAM` |

---

## 模块 15：深度工作模式

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_15_01 | 获取当前模式 | 15-010 | 是 | `getCurrentDepthWorkMode()` 有效 |
| TC_CPP_15_02 | 获取模式名称 | 15-020 | 是 | `getCurrentDepthWorkModeName()` 非空 |
| TC_CPP_15_03 | 枚举模式列表 | 15-030 | 是 | `getDepthWorkModeList()` count > 0 |
| TC_CPP_15_04 | 切换模式 | 15-040 | 是 | 切换到非当前模式 → 验证 → 切回 |
| TC_CPP_15_05 | 切换后 Profile 变化 | 15-050 | 是 | 切换后重新查 profile 列表 |

---

## 模块 16：设备预设

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_16_01 | 获取当前 Preset 名称 | 16-010 | 是 | `getCurrentPresetName()` 非空 |
| TC_CPP_16_02 | 枚举 Preset 列表 | 16-020 | 是 | `getAvailablePresetList()` count > 0 |
| TC_CPP_16_03 | 加载内置 Preset | 16-030 | 是 | `loadPreset(name)` → 当前名称变更 |
| TC_CPP_16_04 | 导出 Preset 为 JSON | 16-040 | 是 | `exportCurrentSettingsAsPresetJsonFile(path)` |
| TC_CPP_16_05 | 从 JSON 加载 Preset | 16-050 | 是 | 导出 → 加载 → 验证 |

---

## 模块 17：帧交错

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_17_01 | 支持查询 | 17-010 | 是 | `isFrameInterleaveSupported()` |
| TC_CPP_17_02 | 模式列表 | 17-020 | 是 | 若支持，`getFrameInterleaveModeList()` |
| TC_CPP_17_03 | 加载模式 | 17-030 | 是 | `loadFrameInterleave(mode)` |

---

## 模块 18：录制与回放

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_18_01 | RecordDevice 创建 | 18-010 | 是 | `RecordDevice(device, path)` |
| TC_CPP_18_02 | 录制启动与停止 | 18-020 | 是 | `startRecord()` 录制 3s → `stopRecord()` |
| TC_CPP_18_03 | 录制文件有效性 | 18-030 | 是 | 录制文件大小 > 1KB |
| TC_CPP_18_04 | PlaybackDevice 创建 | 18-040 | 否 | `PlaybackDevice(path)` |
| TC_CPP_18_05 | 回放读帧 | 18-050 | 否 | Pipeline(playbackDev) → 读取帧 |
| TC_CPP_18_06 | 回放时长与位置 | 18-060 | 否 | `getDuration()`, `getPosition()` |
| TC_CPP_18_07 | 回放 Seek | 18-070 | 否 | `seek(1000)` 跳转 |
| TC_CPP_18_08 | 回放倍速 | 18-080 | 否 | `setPlaybackRate(2.0)` |

---

## 模块 19：多设备同步

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_19_01 | 同步模式 bitmap 查询 | 19-010 | 是 | `getSupportedMultiDeviceSyncModeBitmap()` |
| TC_CPP_19_02 | 获取当前同步配置 | 19-020 | 是 | `getMultiDeviceSyncConfig()` |
| TC_CPP_19_03 | 设置 FREE_RUN 模式 | 19-030 | 是 | `setMultiDeviceSyncConfig()` |
| TC_CPP_19_04 | Software Trigger | 19-040 | 是 | `triggerCapture()` → 收到帧 |
| TC_CPP_19_05 | 时间戳重置 | 19-050 | 是 | `timestampReset()` |
| TC_CPP_19_06 | Timer 同步 | 19-060 | 是 | `timerSyncWithHost()` |
| TC_CPP_19_07 | PRIMARY/SECONDARY 配对 | 19-070 | 2台 | 两台设备配对同步 |

---

## 模块 20：坐标变换

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_20_01 | 3D→3D 坐标变换 | 20-010 | 否 | 单位外参 → src == dst |
| TC_CPP_20_02 | 2D+深度→3D 反投影 | 20-020 | 否 | 已知内参 + 深度 → 3D 坐标合理 |
| TC_CPP_20_03 | 3D→2D 投影 | 20-030 | 否 | 3D 点 → 像素在图像范围内 |
| TC_CPP_20_04 | Pipeline 获取相机参数 | 20-010 | 是 | `getCameraParam()` |
| TC_CPP_20_05 | 保存点云 PLY | 20-040 | 是 | `savePointCloudToPLY(path, frame)` |
| TC_CPP_20_06 | 多分辨率标定参数 | 20-050 | 是 | `getCalibrationCameraParamList()` |

---

## 模块 21：固件管理

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_21_01 | 固件升级 | 21-010 | 是 | `deviceUpgrade(path, callback)` |
| TC_CPP_21_02 | 固件升级进度回调 | 21-020 | 是 | 回调进度 0~100% |
| TC_CPP_21_03 | 升级后版本验证 | 21-030 | 是 | 升级后版本号正确 |
| TC_CPP_21_04 | 镜像文件升级 | 21-040 | 是 | `deviceUpgradeFromData(data, callback)` |
| TC_CPP_21_05 | 状态回调 | 21-050 | 是 | 状态变更通知 |
| TC_CPP_21_06 | 升级中断恢复 | 21-060 | 是 | 异常中断后可恢复 |
| TC_CPP_21_07 | 无效镜像处理 | 21-070 | 否 | 无效镜像应抛异常 |
| TC_CPP_21_08 | 版本兼容性检查 | 21-080 | 是 | 不兼容版本应拒绝 |
| TC_CPP_21_09 | 升级后设备重启 | 21-090 | 是 | 升级后设备能正常重启 |

---

## 模块 22：版本查询

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_22_01 | 版本号查询 | 22-010, 22-020, 22-030 | 否 | `getMajorVersion()`, `getMinorVersion()`, `getPatchVersion()` >= 0 |
| TC_CPP_22_02 | 完整版本号 | 22-040 | 否 | `getVersion()` > 0 |
| TC_CPP_22_03 | 版本字符串 | 22-050 | 否 | 版本号格式 x.y.z |

---

## 模块 23：日志系统

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_23_01 | 日志级别设置 | 23-010 | 否 | `setLoggerSeverity(DEBUG/WARN/ERROR/FATAL)` |
| TC_CPP_23_02 | 日志输出到文件 | 23-020 | 否 | `setLoggerToFile(severity, path)` |
| TC_CPP_23_03 | 日志输出到回调 | 23-030 | 否 | `setLoggerToCallback(severity, cb)` |
| TC_CPP_23_04 | 日志过滤器 | 23-040 | 否 | 按模块过滤 |
| TC_CPP_23_05 | 日志关闭 | 23-050 | 否 | `setLoggerSeverity(OB_LOG_SEVERITY_OFF)` |

---

## 模块 24：错误处理

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_24_01 | 无效设备索引 | 24-010 | 否 | 越界索引应抛异常 |
| TC_CPP_24_02 | 无效属性设置 | 24-020 | 是 | 不支持的属性应抛异常 |
| TC_CPP_24_03 | 设备断开处理 | 24-030 | 是 | 拔线后调用 API 应抛异常 |
| TC_CPP_24_04 | 资源释放安全 | 24-040 | 是 | 异常时资源正确释放 |
| TC_CPP_24_05 | Pipeline 异常恢复 | 24-050 | 是 | 异常后可重新启动 |
| TC_CPP_24_06 | Filter 异常安全 | 24-060 | 否 | `filter.process(nullptr)` 应抛异常 |
| TC_CPP_24_07 | 全异常类型覆盖 | 24-070 | 否 | 验证所有 `OBExceptionType` 枚举值 |

---

## 模块 25：关键数据结构

| 编号 | 用例名称 | 覆盖功能点 | 硬件 | 说明 |
|------|---------|-----------|------|------|
| TC_CPP_25_01 | 相机内参/畸变/外参结构体 | 25-010, 25-020, 25-030 | 是 | `OBCameraIntrinsic` fx/fy/cx/cy > 0；`OBCameraDistortion` k1~k6/p1/p2 可读；`OBExtrinsic` 旋转矩阵正交 |
| TC_CPP_25_02 | CameraParam/CalibrationParam | 25-040, 25-050 | 是 | `OBCameraParam` 含 depth+color 完整内参外参；`OBCalibrationParam` 多传感器标定数据有效 |
| TC_CPP_25_03 | IMU 内参结构体 | 25-060 | 是 | `OBAccelIntrinsic` / `OBGyroIntrinsic` 偏移/比例因子可读 |
| TC_CPP_25_04 | 设备温度结构体 | 25-070 | 是 | `OBDeviceTemperature` CPU/IR/Laser/TEC 温度在 0~80°C |
| TC_CPP_25_05 | 配置与模式结构体 | 25-080, 25-090, 25-100 | 是 | `OBDepthWorkMode` name 非空/checksum 有效；`OBMultiDeviceSyncConfig` 字段可读写；`OBNetIpConfig` IP 地址合法 |
| TC_CPP_25_06 | HDR/ROI/点云/IMU 结构体 | 25-110~25-140 | 是 | `OBHdrConfig` 序列配置有效；`OBRegionOfInterest` 矩形合理；`OBPoint3f`/`OBColorPoint` 坐标可读；`OBAccelValue`/`OBGyroValue` 三轴有效 |
| TC_CPP_25_07 | 属性范围与配置描述结构体 | 25-150, 25-160 | 否 | `OBIntPropertyRange`/`OBFloatPropertyRange` min/max/step/def/cur 字段完整；`OBFilterConfigSchemaItem` name/type/min/max/step/def 有效 |

---

## 详细测试用例文档

详细测试用例（含测试步骤、检查项、API 调用示例）请查看：

- [`docs/testcases/CPP/`](./) — 25 个详细测试文档（TC_CPP_01_*.md ~ TC_CPP_25_*.md）
