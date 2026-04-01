# Multi Devices Sync GMSL Trigger

This sample configures and starts a PWM trigger source for supported GMSL synchronization workflows on NVIDIA Xavier and Orin platforms.

## When To Use It

- provide a trigger source for multi-device GMSL synchronization
- test PWM trigger control through `/dev/camsync`
- use it together with the multi-device sync example on supported platforms

## Supported Devices

| Device Series | Models |
|---------------|--------|
| Gemini 330 Series | Gemini 335Lg |

## Prerequisites

- Linux on a supported NVIDIA Xavier or Orin platform
- Access to `/dev/camsync`
- Build the examples from the repository root as described in [../../README.md](../../README.md)

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_multi_devices_sync_gmsltrigger
```

```bash
./build/linux_arm64/bin/ob_multi_devices_sync_gmsltrigger
```

## Before Running

You may need to grant access to the trigger device node:

```bash
sudo chmod 777 /dev/camsync
```

## Menu Options

- `0` - set the trigger frequency
- `1` - start the PWM trigger
- `2` - stop the PWM trigger
- `3` - exit

## Typical Workflow

1. Run the sample.
2. Select `0` and enter the trigger frequency.
3. Select `1` to start the trigger.
4. Use `2` to stop the trigger when finished.
5. Select `3` to exit.

## Note

- The sample uses values like `3000` to represent `30 fps`.
