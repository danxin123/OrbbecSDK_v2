# Firmware Update Tool

A unified firmware upgrade tool that combines single-device upgrade and multi-device batch upgrade.

## Usage

```bash
# List connected devices
ob_firmware_update -l

# Upgrade a single device (auto-select when only one device is connected)
ob_firmware_update -f firmware.bin

# Upgrade a specific device by serial number
ob_firmware_update -f firmware.bin -s CP1234567890

# Batch upgrade all connected devices
ob_firmware_update -f firmware.bin -a
```

## Arguments

| Argument | Description |
|------|------|
| `-h, --help` | Show help information |
| `-l, --list` | List all connected devices |
| `-f, --file <path>` | Firmware file path (.bin / .img) |
| `-s, --serial <sn>` | Specify target device by serial number |
| `-a, --all` | Upgrade all connected devices |

## Features

- **Single-device mode**: Full upgrade workflow with reboot-and-reupdate support (some devices require two write passes)
- **Batch mode** (`-a`): Upgrade all devices sequentially and output a summary report (success/mismatch/failure)
- Auto-detect ANSI escape sequence support and refresh progress bars in place
- Automatically use the libuvc backend on Linux for better stability
- Automatically reboot devices after upgrade completes

## Notes

- GMSL devices (e.g., Gemini335Lg) do not support hot-plug
- Do not disconnect devices during upgrade
- `-a` and `-s` cannot be used together
