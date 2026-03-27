# LiDAR Playback

This sample replays a recorded LiDAR `.bag` file and prints frame information while the data is being played back.
Use it when you want to debug with recorded data instead of a live device.

## When To Use It

- validate a recorded LiDAR dataset
- debug algorithms with repeatable offline input
- pause and resume playback during inspection
- compare live behavior with offline replay behavior

## Prerequisites

- Build the examples from the repository root as described in [../../LiDAR_README.md](../../LiDAR_README.md)
- A valid `.bag` file recorded from a LiDAR workflow

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_lidar_playback
```

```bash
.\build\win_x64\bin\ob_lidar_playback.exe     # Windows
./build/linux_x86_64/bin/ob_lidar_playback    # Linux x86_64
./build/linux_arm64/bin/ob_lidar_playback     # Linux ARM64
./build/macOS/bin/ob_lidar_playback           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `P` | Pause or resume playback |
| `Esc` | Stop playback and exit |

## How To Use It

1. Start the program.
2. Enter the path to a `.bag` file.
3. Watch the terminal output for duration, frame index, timestamp, and frame format.
4. Use `P` to pause or resume.
5. Use `Esc` to exit.

## Notes

- The sample validates the `.bag` suffix before playback starts.
- When playback reaches the end, it automatically restarts from the beginning.

## Related Examples

- [../3.lidar_record/README.md](../3.lidar_record/README.md) - create the dataset that this sample plays back
