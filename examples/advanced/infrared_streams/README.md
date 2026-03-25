# Infrared Streams

自动检测并显示单目或双目红外流，支持 emitter 控制和帧保存。

## 功能

| 按键 | 功能 |
|------|------|
| **E** | 切换 IR emitter/laser 开关（查看有无结构光 pattern 的差异） |
| **S** | 保存当前 IR 帧为 PNG |
| **ESC** | 退出 |

- 自动检测 `OB_SENSOR_IR` / `OB_SENSOR_IR_LEFT` / `OB_SENSOR_IR_RIGHT`
- 双目设备自动并排显示左右 IR 流
- 启动时打印每路 IR 流的分辨率、帧率、格式

## 涵盖知识点

- 多 IR 传感器枚举与 `enableVideoStream(sensorType, ...)`
- `OB_PROP_LASER_BOOL` 属性控制 emitter
- IR 帧格式处理（Y8 / Y16）
- `CVWindow` 的 `ARRANGE_ONE_ROW` 布局

## 运行
```bash
./ob_infrared_streams
```

## 依赖

- OrbbecSDK v2
- OpenCV（用于可视化窗口和保存图片）
