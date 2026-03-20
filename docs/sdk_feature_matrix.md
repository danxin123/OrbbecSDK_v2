# Orbbec SDK V2 - Feature Matrix

An SDK feature inventory organized by functional modules and feature points.



**Numbering rule**: `module-seq`, where `seq` increments by 10 to reserve expansion space.
Submodules use `module.subsection-seq`.

---

## 1. SDK Initialization and Global Management

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 01-010 | Default initialization | Create an SDK context with default configuration | `ob_create_context()` | `Context()` |
| 01-020 | Config-file initialization | Create context with a specified config file | `ob_create_context_with_config(path)` | `Context(configPath)` |
| 01-030 | Context destruction | Release SDK resources | `ob_delete_context(ctx)` | Destructor |
| 01-040 | Idle memory release | Release frame memory pool | `ob_free_idle_memory(ctx)` | `freeIdleMemory()` |
| 01-050 | UVC backend selection | Select libuvc/v4l2/MSMF backend on Linux | `ob_set_uvc_backend_type(ctx, type)` | `setUvcBackendType(type)` |
| 01-060 | Extension plugin directory | Set Filter plugin loading path | `ob_set_extensions_directory(dir)` | N/A |

## 2. Device Discovery and Enumeration

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 02-010 | USB device enumeration | Query connected USB devices | `ob_query_device_list(ctx)` | `queryDeviceList()` |
| 02-020 | Network device discovery | Enable/disable GVCP auto discovery | `ob_enable_net_device_enumeration(ctx, enable)` | `enableNetDeviceEnumeration(bool)` |
| 02-030 | Direct network connection | Connect to a network device by IP and port | `ob_create_net_device(ctx, addr, port)` | `createNetDevice(addr, port)` |
| 02-040 | Direct network connection (access mode) | Connect with specified access mode | `ob_create_net_device_ex(ctx, addr, port, mode)` | `createNetDevice(addr, port, mode)` |
| 02-050 | Force IP configuration | Configure static IP by MAC address | `ob_force_ip_config(mac, config)` | `forceIp(mac, config)` |
| 02-060 | Hot-plug callback | Register/unregister device change callback | `ob_register_device_changed_callback()` / `ob_unregister_device_changed_callback()` | `registerDeviceChangedCallback()` / `unregisterDeviceChangedCallback()` |
| 02-070 | Clock synchronization | Enable host-device multi-device clock sync | `ob_enable_device_clock_sync(ctx, interval_ms)` | `enableDeviceClockSync(interval)` |

## 3. Device List Query

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 03-010 | Device count | Get number of devices in list | `ob_device_list_get_count()` | `deviceCount()` |
| 03-020 | Get by index | Get device handle by index | `ob_device_list_get_device(list, idx)` | `getDevice(idx)` |
| 03-030 | Get by index + access mode | Open device with specified access mode | `ob_device_list_get_device_ex(list, idx, mode)` | N/A |
| 03-040 | Find by serial number | Locate a specific device by SN | `ob_device_list_get_device_by_serial_number()` | `getDeviceBySN()` |
| 03-050 | Find by UID | Locate device by unique ID | `ob_device_list_get_device_by_uid()` | `getDeviceByUid()` |
| 03-060 | Device name query | Get product name | `ob_device_list_get_device_name()` | N/A |
| 03-070 | PID/VID query | Get product/vendor IDs | `ob_device_list_get_device_pid/vid()` | N/A |
| 03-080 | Connection type query | Get "USB" or "Ethernet" | `ob_device_list_get_device_connection_type()` | N/A |
| 03-090 | Network info query | Get device/host IP, subnet, gateway, MAC | `ob_device_list_get_device_ip_address/subnet_mask/gateway/local_mac/local_ip()` | N/A |

## 4. Device Information

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 04-010 | Device name | Product model name | `ob_device_info_get_name()` | `name()` |
| 04-020 | Serial number | Unique production serial number | `ob_device_info_get_serial_number()` | `serialNumber()` |
| 04-030 | Firmware version | Current firmware version string | `ob_device_info_get_firmware_version()` | `firmwareVersion()` |
| 04-040 | Hardware version | Hardware revision | `ob_device_info_get_hardware_version()` | `hardwareVersion()` |
| 04-050 | PID/VID/UID | Product ID, vendor ID, unique ID | `ob_device_info_get_pid/vid/uid()` | `pid()` / `vid()` / `uid()` |
| 04-060 | Connection type | "USB" or "Ethernet" | `ob_device_info_get_connection_type()` | `connectionType()` |
| 04-070 | IP address info | IP/subnet/gateway for network devices | `ob_device_info_get_ip_address/subnet_mask/gateway()` | `ipAddress()` etc. |
| 04-080 | ASIC name | Main chip model | `ob_device_info_get_asicName()` | `asicName()` |
| 04-090 | Device type | Device type enum value | `ob_device_info_get_device_type()` | `deviceType()` |
| 04-100 | Minimum SDK version | Minimum SDK version required by device | `ob_device_info_get_supported_min_sdk_version()` | `supportedMinSdkVersion()` |
| 04-110 | Extension info | Query/get extension key-value info | `ob_device_is_extension_info_exist()` / `ob_device_get_extension_info()` | See C++ equivalent APIs |

## 5. Device Access Modes

| ID | Mode | Description |
|------|------|------|
| 05-010 | `DEFAULT_ACCESS` | Default access mode |
| 05-020 | `EXCLUSIVE_ACCESS` | Exclusive access; other processes cannot open |
| 05-030 | `SHARED_ACCESS` | Shared access; multi-process open allowed |
| 05-040 | `CONTROL_ONLY_ACCESS` | Control only, no streaming |

## 6. Sensor Enumeration

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 06-010 | Sensor list | Get all sensors on a device | `ob_device_get_sensor_list()` | `getSensorList()` |
| 06-020 | Get sensor by type | Get a specific sensor by type | `ob_device_get_sensor(dev, type)` | `getSensor(type)` |
| 06-030 | Sensor type | Query sensor type enum | `ob_sensor_get_type()` | `type()` |

### Sensor Type List

| ID | Sensor Type | Description |
|------|-----------|------|
| 06-100 | `DEPTH` | Depth sensor |
| 06-110 | `COLOR` | Color camera |
| 06-120 | `IR` | Infrared sensor |
| 06-130 | `IR_LEFT` | Left IR (stereo) |
| 06-140 | `IR_RIGHT` | Right IR (stereo) |
| 06-150 | `ACCEL` | Accelerometer |
| 06-160 | `GYRO` | Gyroscope |
| 06-170 | `CONFIDENCE` | Confidence map |
| 06-180 | `RAW_PHASE` | Raw phase |
| 06-190 | `LIDAR` | LiDAR |
| 06-200 | `COLOR_LEFT` | Left color |
| 06-210 | `COLOR_RIGHT` | Right color |

## 7. Stream Configuration (StreamProfile)

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 07-010 | Query profile list | Get all configurations supported by sensor | `ob_sensor_get_stream_profile_list()` | `getStreamProfileList()` |
| 07-020 | Filter by params | Filter profile by resolution/FPS/format | `ob_stream_profile_list_get_video_stream_profile()` | `getVideoStreamProfile(w,h,fmt,fps)` |
| 07-030 | Video stream properties | Width, height, FPS, format | `ob_video_stream_profile_get_width/height/fps/format()` | `width()` / `height()` / `fps()` / `format()` |
| 07-040 | Accelerometer profile | Range, sample rate | `ob_accel_stream_profile_get_full_scale_range/sample_rate()` | `fullScaleRange()` / `sampleRate()` |
| 07-050 | Gyroscope profile | Range, sample rate | `ob_gyro_stream_profile_get_full_scale_range/sample_rate()` | `fullScaleRange()` / `sampleRate()` |
| 07-060 | Type check/cast | StreamProfile subtype check | N/A | `is<T>()` / `as<T>()` |

### Pixel Format List (38 types)

| ID | Category | Formats |
|------|------|------|
| 07-100 | YUV/RGB | `YUYV`, `YUY2`, `UYVY`, `NV12`, `NV21`, `I420`, `GRAY`, `RGB`, `BGR`, `BGRA`, `RGBA`, `BYR2`, `RW16`, `YV12`, `BA81` |
| 07-110 | Encoded | `MJPG`, `H264`, `H265`/`HEVC` |
| 07-120 | Depth/IR raw | `Y16`, `Y8`, `Y10`, `Y11`, `Y12`, `Y14`, `Z16`, `Y12C4`, `RLE`, `RVL`, `COMPRESSED` |
| 07-130 | IMU | `ACCEL`, `GYRO` |
| 07-140 | Point cloud | `POINT` (XYZ), `RGB_POINT` (XYZRGB) |
| 07-150 | LiDAR | `LIDAR_POINT`, `LIDAR_SPHERE_POINT`, `LIDAR_SCAN`, `LIDAR_CALIBRATION` |

## 8. Pipeline

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 08-010 | Create default pipeline | Auto-bind first available device | `ob_create_pipeline()` | `Pipeline()` |
| 08-020 | Create with device | Bind to specified device | `ob_create_pipeline_with_device(dev)` | `Pipeline(device)` |
| 08-030 | Start with default streams | Start all default streams | `ob_pipeline_start(pipe)` | `start()` |
| 08-040 | Start with config (polling) | Start using Config; fetch via waitForFrames | `ob_pipeline_start_with_config(pipe, config)` | `start(config)` |
| 08-050 | Start with config (callback) | Start using Config; receive frames by callback | `ob_pipeline_start_with_callback(pipe, config, cb)` | `start(config, callback)` |
| 08-060 | Stop streaming | Stop all streams | `ob_pipeline_stop(pipe)` | `stop()` |
| 08-070 | Poll frames | Blocking wait for frameset | `ob_pipeline_wait_for_frameset(pipe, timeout_ms)` | `waitForFrames(timeout)` |
| 08-080 | Switch config at runtime | Switch stream set/resolution while running | `ob_pipeline_switch_config(pipe, config)` | `switchConfig(config)` |
| 08-090 | Get bound device | Get pipeline-associated device | `ob_pipeline_get_device(pipe)` | `getDevice()` |
| 08-100 | Get stream profiles | Get available profiles for a sensor type | `ob_pipeline_get_stream_profile_list(pipe, sensorType)` | `getStreamProfileList(sensorType)` |
| 08-110 | Frame sync toggle | Enable/disable timestamp-based multi-stream sync | `ob_pipeline_enable/disable_frame_sync()` | `enableFrameSync()` / `disableFrameSync()` |
| 08-120 | D2C compatible profiles | Get depth profiles compatible with color profile | `ob_get_d2c_depth_profile_list()` | `getD2CDepthProfileList()` |
| 08-130 | Get camera params | Get intrinsics/extrinsics | `ob_pipeline_get_camera_param()` | `getCameraParam()` |
| 08-140 | Get params by profile | Get camera params for a specific resolution/profile | `ob_pipeline_get_camera_param_with_profile()` | `getCameraParamWithProfile()` |
| 08-150 | Get full calibration params | Get complete multi-sensor calibration | `ob_pipeline_get_calibration_param()` | `getCalibrationParam()` |

## 9. Pipeline Configuration (Config)

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 09-010 | Enable stream by type | Enable stream type with default profile | `ob_config_enable_stream(config, type)` | `enableStream(type)` |
| 09-020 | Enable stream by profile | Enable stream with specified profile | `ob_config_enable_stream_with_stream_profile(config, profile)` | `enableStream(profile)` |
| 09-030 | Enable video stream by params | Enable with width/height/FPS/format | `ob_config_enable_video_stream(config, type, w, h, fps, fmt)` | `enableVideoStream(...)` |
| 09-040 | Enable accelerometer | Specify range and sample rate | `ob_config_enable_accel_stream(config, fullScale, sampleRate)` | `enableAccelStream(...)` |
| 09-050 | Enable gyroscope | Specify range and sample rate | `ob_config_enable_gyro_stream(config, fullScale, sampleRate)` | `enableGyroStream(...)` |
| 09-060 | Enable LiDAR | Specify scan rate and format | `ob_config_enable_lidar_stream(config, scanRate, format)` | `enableLidarStream(...)` |
| 09-070 | Enable all streams | One-click enable all supported streams | `ob_config_enable_all_stream(config)` | `enableAllStream()` |
| 09-080 | Disable specific stream | Disable one stream type | `ob_config_disable_stream(config, type)` | `disableStream(type)` |
| 09-090 | Disable all streams | Disable all streams | `ob_config_disable_all_stream(config)` | `disableAllStream()` |
| 09-100 | Query enabled profiles | Get currently enabled stream list | `ob_config_get_enabled_stream_profile_list()` | `getEnabledStreamProfileList()` |
| 09-110 | D2C align mode | Set depth-to-color alignment mode | `ob_config_set_align_mode(config, mode)` | `setAlignMode(mode)` |
| 09-120 | Depth scaling after align | Whether to scale depth value after alignment | `ob_config_set_depth_scale_after_align_require()` | `setDepthScaleAfterAlignRequire()` |
| 09-130 | Frame aggregate output mode | Set multi-stream output strategy | `ob_config_set_frame_aggregate_output_mode()` | `setFrameAggregateOutputMode()` |

### D2C Alignment Modes

| ID | Mode | Description |
|------|------|------|
| 09-200 | `ALIGN_DISABLE` | No alignment |
| 09-210 | `ALIGN_D2C_HW_MODE` | Hardware depth-to-color alignment |
| 09-220 | `ALIGN_D2C_SW_MODE` | Software depth-to-color alignment |

### Frame Aggregate Output Modes

| ID | Mode | Description |
|------|------|------|
| 09-300 | `ALL_TYPE_FRAME_REQUIRE` | Output only when all streams have frames |
| 09-310 | `COLOR_FRAME_REQUIRE` | Output when color frame is available |
| 09-320 | `ANY_SITUATION` | Output under any condition (default) |
| 09-330 | `DISABLE` | Disable frame aggregation |

## 10. Frame Data Access

### 10.1 Common Frame Properties

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.1-010 | Frame index | Frame sequence index | `ob_frame_get_index()` | `index()` |
| 10.1-020 | Frame format | Pixel/data format | `ob_frame_get_format()` | `format()` |
| 10.1-030 | Frame type | DEPTH / COLOR / IR etc. | `ob_frame_get_type()` | `type()` |
| 10.1-040 | Device timestamp | Device-side timestamp (us) | `ob_frame_get_timestamp_us()` | `timeStampUs()` |
| 10.1-050 | System timestamp | Host-side receive timestamp | `ob_frame_get_system_timestamp_us()` | `systemTimeStampUs()` |
| 10.1-060 | Global timestamp | Cross-device synchronized global timestamp | `ob_frame_get_global_timestamp_us()` | `globalTimeStampUs()` |
| 10.1-070 | Raw data pointer | Get raw frame data | `ob_frame_get_data()` | `data()` |
| 10.1-080 | Data size | Frame buffer size | `ob_frame_get_data_size()` | `dataSize()` |
| 10.1-090 | Associated StreamProfile | Get profile corresponding to frame | `ob_frame_get_stream_profile()` | `getStreamProfile()` |
| 10.1-100 | Source sensor | Get frame source sensor | `ob_frame_get_sensor()` | `getSensor()` |
| 10.1-110 | Source device | Get frame source device | `ob_frame_get_device()` | `getDevice()` |
| 10.1-120 | Reference count | Increase frame reference count | `ob_frame_add_ref()` | managed automatically by shared_ptr |

### 10.2 VideoFrame Properties

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.2-010 | Image width | Pixel width | `ob_video_frame_get_width()` | `width()` |
| 10.2-020 | Image height | Pixel height | `ob_video_frame_get_height()` | `height()` |
| 10.2-030 | Pixel type | Pixel data type | `ob_video_frame_get_pixel_type()` | `pixelType()` |
| 10.2-040 | Pixel bit depth | Valid bits per pixel | `ob_video_frame_get_pixel_available_bit_size()` | `pixelAvailableBitSize()` |

### 10.3 DepthFrame Properties

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.3-010 | Depth value scale | Scale factor from depth value to millimeter | `ob_depth_frame_get_value_scale()` | `getValueScale()` |

### 10.4 PointsFrame Properties

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.4-010 | Coordinate scaling | Point cloud coordinate scale factor | `ob_points_frame_get_coordinate_value_scale()` | `getCoordinateValueScale()` |
| 10.4-020 | Point cloud width/height | Organized point cloud width/height | `ob_point_cloud_frame_get_width/height()` | `width()` / `height()` |

### 10.5 IMU Frames

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.5-010 | Acceleration value | 3-axis acceleration (x, y, z) | `ob_accel_frame_get_value()` | `value()` |
| 10.5-020 | Gyroscope value | 3-axis angular velocity (x, y, z) | `ob_gyro_frame_get_value()` | `value()` |
| 10.5-030 | IMU temperature | Sensor temperature | `ob_accel/gyro_frame_get_temperature()` | `temperature()` |

### 10.6 FrameSet Operations

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 10.6-010 | Frame count | Number of frames in FrameSet | `ob_frameset_get_count()` | `frameCount()` |
| 10.6-020 | Extract depth frame | Extract depth frame from set | `ob_frameset_get_depth_frame()` | `depthFrame()` |
| 10.6-030 | Extract color frame | Extract color frame from set | `ob_frameset_get_color_frame()` | `colorFrame()` |
| 10.6-040 | Extract IR frame | Extract IR frame from set | `ob_frameset_get_ir_frame()` | `irFrame()` |
| 10.6-050 | Extract points frame | Extract point cloud frame from set | `ob_frameset_get_points_frame()` | `pointsFrame()` |
| 10.6-060 | Extract by type | Extract by frame type | `ob_frameset_get_frame(frameset, type)` | `getFrame(type)` |
| 10.6-070 | Extract by index | Extract by index | `ob_frameset_get_frame_by_index(frameset, idx)` | `getFrame(idx)` |
| 10.6-080 | Push frame | Add a frame to FrameSet | `ob_frameset_push_frame()` | `pushFrame()` |

## 11. Frame Metadata

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 11-010 | Check metadata existence | Check whether specific metadata is available | `ob_frame_has_metadata(frame, type)` | `hasMetadata(type)` |
| 11-020 | Read metadata value | Get int64 metadata value | `ob_frame_get_metadata_value(frame, type)` | `getMetadataValue(type)` |
| 11-030 | Raw metadata buffer | Get raw metadata bytes | `ob_frame_get_metadata()` | `getMetadata()` |
| 11-040 | Update metadata | Modify frame metadata | `ob_frame_update_metadata()` | N/A |

### Metadata Field List (34 fields)

| ID | Category | Fields |
|------|------|------|
| 11-100 | Time and frame index | `TIMESTAMP`, `SENSOR_TIMESTAMP`, `FRAME_NUMBER` |
| 11-110 | Exposure and gain | `AUTO_EXPOSURE`, `EXPOSURE`, `GAIN`, `EXPOSURE_PRIORITY` |
| 11-120 | White balance | `AUTO_WHITE_BALANCE`, `WHITE_BALANCE`, `MANUAL_WHITE_BALANCE` |
| 11-130 | Image adjustment | `BRIGHTNESS`, `CONTRAST`, `SATURATION`, `SHARPNESS`, `HUE`, `GAMMA` |
| 11-140 | Compensation | `BACKLIGHT_COMPENSATION`, `LOW_LIGHT_COMPENSATION`, `POWER_LINE_FREQUENCY` |
| 11-150 | Frame rate | `ACTUAL_FRAME_RATE`, `FRAME_RATE` |
| 11-160 | AE ROI | `AE_ROI_LEFT`, `AE_ROI_TOP`, `AE_ROI_RIGHT`, `AE_ROI_BOTTOM` |
| 11-170 | HDR | `HDR_SEQUENCE_NAME`, `HDR_SEQUENCE_SIZE`, `HDR_SEQUENCE_INDEX` |
| 11-180 | Laser | `LASER_POWER`, `LASER_POWER_LEVEL`, `LASER_STATUS` |
| 11-190 | Other | `GPIO_INPUT_DATA`, `DISPARITY_SEARCH_OFFSET`, `DISPARITY_SEARCH_RANGE` |

## 12. Frame Creation (FrameFactory)

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 12-010 | Create empty frame | Create by type, format, size | `ob_create_frame(type, format, size)` | `FrameFactory::createFrame()` |
| 12-020 | Create video frame | Create with width/height/stride | `ob_create_video_frame(type, fmt, w, h, stride)` | `FrameFactory::createVideoFrame()` |
| 12-030 | Clone frame | Create from existing frame copy | `ob_create_frame_from_other_frame(other, copy)` | `FrameFactory::createFrameFromOtherFrame()` |
| 12-040 | Create from profile | Create from StreamProfile | `ob_create_frame_from_stream_profile()` | `FrameFactory::createFrameFromStreamProfile()` |
| 12-050 | Wrap external buffer | Wrap external memory as Frame | `ob_create_frame_from_buffer(...)` | `FrameFactory::createFrameFromBuffer()` |
| 12-060 | Wrap external buffer (video) | Wrap external memory as VideoFrame | `ob_create_video_frame_from_buffer(...)` | `FrameFactory::createVideoFrameFromBuffer()` |
| 12-070 | Create empty FrameSet | Create an empty frame set | `ob_create_frameset()` | `FrameFactory::createFrameSet()` |

## 13. Filters

### 13.1 Common Filter Operations

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 13.1-010 | Create by name | Create filter by string name | `ob_create_filter(name)` | `Filter(name)` / `FilterFactory::createFilter(name)` |
| 13.1-020 | Create private filter | Filter that requires activation key | `ob_create_private_filter(name, key)` | `FilterFactory::createPrivateFilter(name, key)` |
| 13.1-030 | Synchronous process | Input frame and immediately return processed frame | `ob_filter_process(filter, frame)` | `process(frame)` |
| 13.1-040 | Asynchronous callback process | Set callback and push frame | `ob_filter_set_callback()` + `ob_filter_push_frame()` | `setCallback()` + `pushFrame()` |
| 13.1-050 | Enable/disable | Toggle filter processing (pass-through when disabled) | `ob_filter_enable(filter, enable)` | `enable(bool)` |
| 13.1-060 | Query enabled state | Check if enabled | `ob_filter_is_enabled()` | `isEnabled()` |
| 13.1-070 | Reset state | Clear internal filter cache | `ob_filter_reset()` | `reset()` |
| 13.1-080 | Get config schema | Get configurable parameter schema | `ob_filter_get_config_schema()` | `getConfigSchema()` / `getConfigSchemaVec()` |
| 13.1-090 | Read config value | Read filter parameter | `ob_filter_get_config_value(name)` | `getConfigValue(name)` |
| 13.1-100 | Set config value | Modify filter parameter | `ob_filter_set_config_value(name, value)` | `setConfigValue(name, value)` |
| 13.1-110 | Type check/cast | Check filter subtype | N/A | `is<T>()` / `as<T>()` |

### 13.2 Built-in Filter Types

| ID | Filter Name | Usage | Key Config Methods |
|------|-------------|------|-------------|
| 13.2-010 | `PointCloudFilter` | Convert depth frame to point cloud (XYZ / XYZRGB) | `setCreatePointFormat()`, `setCameraParam()`, `setCoordinateDataScaled()`, `setDecimationFactor()` |
| 13.2-020 | `Align` | Align depth and color frames (D2C / C2D) | `setAlignToStreamProfile()`, `setMatchTargetResolution()` |
| 13.2-030 | `FormatConverter` | Pixel format conversion (YUYV to RGB etc.) | `setFormatConvertType()` |
| 13.2-040 | `HDRMerge` | Merge HDR multi-exposure sequence into one frame | N/A |
| 13.2-050 | `SequenceIdFilter` | Filter frames by sequence ID | `selectSequenceId()`, `getSelectSequenceId()`, `getSequenceIdListSize()` |
| 13.2-060 | `DecimationFilter` | Depth map downsampling | `setScaleValue()` |
| 13.2-070 | `ThresholdFilter` | Depth range threshold filter | `setValueRange(min, max)` |
| 13.2-080 | `SpatialAdvancedFilter` | Advanced spatial filter (activation key required) | `setFilterParams(magnitude, alpha, disp_diff, radius)` |
| 13.2-090 | `SpatialFastFilter` | Fast spatial filter | Generic config interface |
| 13.2-100 | `SpatialModerateFilter` | Moderate spatial filter | Generic config interface |
| 13.2-110 | `HoleFillingFilter` | Depth hole filling | Generic config interface |
| 13.2-120 | `NoiseRemovalFilter` | Depth noise removal | Generic config interface |
| 13.2-130 | `TemporalFilter` | Temporal smoothing filter | Generic config interface |
| 13.2-140 | `FalsePositiveFilter` | False depth value removal | Generic config interface |
| 13.2-150 | `DisparityTransform` | Depth to disparity conversion and back | Generic config interface |

### 13.3 Format Conversion Types (22 types)

| ID | Conversion | ID | Conversion | ID | Conversion |
|------|------|------|------|------|------|
| 13.3-010 | `YUYV_TO_RGB` | 13.3-020 | `I420_TO_RGB` | 13.3-030 | `NV21_TO_RGB` |
| 13.3-040 | `NV12_TO_RGB` | 13.3-050 | `MJPG_TO_I420` | 13.3-060 | `RGB_TO_BGR` |
| 13.3-070 | `MJPG_TO_NV21` | 13.3-080 | `MJPG_TO_RGB` | 13.3-090 | `MJPG_TO_BGR` |
| 13.3-100 | `MJPG_TO_BGRA` | 13.3-110 | `UYVY_TO_RGB` | 13.3-120 | `BGR_TO_RGB` |
| 13.3-130 | `MJPG_TO_NV12` | 13.3-140 | `YUYV_TO_BGR` | 13.3-150 | `YUYV_TO_RGBA` |
| 13.3-160 | `YUYV_TO_BGRA` | 13.3-170 | `YUYV_TO_Y16` | 13.3-180 | `YUYV_TO_Y8` |
| 13.3-190 | `RGBA_TO_RGB` | 13.3-200 | `BGRA_TO_BGR` | 13.3-210 | `Y16_TO_RGB` |
| 13.3-220 | `Y8_TO_RGB` | | | | |

## 14. Device Property Control

### 14.1 Property Access Interfaces

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 14.1-010 | Set integer property | Write int value | `ob_device_set_int_property()` | `setIntProperty()` |
| 14.1-020 | Get integer property | Read int value | `ob_device_get_int_property()` | `getIntProperty()` |
| 14.1-030 | Integer property range | Get min/max/step/default | `ob_device_get_int_property_range()` | `getIntPropertyRange()` |
| 14.1-040 | Set float property | Write float value | `ob_device_set_float_property()` | `setFloatProperty()` |
| 14.1-050 | Get float property | Read float value | `ob_device_get_float_property()` | `getFloatProperty()` |
| 14.1-060 | Float property range | Get min/max/step/default | `ob_device_get_float_property_range()` | `getFloatPropertyRange()` |
| 14.1-070 | Set bool property | Write bool value | `ob_device_set_bool_property()` | `setBoolProperty()` |
| 14.1-080 | Get bool property | Read bool value | `ob_device_get_bool_property()` | `getBoolProperty()` |
| 14.1-090 | Structured data read/write | Read/write complex structured data | `ob_device_get/set_structured_data()` | `getStructuredData()` / `setStructuredData()` |
| 14.1-100 | Raw data read | Read raw data via callback | `ob_device_get_raw_data()` | `getRawData()` |
| 14.1-110 | Custom user data | Read/write user custom data on device | `ob_device_write/read_customer_data()` | `writeCustomerData()` / `readCustomerData()` |
| 14.1-120 | Enumerate supported properties | List all properties supported by device | `ob_device_get_supported_property_count/item()` | `getSupportedPropertyCount/Item()` |
| 14.1-130 | Check property support | Check whether a property is supported | `ob_device_is_property_supported()` | `isPropertySupported()` |

### 14.2 Laser/Emitter Control Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.2-010 | `OB_PROP_LDP_BOOL` (2) | bool | LDP (laser safety protection) switch |
| 14.2-020 | `OB_PROP_LASER_BOOL` (3) | bool | Laser on/off |
| 14.2-030 | `OB_PROP_LASER_PULSE_WIDTH_INT` (4) | int | Laser pulse width |
| 14.2-040 | `OB_PROP_LASER_CURRENT_FLOAT` (5) | float | Laser current (mA) |
| 14.2-050 | `OB_PROP_FLOOD_BOOL` (6) | bool | IR floodlight switch |
| 14.2-060 | `OB_PROP_FLOOD_LEVEL_INT` (7) | int | IR floodlight level |
| 14.2-070 | `OB_PROP_LASER_MODE_INT` (79) | int | Laser mode (1=IR Drive, 2=Torch) |
| 14.2-080 | `OB_PROP_LASER_POWER_LEVEL_CONTROL_INT` (99) | int | Laser power level setting |
| 14.2-090 | `OB_PROP_LASER_POWER_ACTUAL_LEVEL_INT` (119) | int | Actual laser power level (read-only) |
| 14.2-100 | `OB_PROP_LASER_OVERCURRENT_PROTECTION_STATUS_BOOL` (148) | bool | Overcurrent protection status |
| 14.2-110 | `OB_PROP_LASER_ALWAYS_ON_BOOL` (174) | bool | Laser always-on |
| 14.2-120 | `OB_PROP_LASER_ON_OFF_PATTERN_INT` (175) | int | Laser interleave mode |
| 14.2-130 | `OB_PROP_LASER_CONTROL_INT` (182) | int | Laser control (0=off, 1=on, 2=auto) |

### 14.3 Depth Control Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.3-010 | `OB_PROP_DEPTH_MIRROR_BOOL` (14) | bool | Depth image mirror |
| 14.3-020 | `OB_PROP_DEPTH_FLIP_BOOL` (15) | bool | Depth image flip |
| 14.3-030 | `OB_PROP_DEPTH_POSTFILTER_BOOL` (16) | bool | Post-processing filter switch |
| 14.3-040 | `OB_PROP_DEPTH_HOLEFILTER_BOOL` (17) | bool | Hole filling switch |
| 14.3-050 | `OB_PROP_MIN_DEPTH_INT` (22) | int | Minimum depth threshold |
| 14.3-060 | `OB_PROP_MAX_DEPTH_INT` (23) | int | Maximum depth threshold |
| 14.3-070 | `OB_PROP_DEPTH_NOISE_REMOVAL_FILTER_BOOL` (24) | bool | Noise removal filter switch |
| 14.3-080 | `OB_PROP_DEPTH_ALIGN_HARDWARE_BOOL` (42) | bool | Hardware D2C alignment switch |
| 14.3-090 | `OB_PROP_DEPTH_PRECISION_LEVEL_INT` (75) | int | Depth precision level |
| 14.3-100 | `OB_PROP_DEPTH_CROPPING_MODE_INT` (90) | int | Cropping mode |
| 14.3-110 | `OB_PROP_DEPTH_UNIT_FLEXIBLE_ADJUSTMENT_FLOAT` (176) | float | Continuous depth unit adjustment |
| 14.3-120 | `OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL` (2016) | bool | Depth auto exposure |
| 14.3-130 | `OB_PROP_DEPTH_EXPOSURE_INT` (2017) | int | Depth exposure value |
| 14.3-140 | `OB_PROP_DEPTH_GAIN_INT` (2018) | int | Depth gain |
| 14.3-150 | `OB_PROP_DISPARITY_TO_DEPTH_BOOL` (85) | bool | Hardware disparity-to-depth |

### 14.4 Color Control Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.4-010 | `OB_PROP_COLOR_MIRROR_BOOL` (81) | bool | Mirror |
| 14.4-020 | `OB_PROP_COLOR_FLIP_BOOL` (82) | bool | Flip |
| 14.4-030 | `OB_PROP_COLOR_AUTO_EXPOSURE_BOOL` (2000) | bool | Auto exposure |
| 14.4-040 | `OB_PROP_COLOR_EXPOSURE_INT` (2001) | int | Exposure value |
| 14.4-050 | `OB_PROP_COLOR_GAIN_INT` (2002) | int | Gain |
| 14.4-060 | `OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL` (2003) | bool | Auto white balance |
| 14.4-070 | `OB_PROP_COLOR_WHITE_BALANCE_INT` (2004) | int | White balance |
| 14.4-080 | `OB_PROP_COLOR_BRIGHTNESS_INT` (2005) | int | Brightness |
| 14.4-090 | `OB_PROP_COLOR_SHARPNESS_INT` (2006) | int | Sharpness |
| 14.4-100 | `OB_PROP_COLOR_SATURATION_INT` (2008) | int | Saturation |
| 14.4-110 | `OB_PROP_COLOR_CONTRAST_INT` (2009) | int | Contrast |
| 14.4-120 | `OB_PROP_COLOR_GAMMA_INT` (2010) | int | Gamma |
| 14.4-130 | `OB_PROP_COLOR_HUE_INT` (2014) | int | Hue |
| 14.4-140 | `OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT` (2015) | int | Power line frequency |
| 14.4-150 | `OB_PROP_COLOR_HDR_BOOL` (2034) | bool | Color HDR |
| 14.4-160 | `OB_PROP_COLOR_FOCUS_INT` (2038) | int | Focus |

### 14.5 IR Control Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.5-010 | `OB_PROP_IR_MIRROR_BOOL` (18) | bool | IR mirror |
| 14.5-020 | `OB_PROP_IR_FLIP_BOOL` (19) | bool | IR flip |
| 14.5-030 | `OB_PROP_IR_AUTO_EXPOSURE_BOOL` (2025) | bool | IR auto exposure |
| 14.5-040 | `OB_PROP_IR_EXPOSURE_INT` (2026) | int | IR exposure value |
| 14.5-050 | `OB_PROP_IR_GAIN_INT` (2027) | int | IR gain |
| 14.5-060 | `OB_PROP_IR_CHANNEL_DATA_SOURCE_INT` (2028) | int | Data source (0=left, 1=right) |
| 14.5-070 | `OB_PROP_IR_SHORT_EXPOSURE_BOOL` (2032) | bool | Short exposure mode |
| 14.5-080 | `OB_PROP_IR_LONG_EXPOSURE_BOOL` (2035) | bool | Long exposure mode |
| 14.5-090 | `OB_PROP_IR_RECTIFY_BOOL` (2040) | bool | IR rectification |

### 14.6 Device Management Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.6-010 | `OB_PROP_INDICATOR_LIGHT_BOOL` (83) | bool | Indicator light switch |
| 14.6-020 | `OB_PROP_FAN_WORK_MODE_INT` (62) | int | Fan working mode |
| 14.6-030 | `OB_PROP_WATCHDOG_BOOL` (87) | bool | Watchdog switch |
| 14.6-040 | `OB_PROP_HEARTBEAT_BOOL` (89) | bool | Heartbeat monitor switch |
| 14.6-050 | `OB_PROP_DEVICE_WORK_MODE_INT` (95) | int | Device work/power mode |
| 14.6-060 | `OB_PROP_DEVICE_COMMUNICATION_TYPE_INT` (97) | int | Communication type (0=USB, 1=Ethernet) |
| 14.6-070 | `OB_PROP_RESTORE_FACTORY_SETTINGS_BOOL` (131) | bool | Restore factory settings |
| 14.6-080 | `OB_PROP_DEVICE_REBOOT_DELAY_INT` (142) | int | Reboot delay (ms) |
| 14.6-090 | `OB_PROP_DEVICE_REPOWER_BOOL` (202) | bool | GMSL repower |

### 14.7 Timing/Synchronization Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.7-010 | `OB_PROP_TIMESTAMP_OFFSET_INT` (43) | int | Timestamp offset |
| 14.7-020 | `OB_PROP_TIMER_RESET_SIGNAL_BOOL` (104) | bool | Reset timer to zero |
| 14.7-030 | `OB_PROP_TIMER_RESET_ENABLE_BOOL` (140) | bool | Enable timer reset |
| 14.7-040 | `OB_PROP_SYNC_SIGNAL_TRIGGER_OUT_BOOL` (130) | bool | Sync trigger signal output |
| 14.7-050 | `OB_DEVICE_PTP_CLOCK_SYNC_ENABLE_BOOL` (223) | bool | PTP clock sync |

### 14.8 HDR/Frame Interleave Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.8-010 | `OB_PROP_HDR_MERGE_BOOL` (2037) | bool | HDR merge switch |
| 14.8-020 | `OB_PROP_FRAME_INTERLEAVE_ENABLE_BOOL` (205) | bool | Enable frame interleave |
| 14.8-030 | `OB_PROP_FRAME_INTERLEAVE_CONFIG_INDEX_INT` (204) | int | Interleave config index |

### 14.9 Structured Properties

| ID | Property ID | Description |
|------|---------|------|
| 14.9-010 | `OB_STRUCT_DEVICE_TEMPERATURE` (1003) | Device temperatures (CPU/IR/Laser/TEC) |
| 14.9-020 | `OB_STRUCT_BASELINE_CALIBRATION_PARAM` (1002) | Baseline calibration parameters |
| 14.9-030 | `OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG` (1038) | Multi-device sync config |
| 14.9-040 | `OB_STRUCT_DEVICE_SERIAL_NUMBER` (1035) | Device serial number |
| 14.9-050 | `OB_STRUCT_DEVICE_TIME` (1037) | Device time |
| 14.9-060 | `OB_STRUCT_DEVICE_IP_ADDR_CONFIG` (1041) | IP address config |
| 14.9-070 | `OB_STRUCT_RGB_CROP_ROI` (1040) | RGB crop ROI |
| 14.9-080 | `OB_STRUCT_DEPTH_HDR_CONFIG` (1059) | Depth HDR config |
| 14.9-090 | `OB_STRUCT_COLOR_AE_ROI` (1060) | Color AE ROI |
| 14.9-100 | `OB_STRUCT_DEPTH_AE_ROI` (1061) | Depth AE ROI |
| 14.9-110 | `OB_STRUCT_DEPTH_PRECISION_SUPPORT_LIST` (1045) | Depth precision level list |
| 14.9-120 | `OB_STRUCT_PRESET_RESOLUTION_CONFIG` (1069) | Resolution preset config |

### 14.10 SDK-level Properties

| ID | Property ID | Type | Description |
|------|---------|------|------|
| 14.10-010 | `OB_PROP_SDK_DISPARITY_TO_DEPTH_BOOL` (3004) | bool | Software disparity-to-depth |
| 14.10-020 | `OB_PROP_SDK_DEPTH_FRAME_UNPACK_BOOL` (3007) | bool | Depth frame unpacking |
| 14.10-030 | `OB_PROP_SDK_IR_FRAME_UNPACK_BOOL` (3008) | bool | IR frame unpacking |
| 14.10-040 | `OB_PROP_SDK_ACCEL_FRAME_TRANSFORMED_BOOL` (3009) | bool | Accelerometer coordinate transform |
| 14.10-050 | `OB_PROP_SDK_GYRO_FRAME_TRANSFORMED_BOOL` (3010) | bool | Gyroscope coordinate transform |

## 15. Depth Work Mode

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 15-010 | Get current mode | Get current depth work mode | `ob_device_get_current_depth_work_mode()` | `getCurrentDepthWorkMode()` |
| 15-020 | Get current mode name | Get mode name string | `ob_device_get_current_depth_work_mode_name()` | `getCurrentDepthWorkModeName()` |
| 15-030 | Enumerate mode list | List all depth modes supported by device | `ob_device_get_depth_work_mode_list()` | `getDepthWorkModeList()` |
| 15-040 | Switch mode by name | Switch to depth mode by name | `ob_device_switch_depth_work_mode_by_name()` | `switchDepthWorkModeByName()` |
| 15-050 | Switch mode by struct | Switch using `OBDepthWorkMode` | `ob_device_switch_depth_work_mode()` | `switchDepthWorkMode()` |

## 16. Device Presets

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 16-010 | Get current preset name | Read currently active preset | `ob_device_get_current_preset_name()` | `getCurrentPresetName()` |
| 16-020 | Enumerate preset list | List all available presets | `ob_device_get_available_preset_list()` | `getAvailablePresetList()` |
| 16-030 | Load built-in preset | Load preset by name | `ob_device_load_preset(dev, name)` | `loadPreset(name)` |
| 16-040 | Load from JSON file | Import preset from JSON file | `ob_device_load_preset_from_json_file()` | `loadPresetFromJsonFile()` |
| 16-050 | Load from JSON data | Import preset from JSON buffer | `ob_device_load_preset_from_json_data()` | `loadPresetFromJsonData()` |
| 16-060 | Export to JSON file | Export current settings to JSON | `ob_device_export_current_settings_as_preset_json_file()` | `exportCurrentSettingsAsPresetJsonFile()` |
| 16-070 | Export to JSON data | Export current settings to buffer | `ob_device_export_current_settings_as_preset_json_data()` | `exportCurrentSettingsAsPresetJsonData()` |
| 16-080 | Check preset existence | Check by name | `ob_device_preset_list_has_preset()` | `hasPreset()` |

## 17. Frame Interleave

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 17-010 | Support query | Check if device supports frame interleave | `ob_device_is_frame_interleave_supported()` | `isFrameInterleaveSupported()` |
| 17-020 | Enumerate mode list | List all available interleave modes | `ob_device_get_available_frame_interleave_list()` | `getAvailableFrameInterleaveList()` |
| 17-030 | Load interleave mode | Load by name | `ob_device_load_frame_interleave()` | `loadFrameInterleave()` |
| 17-040 | Check mode existence | Check by name | `ob_device_frame_interleave_list_has_frame_interleave()` | `hasFrameInterleave()` |

## 18. Recording and Playback

### 18.1 Recording

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 18.1-010 | Create recording device | Wrap device as recording device | `ob_create_record_device(dev, path, compression)` | `RecordDevice(device, file, compression)` |
| 18.1-020 | Pause recording | Pause writing | `ob_record_device_pause()` | `pause()` |
| 18.1-030 | Resume recording | Continue writing | `ob_record_device_resume()` | `resume()` |
| 18.1-040 | Destroy recording device | Stop recording and release resources | `ob_delete_record_device()` | Destructor |

### 18.2 Playback

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 18.2-010 | Create playback device | Create from recording file (returns Device) | `ob_create_playback_device(path)` | `PlaybackDevice(file)` |
| 18.2-020 | Pause playback | Pause frame output | `ob_playback_device_pause()` | `pause()` |
| 18.2-030 | Resume playback | Resume frame output | `ob_playback_device_resume()` | `resume()` |
| 18.2-040 | Seek | Seek to specified timestamp (ms) | `ob_playback_device_seek(ts)` | `seek(ts)` |
| 18.2-050 | Set playback rate | Set speed multiplier | `ob_playback_device_set_playback_rate(rate)` | `setPlaybackRate(rate)` |
| 18.2-060 | Get playback status | IDLE/PLAYING/PAUSED/STOPPED | `ob_playback_device_get_current_playback_status()` | `getPlaybackStatus()` |
| 18.2-070 | Get current position | Current playback position (ms) | `ob_playback_device_get_position()` | `getPosition()` |
| 18.2-080 | Get total duration | Total recording duration (ms) | `ob_playback_device_get_duration()` | `getDuration()` |
| 18.2-090 | Status change callback | Playback state change notification | `ob_playback_device_set_playback_status_changed_callback()` | `setPlaybackStatusChangeCallback()` |

## 19. Multi-Device Synchronization

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 19-010 | Query supported modes | Get sync mode bitmap supported by device | `ob_device_get_supported_multi_device_sync_mode_bitmap()` | `getSupportedMultiDeviceSyncModeBitmap()` |
| 19-020 | Set sync config | Configure sync mode and parameters | `ob_device_set_multi_device_sync_config()` | `setMultiDeviceSyncConfig()` |
| 19-030 | Get sync config | Read current sync configuration | `ob_device_get_multi_device_sync_config()` | `getMultiDeviceSyncConfig()` |
| 19-040 | Software trigger capture | Manually trigger in software trigger mode | `ob_device_trigger_capture()` | `triggerCapture()` |
| 19-050 | Timestamp reset config | Set/get timestamp reset parameters | `ob_device_set/get_timestamp_reset_config()` | `setTimestampResetConfig()` etc. |
| 19-060 | Timestamp reset | Reset device timer to zero | `ob_device_timestamp_reset()` | `timestampReset()` |
| 19-070 | Host time sync | Sync device timer with host | `ob_device_timer_sync_with_host()` | `timerSyncWithHost()` |

### Synchronization Modes (9 types)

| ID | Mode | Description |
|------|------|------|
| 19-100 | `FREE_RUN` | Free-running, no synchronization |
| 19-110 | `STANDALONE` | Standalone mode, sync only among internal cameras |
| 19-120 | `PRIMARY` | Primary device, outputs trigger via VSYNC_OUT |
| 19-130 | `SECONDARY` | Secondary device, waits for external VSYNC_IN trigger |
| 19-140 | `SECONDARY_SYNCED` | Secondary starts immediately, synchronized on trigger |
| 19-150 | `SOFTWARE_TRIGGERING` | Software trigger mode |
| 19-160 | `HARDWARE_TRIGGERING` | External hardware trigger mode |
| 19-170 | `IR_IMU_SYNC` | IR and IMU synchronization |
| 19-180 | `SOFTWARE_SYNCED` | Software synchronized mode |

### Synchronization Config Fields

| ID | Field | Description |
|------|------|------|
| 19-200 | `syncMode` | Synchronization mode |
| 19-210 | `depthDelayUs` | Depth capture delay (us) |
| 19-220 | `colorDelayUs` | Color capture delay (us) |
| 19-230 | `trigger2ImageDelayUs` | Trigger-to-image delay |
| 19-240 | `triggerOutEnable` | Trigger output enable |
| 19-250 | `triggerOutDelayUs` | Trigger output delay |
| 19-260 | `framesPerTrigger` | Frames captured per trigger |

## 20. Coordinate Transformation

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 20-010 | 3D to 3D transform | Transform between two 3D coordinate systems with extrinsics | `ob_transformation_3d_to_3d()` | See C++ equivalent APIs |
| 20-020 | 2D+depth to 3D | Back-project a 2D pixel plus depth value into 3D | `ob_transformation_2d_to_3d()` | See C++ equivalent APIs |
| 20-030 | 3D to 2D | Project a 3D point to a 2D pixel | `ob_transformation_3d_to_2d()` | See C++ equivalent APIs |
| 20-040 | 2D to 2D cross-camera | Map a 2D pixel between different cameras | `ob_transformation_2d_to_2d()` | See C++ equivalent APIs |
| 20-050 | Save point cloud to PLY | Save a point-cloud frame as a PLY file | `ob_save_pointcloud_to_ply()` | See C++ equivalent APIs |
| 20-060 | Save LiDAR PLY | Save a LiDAR point cloud as a PLY file | `ob_save_lidar_pointcloud_to_ply()` | See C++ equivalent APIs |

## 21. Firmware Management

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 21-010 | Upgrade firmware from file | Upgrade from path, supports progress callback | `ob_device_update_firmware(dev, path, cb, async)` | `updateFirmware()` |
| 21-020 | Upgrade firmware from buffer | Upgrade from in-memory data | `ob_device_update_firmware_from_data()` | `updateFirmwareFromData()` |
| 21-030 | Update optional depth presets | Batch update optional depth presets | `ob_device_update_optional_depth_presets()` | `updateOptionalDepthPresets()` |
| 21-040 | Reboot device | Soft reboot device | `ob_device_reboot()` | `reboot()` |
| 21-050 | Global timestamp support | Query if global timestamp is supported | `ob_device_is_global_timestamp_supported()` | `isGlobalTimestampSupported()` |
| 21-060 | Enable global timestamp | Enable global timestamp | `ob_device_enable_global_timestamp()` | `enableGlobalTimestamp()` |
| 21-070 | Device state query | Get device state bitmap | `ob_device_get_device_state()` | `getDeviceState()` |
| 21-080 | State change callback | Register device state change notification | `ob_device_set_state_changed_callback()` | `setStateChangedCallback()` |
| 21-090 | Heartbeat monitoring | Enable/disable device heartbeat | `ob_device_enable_heartbeat()` | `enableHeartbeat()` |
| 21-100 | Raw vendor command | Send and receive raw data | `ob_device_send_and_receive_data()` | `sendAndReceiveData()` |
| 21-110 | Camera calibration params | Get multi-resolution calibration param list | `ob_device_get_calibration_camera_param_list()` | `getCalibrationCameraParamList()` |

## 22. Version Query

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 22-010 | Full version number | Single integer version | `ob_get_version()` | `Version::getVersion()` |
| 22-020 | Major version | Major version | `ob_get_major_version()` | `Version::getMajor()` |
| 22-030 | Minor version | Minor version | `ob_get_minor_version()` | `Version::getMinor()` |
| 22-040 | Patch version | Patch version | `ob_get_patch_version()` | `Version::getPatch()` |
| 22-050 | Stage version | Pre-release label (alpha/beta/rc) | `ob_get_stage_version()` | `Version::getStageVersion()` |

## 23. Logging System

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 23-010 | Set log level | Set global log filtering severity | `ob_set_logger_severity(severity)` | See C++ equivalent APIs |
| 23-020 | Output to file | Write logs to a file | `ob_set_logger_to_file(severity, dir)` | See C++ equivalent APIs |
| 23-030 | Set file name | Specify log file name | `ob_set_logger_file_name(filename)` | See C++ equivalent APIs |
| 23-040 | Output to console | Print logs to console | `ob_set_logger_to_console(severity)` | See C++ equivalent APIs |
| 23-050 | Output to callback | Deliver logs through a callback function | `ob_set_logger_to_callback(severity, cb)` | See C++ equivalent APIs |
| 23-060 | External message injection | Inject external messages into SDK logs | `ob_log_external_message(...)` | See C++ equivalent APIs |

### Log Levels

| ID | Level | Description |
|------|------|------|
| 23-100 | `DEBUG` | Debug information |
| 23-110 | `INFO` | General information |
| 23-120 | `WARN` | Warning |
| 23-130 | `ERROR` | Error |
| 23-140 | `FATAL` | Fatal error |
| 23-150 | `OFF` | Disable logging |

## 24. Error Handling

| ID | Feature | Description | C API | C++ API |
|------|--------|------|-------|---------|
| 24-010 | Get error status code | Error code | `ob_error_get_status()` | Throws `ob::Error` exception |
| 24-020 | Get error message | Human-readable error description | `ob_error_get_message()` | `what()` |
| 24-030 | Get failing function | API function name where error occurred | `ob_error_get_function()` | `getFunction()` |
| 24-040 | Get exception type | Exception category | `ob_error_get_exception_type()` | `getExceptionType()` |

### Exception Types

| ID | Type | Description |
|------|------|------|
| 24-100 | `CAMERA_DISCONNECTED` | Device disconnected |
| 24-110 | `PLATFORM` | Platform/driver error |
| 24-120 | `INVALID_VALUE` | Invalid parameter value |
| 24-130 | `WRONG_API_CALL_SEQUENCE` | Incorrect API call sequence |
| 24-140 | `NOT_IMPLEMENTED` | Function not implemented |
| 24-150 | `IO` | IO error |
| 24-160 | `MEMORY` | Memory error |
| 24-170 | `UNSUPPORTED_OPERATION` | Unsupported operation |
| 24-180 | `ACCESS_DENIED` | Access denied |

## 25. Key Data Structures

| ID | Struct | Description |
|------|--------|------|
| 25-010 | `OBCameraIntrinsic` | Camera intrinsics (fx, fy, cx, cy, width, height) |
| 25-020 | `OBCameraDistortion` | Distortion coefficients (k1-k6, p1, p2 Brown model) |
| 25-030 | `OBExtrinsic` | Extrinsics (3x3 rotation + 3x1 translation) |
| 25-040 | `OBCameraParam` | Depth + color camera intrinsics/extrinsics |
| 25-050 | `OBCalibrationParam` | Full multi-sensor calibration parameters |
| 25-060 | `OBAccelIntrinsic` / `OBGyroIntrinsic` | IMU intrinsics |
| 25-070 | `OBDeviceTemperature` | Device temperature (CPU/IR/Laser/TEC) |
| 25-080 | `OBDepthWorkMode` | Depth work mode (name + checksum) |
| 25-090 | `OBMultiDeviceSyncConfig` | Multi-device synchronization config |
| 25-100 | `OBNetIpConfig` | Network IP config (address/mask/gateway) |
| 25-110 | `OBHdrConfig` | HDR sequence config |
| 25-120 | `OBRegionOfInterest` | AE ROI rectangle |
| 25-130 | `OBPoint3f` / `OBColorPoint` | 3D point / XYZRGB point |
| 25-140 | `OBAccelValue` / `OBGyroValue` | IMU 3-axis float vector |
| 25-150 | `OBIntPropertyRange` / `OBFloatPropertyRange` | Property value range (min/max/step/default/current) |
| 25-160 | `OBFilterConfigSchemaItem` | Filter config item descriptor |

---

## Summary

| Metric | Count |
|--------|------|
| Functional modules | 25 |
| Total numbered feature points | 380+ |
| Exported C API functions | 300+ |
| Device property IDs | ~180 |
| Pixel formats | 38 |
| Sensor/stream types | 13 |
| Frame types | 16 |
| Frame metadata fields | 34 |
| Multi-device sync modes | 9 |
| Built-in filter types | 16+ |
| Official examples | 40+ |

### Numbering Rule Details

| Rule | Description |
|------|------|
| Format | `module-seq` or `module.subsection-seq` |
| Module ID | 01 to 25, corresponding to 25 major functional modules |
| Sequence interval | API feature points increment by 10 (010, 020, 030...) to reserve expansion |
| Enum value start | Enum/reference values start from 100 in the same module, with interval 10 |
| Expansion example | Insert a new feature between 01-020 and 01-030 -> use 01-025 |
