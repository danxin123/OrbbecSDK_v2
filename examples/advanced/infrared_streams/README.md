# Infrared Streams

Auto-detect and display single or dual infrared streams with emitter control and frame saving.

## Features

| Key | Function |
|-----|----------|
| **E** | Toggle IR emitter/laser (observe structured light pattern difference) |
| **S** | Save current IR frame as PNG |
| **ESC** | Exit |

- Auto-detect `OB_SENSOR_IR` / `OB_SENSOR_IR_LEFT` / `OB_SENSOR_IR_RIGHT`
- Stereo devices auto-display left and right IR streams side-by-side
- Print resolution, frame rate, and format of each IR stream on startup

## Key Concepts

- Multi-IR sensor enumeration with `enableVideoStream(sensorType, ...)`
- `OB_PROP_LASER_BOOL` property to control emitter
- IR frame format handling (Y8 / Y16)
- `CVWindow` with `ARRANGE_ONE_ROW` layout

## Run

```bash
./build/bin/ob_infrared_streams        # Linux
.\build\win_x64\bin\ob_infrared_streams.exe  # Windows
```

## Dependencies

- OrbbecSDK v2
- OpenCV (for visualization window and image saving)
