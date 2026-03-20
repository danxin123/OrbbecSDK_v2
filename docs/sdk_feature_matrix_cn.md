# Orbbec SDK V2 — 功能矩阵

按功能模块和功能点组织的 SDK 功能清单。

**编号规则**：`模块号-序号`，序号以 10 为间隔，预留扩展空间。子模块使用 `模块号.子节-序号` 格式。

---

## 一、SDK 初始化与全局管理

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 01-010 | 默认初始化 | 使用默认配置创建 SDK 上下文 | `ob_create_context()` | `Context()` |
| 01-020 | 配置文件初始化 | 使用指定配置文件创建上下文 | `ob_create_context_with_config(path)` | `Context(configPath)` |
| 01-030 | 上下文销毁 | 释放 SDK 资源 | `ob_delete_context(ctx)` | 析构函数 |
| 01-040 | 空闲内存释放 | 释放帧内存池 | `ob_free_idle_memory(ctx)` | `freeIdleMemory()` |
| 01-050 | UVC 后端选择 | Linux 下选择 libuvc/v4l2/MSMF 后端 | `ob_set_uvc_backend_type(ctx, type)` | `setUvcBackendType(type)` |
| 01-060 | 扩展插件目录 | 设置 Filter 插件加载路径 | `ob_set_extensions_directory(dir)` | — |

## 二、设备发现与枚举

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 02-010 | USB 设备枚举 | 查询已连接的 USB 设备列表 | `ob_query_device_list(ctx)` | `queryDeviceList()` |
| 02-020 | 网络设备发现 | 启用/禁用 GVCP 网络自动发现 | `ob_enable_net_device_enumeration(ctx, enable)` | `enableNetDeviceEnumeration(bool)` |
| 02-030 | 网络设备直连 | 按 IP 地址和端口连接网络设备 | `ob_create_net_device(ctx, addr, port)` | `createNetDevice(addr, port)` |
| 02-040 | 网络设备直连（访问模式） | 指定访问模式连接网络设备 | `ob_create_net_device_ex(ctx, addr, port, mode)` | `createNetDevice(addr, port, mode)` |
| 02-050 | 强制 IP 配置 | 按 MAC 地址为网络设备配置静态 IP | `ob_force_ip_config(mac, config)` | `forceIp(mac, config)` |
| 02-060 | 设备热插拔回调 | 注册/注销设备连接变化回调 | `ob_register_device_changed_callback()` / `ob_unregister_device_changed_callback()` | `registerDeviceChangedCallback()` / `unregisterDeviceChangedCallback()` |
| 02-070 | 时钟同步 | 启用主机-设备跨设备时钟同步 | `ob_enable_device_clock_sync(ctx, interval_ms)` | `enableDeviceClockSync(interval)` |

## 三、设备列表查询

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 03-010 | 设备数量 | 获取列表中的设备数 | `ob_device_list_get_count()` | `deviceCount()` |
| 03-020 | 按索引获取 | 按索引获取设备句柄 | `ob_device_list_get_device(list, idx)` | `getDevice(idx)` |
| 03-030 | 按索引+访问模式获取 | 指定访问模式打开设备 | `ob_device_list_get_device_ex(list, idx, mode)` | — |
| 03-040 | 按序列号查找 | 用 SN 定位特定设备 | `ob_device_list_get_device_by_serial_number()` | `getDeviceBySN()` |
| 03-050 | 按 UID 查找 | 用唯一 ID 定位设备 | `ob_device_list_get_device_by_uid()` | `getDeviceByUid()` |
| 03-060 | 设备名称查询 | 获取设备产品名称 | `ob_device_list_get_device_name()` | — |
| 03-070 | PID/VID 查询 | 获取产品 ID 和厂商 ID | `ob_device_list_get_device_pid/vid()` | — |
| 03-080 | 连接类型查询 | 获取 "USB" 或 "Ethernet" | `ob_device_list_get_device_connection_type()` | — |
| 03-090 | 网络信息查询 | 获取设备/主机的 IP、子网、网关、MAC | `ob_device_list_get_device_ip_address/subnet_mask/gateway/local_mac/local_ip()` | — |

## 四、设备信息

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 04-010 | 设备名称 | 产品型号名称 | `ob_device_info_get_name()` | `name()` |
| 04-020 | 序列号 | 唯一生产序列号 | `ob_device_info_get_serial_number()` | `serialNumber()` |
| 04-030 | 固件版本 | 当前固件版本字符串 | `ob_device_info_get_firmware_version()` | `firmwareVersion()` |
| 04-040 | 硬件版本 | 硬件修订版本 | `ob_device_info_get_hardware_version()` | `hardwareVersion()` |
| 04-050 | PID/VID/UID | 产品 ID、厂商 ID、唯一 ID | `ob_device_info_get_pid/vid/uid()` | `pid()` / `vid()` / `uid()` |
| 04-060 | 连接类型 | "USB" 或 "Ethernet" | `ob_device_info_get_connection_type()` | `connectionType()` |
| 04-070 | IP 地址信息 | 网络设备的 IP、子网、网关 | `ob_device_info_get_ip_address/subnet_mask/gateway()` | `ipAddress()` 等 |
| 04-080 | ASIC 名称 | 主芯片型号 | `ob_device_info_get_asicName()` | `asicName()` |
| 04-090 | 设备类型 | 设备类型枚举值 | `ob_device_info_get_device_type()` | `deviceType()` |
| 04-100 | 最低 SDK 版本 | 设备要求的最低 SDK 版本 | `ob_device_info_get_supported_min_sdk_version()` | `supportedMinSdkVersion()` |
| 04-110 | 扩展信息 | 查询/获取设备扩展键值对 | `ob_device_is_extension_info_exist()` / `ob_device_get_extension_info()` | 对应方法 |

## 五、设备访问模式

| 编号 | 模式 | 描述 |
|------|------|------|
| 05-010 | `DEFAULT_ACCESS` | 默认访问模式 |
| 05-020 | `EXCLUSIVE_ACCESS` | 独占访问，其他进程无法打开 |
| 05-030 | `SHARED_ACCESS` | 共享访问，多进程可同时打开 |
| 05-040 | `CONTROL_ONLY_ACCESS` | 仅控制，不可流传输 |

## 六、传感器枚举

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 06-010 | 传感器列表 | 获取设备上所有传感器 | `ob_device_get_sensor_list()` | `getSensorList()` |
| 06-020 | 按类型获取传感器 | 获取特定类型的传感器 | `ob_device_get_sensor(dev, type)` | `getSensor(type)` |
| 06-030 | 传感器类型 | 查询传感器类型枚举 | `ob_sensor_get_type()` | `type()` |

### 传感器类型列表

| 编号 | 传感器类型 | 说明 |
|------|-----------|------|
| 06-100 | `DEPTH` | 深度传感器 |
| 06-110 | `COLOR` | 彩色相机 |
| 06-120 | `IR` | 红外传感器 |
| 06-130 | `IR_LEFT` | 左红外（双目立体） |
| 06-140 | `IR_RIGHT` | 右红外（双目立体） |
| 06-150 | `ACCEL` | 加速度计 |
| 06-160 | `GYRO` | 陀螺仪 |
| 06-170 | `CONFIDENCE` | 置信度图 |
| 06-180 | `RAW_PHASE` | 原始相位 |
| 06-190 | `LIDAR` | LiDAR |
| 06-200 | `COLOR_LEFT` | 左彩色 |
| 06-210 | `COLOR_RIGHT` | 右彩色 |

## 七、流配置（StreamProfile）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 07-010 | Profile 列表查询 | 获取传感器支持的所有配置 | `ob_sensor_get_stream_profile_list()` | `getStreamProfileList()` |
| 07-020 | 按参数筛选 | 按分辨率/帧率/格式筛选 Profile | `ob_stream_profile_list_get_video_stream_profile()` | `getVideoStreamProfile(w,h,fmt,fps)` |
| 07-030 | 视频流属性 | 宽、高、帧率、格式 | `ob_video_stream_profile_get_width/height/fps/format()` | `width()` / `height()` / `fps()` / `format()` |
| 07-040 | 加速度计属性 | 量程、采样率 | `ob_accel_stream_profile_get_full_scale_range/sample_rate()` | `fullScaleRange()` / `sampleRate()` |
| 07-050 | 陀螺仪属性 | 量程、采样率 | `ob_gyro_stream_profile_get_full_scale_range/sample_rate()` | `fullScaleRange()` / `sampleRate()` |
| 07-060 | 类型检查/转换 | StreamProfile 子类型判定 | — | `is<T>()` / `as<T>()` |

### 像素格式列表（38 种）

| 编号 | 分类 | 格式 |
|------|------|------|
| 07-100 | YUV/RGB | `YUYV`, `YUY2`, `UYVY`, `NV12`, `NV21`, `I420`, `GRAY`, `RGB`, `BGR`, `BGRA`, `RGBA`, `BYR2`, `RW16`, `YV12`, `BA81` |
| 07-110 | 编码 | `MJPG`, `H264`, `H265`/`HEVC` |
| 07-120 | 深度/IR 原始 | `Y16`, `Y8`, `Y10`, `Y11`, `Y12`, `Y14`, `Z16`, `Y12C4`, `RLE`, `RVL`, `COMPRESSED` |
| 07-130 | IMU | `ACCEL`, `GYRO` |
| 07-140 | 点云 | `POINT`（XYZ）, `RGB_POINT`（XYZRGB） |
| 07-150 | LiDAR | `LIDAR_POINT`, `LIDAR_SPHERE_POINT`, `LIDAR_SCAN`, `LIDAR_CALIBRATION` |

## 八、流水线（Pipeline）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 08-010 | 默认创建 | 自动绑定首个可用设备 | `ob_create_pipeline()` | `Pipeline()` |
| 08-020 | 指定设备创建 | 绑定到特定设备 | `ob_create_pipeline_with_device(dev)` | `Pipeline(device)` |
| 08-030 | 默认开流 | 使用默认配置启动所有默认流 | `ob_pipeline_start(pipe)` | `start()` |
| 08-040 | 配置开流（轮询） | 使用 Config 启动，通过 waitForFrames 取帧 | `ob_pipeline_start_with_config(pipe, config)` | `start(config)` |
| 08-050 | 配置开流（回调） | 使用 Config 启动，通过回调接收帧 | `ob_pipeline_start_with_callback(pipe, config, cb)` | `start(config, callback)` |
| 08-060 | 停流 | 停止所有流传输 | `ob_pipeline_stop(pipe)` | `stop()` |
| 08-070 | 轮询取帧 | 阻塞等待帧集合 | `ob_pipeline_wait_for_frameset(pipe, timeout_ms)` | `waitForFrames(timeout)` |
| 08-080 | 运行时切换配置 | 开流状态下切换流组合/分辨率 | `ob_pipeline_switch_config(pipe, config)` | `switchConfig(config)` |
| 08-090 | 获取绑定设备 | 获取 Pipeline 关联的设备 | `ob_pipeline_get_device(pipe)` | `getDevice()` |
| 08-100 | 获取 StreamProfile | 获取特定传感器可用的 Profile 列表 | `ob_pipeline_get_stream_profile_list(pipe, sensorType)` | `getStreamProfileList(sensorType)` |
| 08-110 | 帧同步开关 | 启用/禁用基于时间戳的多流帧同步 | `ob_pipeline_enable/disable_frame_sync()` | `enableFrameSync()` / `disableFrameSync()` |
| 08-120 | D2C 兼容 Profile | 获取与指定 Color Profile 兼容的 Depth Profile 列表 | `ob_get_d2c_depth_profile_list()` | `getD2CDepthProfileList()` |
| 08-130 | 获取相机参数 | 获取内参、外参 | `ob_pipeline_get_camera_param()` | `getCameraParam()` |
| 08-140 | 获取指定分辨率参数 | 按特定分辨率获取相机参数 | `ob_pipeline_get_camera_param_with_profile()` | `getCameraParamWithProfile()` |
| 08-150 | 获取完整标定参数 | 获取多传感器完整标定 | `ob_pipeline_get_calibration_param()` | `getCalibrationParam()` |

## 九、流水线配置（Config）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 09-010 | 按类型启用流 | 启用某类流（使用默认 Profile） | `ob_config_enable_stream(config, type)` | `enableStream(type)` |
| 09-020 | 按 Profile 启用流 | 启用指定 Profile 的流 | `ob_config_enable_stream_with_stream_profile(config, profile)` | `enableStream(profile)` |
| 09-030 | 参数化启用视频流 | 指定宽、高、帧率、格式启用 | `ob_config_enable_video_stream(config, type, w, h, fps, fmt)` | `enableVideoStream(...)` |
| 09-040 | 启用加速度计 | 指定量程和采样率 | `ob_config_enable_accel_stream(config, fullScale, sampleRate)` | `enableAccelStream(...)` |
| 09-050 | 启用陀螺仪 | 指定量程和采样率 | `ob_config_enable_gyro_stream(config, fullScale, sampleRate)` | `enableGyroStream(...)` |
| 09-060 | 启用 LiDAR | 指定扫描频率和格式 | `ob_config_enable_lidar_stream(config, scanRate, format)` | `enableLidarStream(...)` |
| 09-070 | 启用所有流 | 一键启用设备所有支持的流 | `ob_config_enable_all_stream(config)` | `enableAllStream()` |
| 09-080 | 禁用特定流 | 关闭某一类流 | `ob_config_disable_stream(config, type)` | `disableStream(type)` |
| 09-090 | 禁用所有流 | 关闭所有流 | `ob_config_disable_all_stream(config)` | `disableAllStream()` |
| 09-100 | 查询已启用 Profile | 获取当前配置中已启用的流列表 | `ob_config_get_enabled_stream_profile_list()` | `getEnabledStreamProfileList()` |
| 09-110 | D2C 对齐模式 | 设置深度到色彩对齐方式 | `ob_config_set_align_mode(config, mode)` | `setAlignMode(mode)` |
| 09-120 | 对齐后深度缩放 | 对齐后是否缩放深度值 | `ob_config_set_depth_scale_after_align_require()` | `setDepthScaleAfterAlignRequire()` |
| 09-130 | 帧聚合输出模式 | 设置多流帧输出策略 | `ob_config_set_frame_aggregate_output_mode()` | `setFrameAggregateOutputMode()` |

### D2C 对齐模式

| 编号 | 模式 | 描述 |
|------|------|------|
| 09-200 | `ALIGN_DISABLE` | 不做对齐 |
| 09-210 | `ALIGN_D2C_HW_MODE` | 硬件深度到色彩对齐 |
| 09-220 | `ALIGN_D2C_SW_MODE` | 软件深度到色彩对齐 |

### 帧聚合输出模式

| 编号 | 模式 | 描述 |
|------|------|------|
| 09-300 | `ALL_TYPE_FRAME_REQUIRE` | 所有流都有帧时才输出 |
| 09-310 | `COLOR_FRAME_REQUIRE` | 色彩帧可用时即输出 |
| 09-320 | `ANY_SITUATION` | 任何情况下都输出（默认） |
| 09-330 | `DISABLE` | 禁用帧聚合 |

## 十、帧数据访问

### 10.1 Frame 通用属性

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.1-010 | 帧序号 | 帧序列索引 | `ob_frame_get_index()` | `index()` |
| 10.1-020 | 帧格式 | 像素/数据格式 | `ob_frame_get_format()` | `format()` |
| 10.1-030 | 帧类型 | DEPTH / COLOR / IR 等 | `ob_frame_get_type()` | `type()` |
| 10.1-040 | 设备时间戳 | 设备端时间戳（微秒） | `ob_frame_get_timestamp_us()` | `timeStampUs()` |
| 10.1-050 | 系统时间戳 | 主机端接收时间戳 | `ob_frame_get_system_timestamp_us()` | `systemTimeStampUs()` |
| 10.1-060 | 全局时间戳 | 跨设备同步后的全局时间戳 | `ob_frame_get_global_timestamp_us()` | `globalTimeStampUs()` |
| 10.1-070 | 原始数据指针 | 获取帧原始数据 | `ob_frame_get_data()` | `data()` |
| 10.1-080 | 数据大小 | 帧数据缓冲区大小 | `ob_frame_get_data_size()` | `dataSize()` |
| 10.1-090 | 关联 StreamProfile | 获取帧对应的流配置 | `ob_frame_get_stream_profile()` | `getStreamProfile()` |
| 10.1-100 | 源传感器 | 获取帧来源传感器 | `ob_frame_get_sensor()` | `getSensor()` |
| 10.1-110 | 源设备 | 获取帧来源设备 | `ob_frame_get_device()` | `getDevice()` |
| 10.1-120 | 引用计数 | 增加帧引用计数 | `ob_frame_add_ref()` | shared_ptr 自动管理 |

### 10.2 VideoFrame 属性

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.2-010 | 图像宽度 | 像素宽度 | `ob_video_frame_get_width()` | `width()` |
| 10.2-020 | 图像高度 | 像素高度 | `ob_video_frame_get_height()` | `height()` |
| 10.2-030 | 像素类型 | 像素数据类型 | `ob_video_frame_get_pixel_type()` | `pixelType()` |
| 10.2-040 | 像素位深 | 每像素有效位数 | `ob_video_frame_get_pixel_available_bit_size()` | `pixelAvailableBitSize()` |

### 10.3 DepthFrame 属性

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.3-010 | 深度单位比例 | 深度值到毫米的比例因子 | `ob_depth_frame_get_value_scale()` | `getValueScale()` |

### 10.4 PointsFrame 属性

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.4-010 | 坐标缩放 | 点云坐标缩放因子 | `ob_points_frame_get_coordinate_value_scale()` | `getCoordinateValueScale()` |
| 10.4-020 | 点云宽高 | 点云组织宽度/高度 | `ob_point_cloud_frame_get_width/height()` | `width()` / `height()` |

### 10.5 IMU 帧

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.5-010 | 加速度值 | 三轴加速度 (x, y, z) | `ob_accel_frame_get_value()` | `value()` |
| 10.5-020 | 陀螺仪值 | 三轴角速度 (x, y, z) | `ob_gyro_frame_get_value()` | `value()` |
| 10.5-030 | IMU 温度 | 传感器温度 | `ob_accel/gyro_frame_get_temperature()` | `temperature()` |

### 10.6 FrameSet 操作

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 10.6-010 | 帧数量 | FrameSet 中的帧数 | `ob_frameset_get_count()` | `frameCount()` |
| 10.6-020 | 提取深度帧 | 从集合中提取深度帧 | `ob_frameset_get_depth_frame()` | `depthFrame()` |
| 10.6-030 | 提取色彩帧 | 从集合中提取色彩帧 | `ob_frameset_get_color_frame()` | `colorFrame()` |
| 10.6-040 | 提取 IR 帧 | 从集合中提取 IR 帧 | `ob_frameset_get_ir_frame()` | `irFrame()` |
| 10.6-050 | 提取点云帧 | 从集合中提取点云帧 | `ob_frameset_get_points_frame()` | `pointsFrame()` |
| 10.6-060 | 按类型提取 | 按帧类型提取 | `ob_frameset_get_frame(frameset, type)` | `getFrame(type)` |
| 10.6-070 | 按索引提取 | 按索引提取 | `ob_frameset_get_frame_by_index(frameset, idx)` | `getFrame(idx)` |
| 10.6-080 | 添加帧 | 向 FrameSet 中添加帧 | `ob_frameset_push_frame()` | `pushFrame()` |

## 十一、帧元数据

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 11-010 | 检查元数据是否存在 | 查询特定元数据字段是否可用 | `ob_frame_has_metadata(frame, type)` | `hasMetadata(type)` |
| 11-020 | 读取元数据值 | 获取 int64 类型的元数据值 | `ob_frame_get_metadata_value(frame, type)` | `getMetadataValue(type)` |
| 11-030 | 原始元数据缓冲 | 获取元数据原始字节 | `ob_frame_get_metadata()` | `getMetadata()` |
| 11-040 | 更新元数据 | 修改帧的元数据 | `ob_frame_update_metadata()` | — |

### 元数据字段列表（34 种）

| 编号 | 分类 | 字段 |
|------|------|------|
| 11-100 | 时间与帧号 | `TIMESTAMP`, `SENSOR_TIMESTAMP`, `FRAME_NUMBER` |
| 11-110 | 曝光与增益 | `AUTO_EXPOSURE`, `EXPOSURE`, `GAIN`, `EXPOSURE_PRIORITY` |
| 11-120 | 白平衡 | `AUTO_WHITE_BALANCE`, `WHITE_BALANCE`, `MANUAL_WHITE_BALANCE` |
| 11-130 | 图像调节 | `BRIGHTNESS`, `CONTRAST`, `SATURATION`, `SHARPNESS`, `HUE`, `GAMMA` |
| 11-140 | 补偿 | `BACKLIGHT_COMPENSATION`, `LOW_LIGHT_COMPENSATION`, `POWER_LINE_FREQUENCY` |
| 11-150 | 帧率 | `ACTUAL_FRAME_RATE`, `FRAME_RATE` |
| 11-160 | AE ROI | `AE_ROI_LEFT`, `AE_ROI_TOP`, `AE_ROI_RIGHT`, `AE_ROI_BOTTOM` |
| 11-170 | HDR | `HDR_SEQUENCE_NAME`, `HDR_SEQUENCE_SIZE`, `HDR_SEQUENCE_INDEX` |
| 11-180 | 激光 | `LASER_POWER`, `LASER_POWER_LEVEL`, `LASER_STATUS` |
| 11-190 | 其他 | `GPIO_INPUT_DATA`, `DISPARITY_SEARCH_OFFSET`, `DISPARITY_SEARCH_RANGE` |

## 十二、帧创建（FrameFactory）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 12-010 | 创建空帧 | 按类型、格式、大小创建 | `ob_create_frame(type, format, size)` | `FrameFactory::createFrame()` |
| 12-020 | 创建视频帧 | 指定宽高步长创建 | `ob_create_video_frame(type, fmt, w, h, stride)` | `FrameFactory::createVideoFrame()` |
| 12-030 | 克隆帧 | 从现有帧复制创建 | `ob_create_frame_from_other_frame(other, copy)` | `FrameFactory::createFrameFromOtherFrame()` |
| 12-040 | 从 Profile 创建 | 根据 StreamProfile 创建帧 | `ob_create_frame_from_stream_profile()` | `FrameFactory::createFrameFromStreamProfile()` |
| 12-050 | 外部 Buffer 包装 | 将外部内存包装为 Frame | `ob_create_frame_from_buffer(...)` | `FrameFactory::createFrameFromBuffer()` |
| 12-060 | 外部 Buffer 包装(视频) | 将外部内存包装为 VideoFrame | `ob_create_video_frame_from_buffer(...)` | `FrameFactory::createVideoFrameFromBuffer()` |
| 12-070 | 创建空 FrameSet | 创建空的帧集合 | `ob_create_frameset()` | `FrameFactory::createFrameSet()` |

## 十三、滤波器（Filter）

### 13.1 Filter 通用操作

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 13.1-010 | 按名称创建 | 用字符串名称创建 Filter | `ob_create_filter(name)` | `Filter(name)` / `FilterFactory::createFilter(name)` |
| 13.1-020 | 创建私有 Filter | 需要激活密钥的 Filter | `ob_create_private_filter(name, key)` | `FilterFactory::createPrivateFilter(name, key)` |
| 13.1-030 | 同步处理 | 输入帧 → 立即返回处理后的帧 | `ob_filter_process(filter, frame)` | `process(frame)` |
| 13.1-040 | 异步回调处理 | 设置回调 + 推入帧 | `ob_filter_set_callback()` + `ob_filter_push_frame()` | `setCallback()` + `pushFrame()` |
| 13.1-050 | 启用/禁用 | 开关 Filter 处理（禁用时直通） | `ob_filter_enable(filter, enable)` | `enable(bool)` |
| 13.1-060 | 查询启用状态 | 检查是否启用 | `ob_filter_is_enabled()` | `isEnabled()` |
| 13.1-070 | 重置状态 | 清除 Filter 内部缓存 | `ob_filter_reset()` | `reset()` |
| 13.1-080 | 获取配置模式 | 获取 Filter 可配置参数 schema | `ob_filter_get_config_schema()` | `getConfigSchema()` / `getConfigSchemaVec()` |
| 13.1-090 | 读取配置值 | 读取 Filter 参数 | `ob_filter_get_config_value(name)` | `getConfigValue(name)` |
| 13.1-100 | 设置配置值 | 修改 Filter 参数 | `ob_filter_set_config_value(name, value)` | `setConfigValue(name, value)` |
| 13.1-110 | 类型检查/转换 | 判定 Filter 子类型 | — | `is<T>()` / `as<T>()` |

### 13.2 内置 Filter 类型

| 编号 | Filter 名称 | 用途 | 关键配置方法 |
|------|-------------|------|-------------|
| 13.2-010 | `PointCloudFilter` | 深度帧转点云 (XYZ / XYZRGB) | `setCreatePointFormat()`, `setCameraParam()`, `setCoordinateDataScaled()`, `setDecimationFactor()` |
| 13.2-020 | `Align` | 深度与色彩帧对齐（D2C / C2D） | `setAlignToStreamProfile()`, `setMatchTargetResolution()` |
| 13.2-030 | `FormatConverter` | 像素格式转换 (YUYV→RGB 等) | `setFormatConvertType()` |
| 13.2-040 | `HDRMerge` | HDR 多曝光序列合并为单帧 | — |
| 13.2-050 | `SequenceIdFilter` | 按序列 ID 筛选帧 | `selectSequenceId()`, `getSelectSequenceId()`, `getSequenceIdListSize()` |
| 13.2-060 | `DecimationFilter` | 深度图降采样 | `setScaleValue()` |
| 13.2-070 | `ThresholdFilter` | 深度值范围阈值过滤 | `setValueRange(min, max)` |
| 13.2-080 | `SpatialAdvancedFilter` | 高级空间滤波（需激活密钥） | `setFilterParams(magnitude, alpha, disp_diff, radius)` |
| 13.2-090 | `SpatialFastFilter` | 快速空间滤波 | 通用配置接口 |
| 13.2-100 | `SpatialModerateFilter` | 中等空间滤波 | 通用配置接口 |
| 13.2-110 | `HoleFillingFilter` | 深度图空洞填充 | 通用配置接口 |
| 13.2-120 | `NoiseRemovalFilter` | 深度噪点去除 | 通用配置接口 |
| 13.2-130 | `TemporalFilter` | 时域平滑滤波 | 通用配置接口 |
| 13.2-140 | `FalsePositiveFilter` | 虚假深度值去除 | 通用配置接口 |
| 13.2-150 | `DisparityTransform` | 深度↔视差转换 | 通用配置接口 |

### 13.3 格式转换类型（22 种）

| 编号 | 转换 | 编号 | 转换 | 编号 | 转换 |
|------|------|------|------|------|------|
| 13.3-010 | `YUYV_TO_RGB` | 13.3-020 | `I420_TO_RGB` | 13.3-030 | `NV21_TO_RGB` |
| 13.3-040 | `NV12_TO_RGB` | 13.3-050 | `MJPG_TO_I420` | 13.3-060 | `RGB_TO_BGR` |
| 13.3-070 | `MJPG_TO_NV21` | 13.3-080 | `MJPG_TO_RGB` | 13.3-090 | `MJPG_TO_BGR` |
| 13.3-100 | `MJPG_TO_BGRA` | 13.3-110 | `UYVY_TO_RGB` | 13.3-120 | `BGR_TO_RGB` |
| 13.3-130 | `MJPG_TO_NV12` | 13.3-140 | `YUYV_TO_BGR` | 13.3-150 | `YUYV_TO_RGBA` |
| 13.3-160 | `YUYV_TO_BGRA` | 13.3-170 | `YUYV_TO_Y16` | 13.3-180 | `YUYV_TO_Y8` |
| 13.3-190 | `RGBA_TO_RGB` | 13.3-200 | `BGRA_TO_BGR` | 13.3-210 | `Y16_TO_RGB` |
| 13.3-220 | `Y8_TO_RGB` | | | | |

## 十四、设备属性控制

### 14.1 属性访问接口

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 14.1-010 | 设置整数属性 | 写入 int 值 | `ob_device_set_int_property()` | `setIntProperty()` |
| 14.1-020 | 获取整数属性 | 读取 int 值 | `ob_device_get_int_property()` | `getIntProperty()` |
| 14.1-030 | 整数属性范围 | 获取 min/max/step/default | `ob_device_get_int_property_range()` | `getIntPropertyRange()` |
| 14.1-040 | 设置浮点属性 | 写入 float 值 | `ob_device_set_float_property()` | `setFloatProperty()` |
| 14.1-050 | 获取浮点属性 | 读取 float 值 | `ob_device_get_float_property()` | `getFloatProperty()` |
| 14.1-060 | 浮点属性范围 | 获取 min/max/step/default | `ob_device_get_float_property_range()` | `getFloatPropertyRange()` |
| 14.1-070 | 设置布尔属性 | 写入 bool 值 | `ob_device_set_bool_property()` | `setBoolProperty()` |
| 14.1-080 | 获取布尔属性 | 读取 bool 值 | `ob_device_get_bool_property()` | `getBoolProperty()` |
| 14.1-090 | 结构体属性读写 | 读写复杂结构数据 | `ob_device_get/set_structured_data()` | `getStructuredData()` / `setStructuredData()` |
| 14.1-100 | 原始数据读取 | 通过回调获取原始数据 | `ob_device_get_raw_data()` | `getRawData()` |
| 14.1-110 | 自定义用户数据 | 读写设备上的用户自定义数据 | `ob_device_write/read_customer_data()` | `writeCustomerData()` / `readCustomerData()` |
| 14.1-120 | 支持属性枚举 | 列出设备支持的所有属性 | `ob_device_get_supported_property_count/item()` | `getSupportedPropertyCount/Item()` |
| 14.1-130 | 属性支持查询 | 检查特定属性是否支持 | `ob_device_is_property_supported()` | `isPropertySupported()` |

### 14.2 激光/发射器控制属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.2-010 | `OB_PROP_LDP_BOOL`（2） | bool | LDP（激光安全保护）开关 |
| 14.2-020 | `OB_PROP_LASER_BOOL`（3） | bool | 激光开/关 |
| 14.2-030 | `OB_PROP_LASER_PULSE_WIDTH_INT`（4） | int | 激光脉冲宽度 |
| 14.2-040 | `OB_PROP_LASER_CURRENT_FLOAT`（5） | float | 激光电流（mA） |
| 14.2-050 | `OB_PROP_FLOOD_BOOL`（6） | bool | IR 泛光灯开关 |
| 14.2-060 | `OB_PROP_FLOOD_LEVEL_INT`（7） | int | IR 泛光灯等级 |
| 14.2-070 | `OB_PROP_LASER_MODE_INT`（79） | int | 激光模式（1=IR Drive, 2=Torch） |
| 14.2-080 | `OB_PROP_LASER_POWER_LEVEL_CONTROL_INT`（99） | int | 激光功率等级设置 |
| 14.2-090 | `OB_PROP_LASER_POWER_ACTUAL_LEVEL_INT`（119） | int | 实际激光功率等级（只读） |
| 14.2-100 | `OB_PROP_LASER_OVERCURRENT_PROTECTION_STATUS_BOOL`（148） | bool | 过流保护状态 |
| 14.2-110 | `OB_PROP_LASER_ALWAYS_ON_BOOL`（174） | bool | 激光常亮 |
| 14.2-120 | `OB_PROP_LASER_ON_OFF_PATTERN_INT`（175） | int | 激光交错模式 |
| 14.2-130 | `OB_PROP_LASER_CONTROL_INT`（182） | int | 激光控制（0=off, 1=on, 2=auto） |

### 14.3 深度控制属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.3-010 | `OB_PROP_DEPTH_MIRROR_BOOL`（14） | bool | 深度图镜像 |
| 14.3-020 | `OB_PROP_DEPTH_FLIP_BOOL`（15） | bool | 深度图翻转 |
| 14.3-030 | `OB_PROP_DEPTH_POSTFILTER_BOOL`（16） | bool | 后处理滤波开关 |
| 14.3-040 | `OB_PROP_DEPTH_HOLEFILTER_BOOL`（17） | bool | 空洞填充开关 |
| 14.3-050 | `OB_PROP_MIN_DEPTH_INT`（22） | int | 最小深度阈值 |
| 14.3-060 | `OB_PROP_MAX_DEPTH_INT`（23） | int | 最大深度阈值 |
| 14.3-070 | `OB_PROP_DEPTH_NOISE_REMOVAL_FILTER_BOOL`（24） | bool | 降噪滤波开关 |
| 14.3-080 | `OB_PROP_DEPTH_ALIGN_HARDWARE_BOOL`（42） | bool | 硬件 D2C 对齐开关 |
| 14.3-090 | `OB_PROP_DEPTH_PRECISION_LEVEL_INT`（75） | int | 深度精度等级 |
| 14.3-100 | `OB_PROP_DEPTH_CROPPING_MODE_INT`（90） | int | 裁剪模式 |
| 14.3-110 | `OB_PROP_DEPTH_UNIT_FLEXIBLE_ADJUSTMENT_FLOAT`（176） | float | 连续深度单位调节 |
| 14.3-120 | `OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL`（2016） | bool | 深度自动曝光 |
| 14.3-130 | `OB_PROP_DEPTH_EXPOSURE_INT`（2017） | int | 深度曝光值 |
| 14.3-140 | `OB_PROP_DEPTH_GAIN_INT`（2018） | int | 深度增益 |
| 14.3-150 | `OB_PROP_DISPARITY_TO_DEPTH_BOOL`（85） | bool | 硬件视差转深度 |

### 14.4 色彩控制属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.4-010 | `OB_PROP_COLOR_MIRROR_BOOL`（81） | bool | 镜像 |
| 14.4-020 | `OB_PROP_COLOR_FLIP_BOOL`（82） | bool | 翻转 |
| 14.4-030 | `OB_PROP_COLOR_AUTO_EXPOSURE_BOOL`（2000） | bool | 自动曝光 |
| 14.4-040 | `OB_PROP_COLOR_EXPOSURE_INT`（2001） | int | 曝光值 |
| 14.4-050 | `OB_PROP_COLOR_GAIN_INT`（2002） | int | 增益 |
| 14.4-060 | `OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL`（2003） | bool | 自动白平衡 |
| 14.4-070 | `OB_PROP_COLOR_WHITE_BALANCE_INT`（2004） | int | 白平衡 |
| 14.4-080 | `OB_PROP_COLOR_BRIGHTNESS_INT`（2005） | int | 亮度 |
| 14.4-090 | `OB_PROP_COLOR_SHARPNESS_INT`（2006） | int | 锐度 |
| 14.4-100 | `OB_PROP_COLOR_SATURATION_INT`（2008） | int | 饱和度 |
| 14.4-110 | `OB_PROP_COLOR_CONTRAST_INT`（2009） | int | 对比度 |
| 14.4-120 | `OB_PROP_COLOR_GAMMA_INT`（2010） | int | Gamma |
| 14.4-130 | `OB_PROP_COLOR_HUE_INT`（2014） | int | 色调 |
| 14.4-140 | `OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT`（2015） | int | 电源线频率 |
| 14.4-150 | `OB_PROP_COLOR_HDR_BOOL`（2034） | bool | 色彩 HDR |
| 14.4-160 | `OB_PROP_COLOR_FOCUS_INT`（2038） | int | 焦点 |

### 14.5 IR 控制属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.5-010 | `OB_PROP_IR_MIRROR_BOOL`（18） | bool | IR 镜像 |
| 14.5-020 | `OB_PROP_IR_FLIP_BOOL`（19） | bool | IR 翻转 |
| 14.5-030 | `OB_PROP_IR_AUTO_EXPOSURE_BOOL`（2025） | bool | IR 自动曝光 |
| 14.5-040 | `OB_PROP_IR_EXPOSURE_INT`（2026） | int | IR 曝光值 |
| 14.5-050 | `OB_PROP_IR_GAIN_INT`（2027） | int | IR 增益 |
| 14.5-060 | `OB_PROP_IR_CHANNEL_DATA_SOURCE_INT`（2028） | int | 数据源（0=left, 1=right） |
| 14.5-070 | `OB_PROP_IR_SHORT_EXPOSURE_BOOL`（2032） | bool | 短曝光模式 |
| 14.5-080 | `OB_PROP_IR_LONG_EXPOSURE_BOOL`（2035） | bool | 长曝光模式 |
| 14.5-090 | `OB_PROP_IR_RECTIFY_BOOL`（2040） | bool | IR 校正 |

### 14.6 设备管理属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.6-010 | `OB_PROP_INDICATOR_LIGHT_BOOL`（83） | bool | 指示灯开关 |
| 14.6-020 | `OB_PROP_FAN_WORK_MODE_INT`（62） | int | 风扇工作模式 |
| 14.6-030 | `OB_PROP_WATCHDOG_BOOL`（87） | bool | 看门狗开关 |
| 14.6-040 | `OB_PROP_HEARTBEAT_BOOL`（89） | bool | 心跳监测开关 |
| 14.6-050 | `OB_PROP_DEVICE_WORK_MODE_INT`（95） | int | 设备工作/电源模式 |
| 14.6-060 | `OB_PROP_DEVICE_COMMUNICATION_TYPE_INT`（97） | int | 通信类型（0=USB, 1=Ethernet） |
| 14.6-070 | `OB_PROP_RESTORE_FACTORY_SETTINGS_BOOL`（131） | bool | 恢复出厂设置 |
| 14.6-080 | `OB_PROP_DEVICE_REBOOT_DELAY_INT`（142） | int | 重启延迟（ms） |
| 14.6-090 | `OB_PROP_DEVICE_REPOWER_BOOL`（202） | bool | GMSL 重新上电 |

### 14.7 时序/同步属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.7-010 | `OB_PROP_TIMESTAMP_OFFSET_INT`（43） | int | 时间戳偏移 |
| 14.7-020 | `OB_PROP_TIMER_RESET_SIGNAL_BOOL`（104） | bool | 重置时间为零 |
| 14.7-030 | `OB_PROP_TIMER_RESET_ENABLE_BOOL`（140） | bool | 定时器重置启用 |
| 14.7-040 | `OB_PROP_SYNC_SIGNAL_TRIGGER_OUT_BOOL`（130） | bool | 同步触发信号输出 |
| 14.7-050 | `OB_DEVICE_PTP_CLOCK_SYNC_ENABLE_BOOL`（223） | bool | PTP 时钟同步 |

### 14.8 HDR/帧交错属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.8-010 | `OB_PROP_HDR_MERGE_BOOL`（2037） | bool | HDR 合并开关 |
| 14.8-020 | `OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL`（205） | bool | 帧交错启用 |
| 14.8-030 | `OB_PROP_FRAME_INTERLEAVE_CONFIG_INDEX_INT`（204） | int | 交错配置索引 |

### 14.9 结构体属性

| 编号 | 属性 ID | 描述 |
|------|---------|------|
| 14.9-010 | `OB_STRUCT_DEVICE_TEMPERATURE`（1003） | 设备温度（CPU/IR/激光/TEC） |
| 14.9-020 | `OB_STRUCT_BASELINE_CALIBRATION_PARAM`（1002） | 基线标定参数 |
| 14.9-030 | `OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG`（1038） | 多设备同步配置 |
| 14.9-040 | `OB_STRUCT_DEVICE_SERIAL_NUMBER`（1035） | 设备序列号 |
| 14.9-050 | `OB_STRUCT_DEVICE_TIME`（1037） | 设备时间 |
| 14.9-060 | `OB_STRUCT_DEVICE_IP_ADDR_CONFIG`（1041） | IP 地址配置 |
| 14.9-070 | `OB_STRUCT_RGB_CROP_ROI`（1040） | RGB 裁剪 ROI |
| 14.9-080 | `OB_STRUCT_DEPTH_HDR_CONFIG`（1059） | 深度 HDR 配置 |
| 14.9-090 | `OB_STRUCT_COLOR_AE_ROI`（1060） | 色彩 AE ROI |
| 14.9-100 | `OB_STRUCT_DEPTH_AE_ROI`（1061） | 深度 AE ROI |
| 14.9-110 | `OB_STRUCT_DEPTH_PRECISION_SUPPORT_LIST`（1045） | 深度精度等级列表 |
| 14.9-120 | `OB_STRUCT_PRESET_RESOLUTION_CONFIG`（1069） | 分辨率预设配置 |

### 14.10 SDK 级属性

| 编号 | 属性 ID | 类型 | 描述 |
|------|---------|------|------|
| 14.10-010 | `OB_PROP_SDK_DISPARITY_TO_DEPTH_BOOL`（3004） | bool | 软件视差转深度 |
| 14.10-020 | `OB_PROP_SDK_DEPTH_FRAME_UNPACK_BOOL`（3007） | bool | 深度帧解包 |
| 14.10-030 | `OB_PROP_SDK_IR_FRAME_UNPACK_BOOL`（3008） | bool | IR 帧解包 |
| 14.10-040 | `OB_PROP_SDK_ACCEL_FRAME_TRANSFORMED_BOOL`（3009） | bool | 加速度数据坐标变换 |
| 14.10-050 | `OB_PROP_SDK_GYRO_FRAME_TRANSFORMED_BOOL`（3010） | bool | 陀螺仪数据坐标变换 |

## 十五、深度工作模式

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 15-010 | 获取当前模式 | 获取当前深度工作模式 | `ob_device_get_current_depth_work_mode()` | `getCurrentDepthWorkMode()` |
| 15-020 | 获取当前模式名称 | 获取模式名称字符串 | `ob_device_get_current_depth_work_mode_name()` | `getCurrentDepthWorkModeName()` |
| 15-030 | 枚举模式列表 | 列出设备支持的所有深度模式 | `ob_device_get_depth_work_mode_list()` | `getDepthWorkModeList()` |
| 15-040 | 按名称切换模式 | 切换到指定名称的深度模式 | `ob_device_switch_depth_work_mode_by_name()` | `switchDepthWorkModeByName()` |
| 15-050 | 按 struct 切换模式 | 使用 OBDepthWorkMode 切换 | `ob_device_switch_depth_work_mode()` | `switchDepthWorkMode()` |

## 十六、设备预设（Preset）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 16-010 | 获取当前预设名称 | 读取当前生效的预设 | `ob_device_get_current_preset_name()` | `getCurrentPresetName()` |
| 16-020 | 枚举预设列表 | 列出所有可用预设 | `ob_device_get_available_preset_list()` | `getAvailablePresetList()` |
| 16-030 | 加载内置预设 | 按名称加载预设 | `ob_device_load_preset(dev, name)` | `loadPreset(name)` |
| 16-040 | 从 JSON 文件加载 | 从 JSON 文件导入预设 | `ob_device_load_preset_from_json_file()` | `loadPresetFromJsonFile()` |
| 16-050 | 从 JSON 数据加载 | 从 JSON 缓冲导入 | `ob_device_load_preset_from_json_data()` | `loadPresetFromJsonData()` |
| 16-060 | 导出为 JSON 文件 | 将当前设置导出为 JSON | `ob_device_export_current_settings_as_preset_json_file()` | `exportCurrentSettingsAsPresetJsonFile()` |
| 16-070 | 导出为 JSON 数据 | 将当前设置导出为缓冲 | `ob_device_export_current_settings_as_preset_json_data()` | `exportCurrentSettingsAsPresetJsonData()` |
| 16-080 | 查询预设是否存在 | 按名称检查 | `ob_device_preset_list_has_preset()` | `hasPreset()` |

## 十七、帧交错（Frame Interleave）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 17-010 | 支持查询 | 检查设备是否支持帧交错 | `ob_device_is_frame_interleave_supported()` | `isFrameInterleaveSupported()` |
| 17-020 | 枚举模式列表 | 列出所有可用交错模式 | `ob_device_get_available_frame_interleave_list()` | `getAvailableFrameInterleaveList()` |
| 17-030 | 加载交错模式 | 按名称加载 | `ob_device_load_frame_interleave()` | `loadFrameInterleave()` |
| 17-040 | 查询模式是否存在 | 按名称检查 | `ob_device_frame_interleave_list_has_frame_interleave()` | `hasFrameInterleave()` |

## 十八、录制与回放

### 18.1 录制（Recording）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 18.1-010 | 创建录制设备 | 将设备包装为录制设备 | `ob_create_record_device(dev, path, compression)` | `RecordDevice(device, file, compression)` |
| 18.1-020 | 暂停录制 | 暂停写入 | `ob_record_device_pause()` | `pause()` |
| 18.1-030 | 恢复录制 | 继续写入 | `ob_record_device_resume()` | `resume()` |
| 18.1-040 | 销毁录制设备 | 停止录制并释放资源 | `ob_delete_record_device()` | 析构函数 |

### 18.2 回放（Playback）

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 18.2-010 | 创建回放设备 | 从录制文件创建（返回 Device） | `ob_create_playback_device(path)` | `PlaybackDevice(file)` |
| 18.2-020 | 暂停回放 | 暂停帧输出 | `ob_playback_device_pause()` | `pause()` |
| 18.2-030 | 恢复回放 | 继续帧输出 | `ob_playback_device_resume()` | `resume()` |
| 18.2-040 | 跳转 | 跳到指定时间位置（ms） | `ob_playback_device_seek(ts)` | `seek(ts)` |
| 18.2-050 | 设置回放速率 | 倍速回放 | `ob_playback_device_set_playback_rate(rate)` | `setPlaybackRate(rate)` |
| 18.2-060 | 获取回放状态 | IDLE/PLAYING/PAUSED/STOPPED | `ob_playback_device_get_current_playback_status()` | `getPlaybackStatus()` |
| 18.2-070 | 获取当前位置 | 当前回放位置（ms） | `ob_playback_device_get_position()` | `getPosition()` |
| 18.2-080 | 获取总时长 | 录制文件总时长（ms） | `ob_playback_device_get_duration()` | `getDuration()` |
| 18.2-090 | 状态变化回调 | 回放状态变化通知 | `ob_playback_device_set_playback_status_changed_callback()` | `setPlaybackStatusChangeCallback()` |

## 十九、多设备同步

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 19-010 | 支持模式查询 | 获取设备支持的同步模式位掩码 | `ob_device_get_supported_multi_device_sync_mode_bitmap()` | `getSupportedMultiDeviceSyncModeBitmap()` |
| 19-020 | 设置同步配置 | 配置同步模式和参数 | `ob_device_set_multi_device_sync_config()` | `setMultiDeviceSyncConfig()` |
| 19-030 | 获取同步配置 | 读取当前同步配置 | `ob_device_get_multi_device_sync_config()` | `getMultiDeviceSyncConfig()` |
| 19-040 | 软件触发捕获 | 软件触发模式下手动触发 | `ob_device_trigger_capture()` | `triggerCapture()` |
| 19-050 | 时间戳重置配置 | 设置/获取时间戳重置参数 | `ob_device_set/get_timestamp_reset_config()` | `setTimestampResetConfig()` 等 |
| 19-060 | 时间戳重置 | 重置设备计时器为零 | `ob_device_timestamp_reset()` | `timestampReset()` |
| 19-070 | Host 时间同步 | 设备计时器与主机同步 | `ob_device_timer_sync_with_host()` | `timerSyncWithHost()` |

### 同步模式（9 种）

| 编号 | 模式 | 描述 |
|------|------|------|
| 19-100 | `FREE_RUN` | 自由运行，无同步 |
| 19-110 | `STANDALONE` | 独立模式，仅内部相机间同步 |
| 19-120 | `PRIMARY` | 主设备，通过 VSYNC_OUT 发送触发信号 |
| 19-130 | `SECONDARY` | 从设备，等待 VSYNC_IN 外部触发 |
| 19-140 | `SECONDARY_SYNCED` | 从设备立即启动，在触发信号上同步 |
| 19-150 | `SOFTWARE_TRIGGERING` | 软件触发模式 |
| 19-160 | `HARDWARE_TRIGGERING` | 外部硬件触发模式 |
| 19-170 | `IR_IMU_SYNC` | IR 与 IMU 同步 |
| 19-180 | `SOFTWARE_SYNCED` | 软件同步 |

### 同步配置字段

| 编号 | 字段 | 描述 |
|------|------|------|
| 19-200 | `syncMode` | 同步模式 |
| 19-210 | `depthDelayUs` | 深度捕获延迟（微秒） |
| 19-220 | `colorDelayUs` | 色彩捕获延迟（微秒） |
| 19-230 | `trigger2ImageDelayUs` | 触发到图像延迟 |
| 19-240 | `triggerOutEnable` | 触发信号输出使能 |
| 19-250 | `triggerOutDelayUs` | 触发输出延迟 |
| 19-260 | `framesPerTrigger` | 每次触发捕获帧数 |

## 二十、坐标变换

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 20-010 | 3D→3D 变换 | 使用外参在两个 3D 坐标系间变换 | `ob_transformation_3d_to_3d()` | 对应方法 |
| 20-020 | 2D+深度→3D | 2D 像素 + 深度值反投影到 3D | `ob_transformation_2d_to_3d()` | 对应方法 |
| 20-030 | 3D→2D | 3D 点投影到 2D 像素 | `ob_transformation_3d_to_2d()` | 对应方法 |
| 20-040 | 2D→2D 跨相机 | 不同相机间的 2D 像素映射 | `ob_transformation_2d_to_2d()` | 对应方法 |
| 20-050 | 保存点云 PLY | 将点云帧保存为 PLY 文件 | `ob_save_pointcloud_to_ply()` | 对应方法 |
| 20-060 | 保存 LiDAR PLY | 保存 LiDAR 点云为 PLY | `ob_save_lidar_pointcloud_to_ply()` | 对应方法 |

## 二十一、固件管理

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 21-010 | 从文件升级固件 | 指定路径升级，支持进度回调 | `ob_device_update_firmware(dev, path, cb, async)` | `updateFirmware()` |
| 21-020 | 从缓冲升级固件 | 从内存数据升级 | `ob_device_update_firmware_from_data()` | `updateFirmwareFromData()` |
| 21-030 | 更新深度预设 | 批量更新可选深度预设 | `ob_device_update_optional_depth_presets()` | `updateOptionalDepthPresets()` |
| 21-040 | 重启设备 | 软重启设备 | `ob_device_reboot()` | `reboot()` |
| 21-050 | 全局时间戳支持 | 查询是否支持全局时间戳 | `ob_device_is_global_timestamp_supported()` | `isGlobalTimestampSupported()` |
| 21-060 | 启用全局时间戳 | 开启全局时间戳 | `ob_device_enable_global_timestamp()` | `enableGlobalTimestamp()` |
| 21-070 | 设备状态查询 | 获取设备状态位掩码 | `ob_device_get_device_state()` | `getDeviceState()` |
| 21-080 | 状态变化回调 | 注册设备状态变化通知 | `ob_device_set_state_changed_callback()` | `setStateChangedCallback()` |
| 21-090 | 心跳监测 | 启用/禁用设备心跳 | `ob_device_enable_heartbeat()` | `enableHeartbeat()` |
| 21-100 | 原始厂商命令 | 发送和接收原始数据 | `ob_device_send_and_receive_data()` | `sendAndReceiveData()` |
| 21-110 | 相机标定参数 | 获取多分辨率标定参数列表 | `ob_device_get_calibration_camera_param_list()` | `getCalibrationCameraParamList()` |

## 二十二、版本查询

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 22-010 | 完整版本号 | 单一整数版本号 | `ob_get_version()` | `Version::getVersion()` |
| 22-020 | 主版本号 | Major 版本 | `ob_get_major_version()` | `Version::getMajor()` |
| 22-030 | 次版本号 | Minor 版本 | `ob_get_minor_version()` | `Version::getMinor()` |
| 22-040 | 补丁版本号 | Patch 版本 | `ob_get_patch_version()` | `Version::getPatch()` |
| 22-050 | 阶段版本 | 预发布标签（alpha/beta/rc） | `ob_get_stage_version()` | `Version::getStageVersion()` |

## 二十三、日志系统

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 23-010 | 设置日志级别 | 全局日志过滤级别 | `ob_set_logger_severity(severity)` | 对应方法 |
| 23-020 | 输出到文件 | 日志写入文件 | `ob_set_logger_to_file(severity, dir)` | 对应方法 |
| 23-030 | 设置文件名 | 指定日志文件名 | `ob_set_logger_file_name(filename)` | 对应方法 |
| 23-040 | 输出到控制台 | 日志打印到控制台 | `ob_set_logger_to_console(severity)` | 对应方法 |
| 23-050 | 输出到回调 | 日志通过回调函数传递 | `ob_set_logger_to_callback(severity, cb)` | 对应方法 |
| 23-060 | 外部消息注入 | 向 SDK 日志中注入外部消息 | `ob_log_external_message(...)` | 对应方法 |

### 日志级别

| 编号 | 级别 | 描述 |
|------|------|------|
| 23-100 | `DEBUG` | 调试信息 |
| 23-110 | `INFO` | 一般信息 |
| 23-120 | `WARN` | 警告 |
| 23-130 | `ERROR` | 错误 |
| 23-140 | `FATAL` | 致命错误 |
| 23-150 | `OFF` | 关闭日志 |

## 二十四、错误处理

| 编号 | 功能点 | 描述 | C API | C++ API |
|------|--------|------|-------|---------|
| 24-010 | 获取错误状态码 | 错误码 | `ob_error_get_status()` | 抛出 `ob::Error` 异常 |
| 24-020 | 获取错误消息 | 可读错误描述 | `ob_error_get_message()` | `what()` |
| 24-030 | 获取出错函数 | 出错的 API 函数名 | `ob_error_get_function()` | `getFunction()` |
| 24-040 | 获取异常类型 | 异常分类 | `ob_error_get_exception_type()` | `getExceptionType()` |

### 异常类型

| 编号 | 类型 | 描述 |
|------|------|------|
| 24-100 | `CAMERA_DISCONNECTED` | 设备已断开 |
| 24-110 | `PLATFORM` | 平台/驱动错误 |
| 24-120 | `INVALID_VALUE` | 非法参数值 |
| 24-130 | `WRONG_API_CALL_SEQUENCE` | API 调用顺序错误 |
| 24-140 | `NOT_IMPLEMENTED` | 功能未实现 |
| 24-150 | `IO` | IO 错误 |
| 24-160 | `MEMORY` | 内存错误 |
| 24-170 | `UNSUPPORTED_OPERATION` | 不支持的操作 |
| 24-180 | `ACCESS_DENIED` | 访问被拒绝 |

## 二十五、关键数据结构

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

## 总结

| 统计项 | 数量 |
|--------|------|
| 功能模块 | 25 |
| 编号功能点总数 | 380+ |
| C API 导出函数 | 300+ |
| 设备属性 ID | ~180 |
| 像素格式 | 38 |
| 传感器/流类型 | 13 |
| 帧类型 | 16 |
| 帧元数据字段 | 34 |
| 多设备同步模式 | 9 |
| 内置 Filter 类型 | 16+ |
| 官方示例 | 40+ |

### 编号规则说明

| 规则 | 说明 |
|------|------|
| 格式 | `模块号-序号` 或 `模块号.子节-序号` |
| 模块号 | 01~25，对应二十五大功能模块 |
| 序号间隔 | API 功能点以 10 为间隔（010, 020, 030...），预留扩展 |
| 枚举值起始 | 同模块内枚举/引用值从 100 起编，以 10 为间隔 |
| 扩展示例 | 在 01-020 和 01-030 之间插入新功能 → 编号 01-025 |
