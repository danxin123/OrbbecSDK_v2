# Record & Playback Example

Record device streams to `.bag` file and play them back — a two-in-one example.

## Features

| Mode | Description |
|------|-------------|
| **Record** | Auto-enable all device sensors, record to `.bag` file. Visualization window shows data in real-time, terminal displays FPS. `S`=pause/resume recording, `ESC`=stop and save. |
| **Playback** | Load `.bag` file for playback with CVWindow visualization. Auto-replay when playback ends. `ESC`=exit. |

## Key Concepts

- `ob::RecordDevice` API (pause / resume / flush)
- `ob::PlaybackDevice` API (getDuration / setPlaybackStatusChangeCallback)
- Pipeline callback pattern
- Config enableStream for full sensor enumeration
- Multi-threading + condition_variable for auto-replay

## Run

```bash
./build/bin/ob_record_playback        # Linux
.\build\win_x64\bin\ob_record_playback.exe  # Windows
```

Select `1` for record or `2` for playback, `q` to exit.

## Dependencies

- OrbbecSDK v2
- OpenCV (for visualization window)
