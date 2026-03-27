# Quick Start

The simplest OrbbecSDK example - zero external dependencies and no OpenCV required.

## What it does

1. Creates a `Pipeline` with the default device
2. Starts streaming with default configuration
3. Reads the **center pixel depth value** from each depth frame
4. Prints frame info (resolution, depth in mm, timestamp) to the terminal
5. Press **ESC** to exit

## Build & Run

```bash
# Build from the repository root
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_quick_start_cli

# Run
.\build\win_x64\bin\ob_quick_start_cli.exe     # Windows
./build/linux_x86_64/bin/ob_quick_start_cli    # Linux x86_64
./build/linux_arm64/bin/ob_quick_start_cli     # Linux ARM64
./build/macOS/bin/ob_quick_start_cli           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `Esc` | Exit the program |
| `L` | Cycle the SDK log level |

## Expected Output

```
Pipeline started. Press 'ESC' to exit, 'L' to cycle log level.
Streaming depth frames — printing center pixel depth value...
------------------------------------------------------------
Frame #     0 | 640x480 | Center depth: 1234.5 mm | Timestamp: 12345 ms
Frame #    15 | 640x480 | Center depth: 1230.2 mm | Timestamp: 12845 ms
...
```

## Key Concepts

- **`ob::Pipeline`** — High-level API that manages device, streams, and frame synchronization
- **`pipe.waitForFrameset()`** — Blocking call that returns synchronized frames
- **`DepthFrame::getValueScale()`** — Converts raw uint16 pixel values to millimeters
- **`frame->getData()`** — Direct access to frame buffer as `uint16_t*` for depth

## Next Steps

- [depth_viewer](../depth_viewer/) — Visualize depth with OpenCV colormaps
- [rgbd_viewer](../rgbd_viewer/) — Get aligned RGB-D streams
- [enumerate_control](../enumerate_control/) — Discover devices and control properties
