# Align Modes

Compare **Hardware** vs **Software** Depth-to-Color alignment in real-time.

## Modes

| Mode | Method | Notes |
|------|--------|-------|
| **DISABLED** | No alignment | Depth and color in their native coordinate spaces |
| **SOFTWARE** | `ob::Align(OB_STREAM_COLOR)` filter | CPU-based, works on all devices |
| **HARDWARE** | `config->setAlignMode(ALIGN_D2C_HW_MODE)` | Device-level, zero CPU cost, requires device support |

## Controls

| Key | Action |
|-----|--------|
| **T** | Cycle align mode (Disabled → SW → HW → ...) |
| **+/-** | Adjust overlay transparency |
| **Esc** | Exit |

## Key Concepts

- **`pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE)`** — Query HW D2C compatible depth profiles
- **`config->setAlignMode(ALIGN_D2C_HW_MODE)`** — Enable HW alignment at pipeline level (requires restart)
- **`ob::Align(OB_STREAM_COLOR)`** — Software filter applied per-frame, no pipeline restart needed
- HW D2C requires matching FPS between color and depth streams
