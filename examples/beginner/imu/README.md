# IMU Viewer

实时 ASCII 可视化加速度计和陀螺仪数据。

## 功能

- 终端内实时刷新 IMU 六轴数据（Accel X/Y/Z, Gyro X/Y/Z）
- ASCII 横条图直观展示数值大小和方向
- 简易倾斜角估算（Pitch / Roll）来自加速度分量
- 显示传感器温度和实时帧率
- 不依赖 OpenCV（纯终端）

## 涵盖知识点

- `ob::AccelFrame` / `ob::GyroFrame` 的 `getValue()` 和 `getTemperature()`
- `config->enableAccelStream()` / `enableGyroStream()`
- `OB_FRAME_AGGREGATE_OUTPUT_ALL_TYPE_FRAME_REQUIRE` 同步输出模式
- ANSI 转义码终端刷新

## 运行
```bash
./ob_imu_viewer
```
按 ESC 退出。

## 依赖

- OrbbecSDK v2
- 不需要 OpenCV
