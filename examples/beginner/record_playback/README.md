# Record & Playback Example

Record device streams to `.bag` file and play them back — a two-in-one example.

## Features

| Mode | Description |
|------|-------------|
| **Record** | Auto-enable all device sensors, record to `.bag` file. Visualization window shows data in real-time, and the window log displays FPS. `S`=pause/resume recording, `ESC`=stop and save. |
| **Playback** | Load `.bag` file for playback with CVWindow visualization. Auto-replay when playback ends. `ESC`=exit. |

## Key Concepts

- `ob::RecordDevice` API (pause / resume / flush)
- `ob::PlaybackDevice` API (getDuration / setPlaybackStatusChangeCallback)
- Pipeline callback pattern
- Config enableStream for full sensor enumeration
- Multi-threading + condition_variable for auto-replay

## Supported Devices

| Device Series | Models |
|---------------|--------|
| Gemini 330 Series | Gemini 330, Gemini 330L, Gemini 335, Gemini 335L, Gemini 335Le, Gemini 336, Gemini 336L, Gemini 335Lg |
| Gemini 305 Series | Gemini 305 |
| Gemini 340 Series | Gemini 345, Gemini 345Lg |
| Gemini 435 Series | Gemini 435Le |
| Gemini 2 Series | Gemini 2, Gemini 2L, Gemini 215, Gemini 210 |
| Femto Series | Femto Bolt, Femto Mega, Femto Mega I |
| Astra Series | Astra 2 |
| Astra Mini Series | Astra Mini Pro, Astra Mini S Pro |

> Refer to the [Supported Devices and Firmware](https://github.com/orbbec/OrbbecSDK_v2?tab=readme-ov-file#supported-devices-and-firmware) section in the main README for more details.

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_record_playback
```

```bash
.\build\win_x64\bin\ob_record_playback.exe     # Windows
./build/linux_x86_64/bin/ob_record_playback    # Linux x86_64
./build/linux_arm64/bin/ob_record_playback     # Linux ARM64
./build/macOS/bin/ob_record_playback           # macOS
```

Select `1` for record or `2` for playback, `q` to exit.

## Dependencies

- OrbbecSDK v2
- OpenCV (for visualization window)
