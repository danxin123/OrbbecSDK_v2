# Hot Plug

This example demonstrates how to detect device disconnect and reconnect events.
It is useful when your application must respond to users unplugging or reconnecting a camera while the program is running.

## When To Use It

- test device hot-plug behavior
- verify that the SDK callback is triggered on disconnect and reconnect
- build applications that must survive cable unplug and device reboot events

## Prerequisites

- Build the examples from the repository root as described in [../../README.md](../../README.md)
- GMSL devices such as Gemini 335Lg do not support hot plugging

## Build & Run

```bash
cmake -S . -B build -DOB_BUILD_EXAMPLES=ON
cmake --build build --config Release --target ob_hot_plugin
```

```bash
.\build\win_x64\bin\ob_hot_plugin.exe     # Windows
./build/linux_x86_64/bin/ob_hot_plugin    # Linux x86_64
./build/linux_arm64/bin/ob_hot_plugin     # Linux ARM64
./build/macOS/bin/ob_hot_plugin           # macOS
```

## Controls

| Key | Action |
| --- | --- |
| `R` | Reboot the currently connected device to trigger disconnect and reconnect callbacks |
| `Esc` | Exit |

You can also manually unplug and reconnect the device to observe the callback behavior.

## What You Will See

- callback output when devices are removed
- callback output when devices are added again
- device identity information such as UID, PID, serial number, and connection type

## Result

![image](../../../docs/resource/hotplugin.jpg)
