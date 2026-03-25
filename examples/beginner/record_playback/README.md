# Record & Playback Example

将设备流录制为 `.bag` 文件并回放的二合一示例。

## 功能

| 模式 | 说明 |
|------|------|
| **Record** | 自动启用设备所有 sensor，录制到 `.bag`。可视化窗口实时展示数据，终端显示 FPS。`S` 暂停/恢复录制，`ESC` 停止并保存。 |
| **Playback** | 加载 `.bag` 文件回放，CVWindow 可视化。播放结束自动 replay。`ESC` 退出。 |

## 涵盖知识点

- `ob::RecordDevice` 录制 API（pause / resume / flush）
- `ob::PlaybackDevice` 回放 API（getDuration / setPlaybackStatusChangeCallback）
- Pipeline callback 模式
- Config enableStream 全传感器枚举
- 多线程 + condition_variable 实现 auto-replay

## 运行
```bash
./ob_record_playback
```
按菜单选择 `1`（录制）或 `2`（回放），`q` 退出。

## 依赖

- OrbbecSDK v2
- OpenCV（用于可视化窗口）
