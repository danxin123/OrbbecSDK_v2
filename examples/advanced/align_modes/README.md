# Align Modes

This sample compares disabled, software, and hardware depth-to-color alignment in one live workflow.
Use it when you want to understand how D2C alignment behaves on the current device.

## When To Use It

- compare no alignment, software alignment, and hardware alignment
- verify whether the current device supports hardware D2C
- see the practical difference between pipeline-level and filter-level alignment

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

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- OpenCV is required for the preview window

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON -DOpenCV_DIR=/path/to/opencv
cmake --build build --config Release --target ob_align_modes
```

```bash
.\build\win_x64\bin\ob_align_modes.exe     # Windows
./build/linux_x86_64/bin/ob_align_modes    # Linux x86_64
./build/linux_arm64/bin/ob_align_modes     # Linux ARM64
./build/macOS/bin/ob_align_modes           # macOS
```

## Modes

| Mode | Method | Notes |
| --- | --- | --- |
| `DISABLED` | No alignment | Depth and color stay in their native coordinate spaces |
| `SOFTWARE` | `ob::Align(OB_STREAM_COLOR)` | CPU-based and generally available |
| `HARDWARE` | `config->setAlignMode(ALIGN_D2C_HW_MODE)` | Requires compatible device support and matching profiles |

## Controls

| Key | Action |
| --- | --- |
| `T` | Cycle through alignment modes |
| `+` / `-` | Adjust overlay transparency |
| `Esc` | Exit |

## What You Will See

- the selected color and depth profiles printed in the terminal
- whether hardware D2C is supported on the current device
- a live overlay preview that updates as you switch alignment modes

## Notes

- Hardware D2C requires compatible color and depth profiles with matching FPS.
- Switching into hardware alignment restarts the pipeline because the alignment mode is applied at the pipeline configuration level.
