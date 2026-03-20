# Open Source Orbbec SDK v2.x

[![Release](https://img.shields.io/github/v/release/orbbec/OrbbecSDK_v2?display_name=tag)](https://github.com/orbbec/OrbbecSDK_v2/releases)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)
[![Docs](https://img.shields.io/badge/Docs-GitHub%20Pages-blue.svg)](https://orbbec.github.io/OrbbecSDK_v2/)
[![Issues](https://img.shields.io/github/issues/orbbec/OrbbecSDK_v2)](https://github.com/orbbec/OrbbecSDK_v2/issues)

Orbbec SDK v2 is an open-source, cross-platform SDK for Orbbec RGB-D and LiDAR devices.
It provides high-performance C/C++ APIs and language wrappers for building depth vision applications.



> [!IMPORTANT]
> This is the mainline README for Orbbec SDK v2.
>
> - Migration from SDK v1 to SDK v2: [docs/tutorial/migration_v1_to_v2.md](docs/tutorial/migration_v1_to_v2.md)
> - OpenNI device upgrade to UVC: [docs/tutorial/openni_to_uvc_upgrade.md](docs/tutorial/openni_to_uvc_upgrade.md)

> [!NOTE]
> LiDAR devices are supported in OrbbecSDK v2.6.2 and later. See [LiDAR_README.md](LiDAR_README.md) for details.

## Table of Contents

- [Core Features](#core-features)
- [Supported Devices and Platforms](#supported-devices-and-platforms)
- [SDK Installation](#sdk-installation)
- [Environment Setup](#environment-setup)
- [Quick Start](#quick-start)
- [Documentation](#documentation)
- [Tools](#tools)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)
- [Links](#links)

## Core Features

- Cross-platform SDK for Windows, Linux, macOS, and Android
- C/C++ APIs with wrappers for multiple languages and frameworks
- Rich feature coverage for RGB-D and LiDAR pipelines
- Built-in support for stream alignment, post-processing, recording/playback, firmware management, and multi-device scenarios
- Open-source repository with active issue tracking and release packages

## Supported Devices and Platforms

### Device Support Policy (v1 vs v2)

For the full v1 vs v2 support policy table and support-level definitions, see:

- [docs/tutorial/migration_v1_to_v2.md](docs/tutorial/migration_v1_to_v2.md)

### Supported Devices and Firmware

| Product Series | Product | Minimum Firmware | Recommended Firmware |
|---|---|---|---|
| Gemini 305 | Gemini 305 | 1.0.30 | 1.0.30 |
| Gemini 340 | Gemini 345 | 1.7.04 | 1.9.03 |
| Gemini 340 | Gemini 345Lg | 1.7.04 | 1.9.03 |
| Gemini 435Le | Gemini 435Le | 1.2.4 | 1.3.6 |
| Gemini 330 | Gemini 335Le | 1.5.31 | 1.6.00 |
| Gemini 330 | Gemini 330 | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 330L | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 335 | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 335L | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 336 | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 336L | 1.2.20 | 1.6.00 |
| Gemini 330 | Gemini 335Lg | 1.3.46 | 1.6.00 |
| Gemini 2 | Gemini 2 | 1.4.92 | 1.4.98 |
| Gemini 2 | Gemini 2 L | 1.4.53 | 1.5.2 |
| Gemini 2 | Gemini 215 | 1.0.9 | 1.0.9 |
| Gemini 2 | Gemini 210 | 1.0.9 | 1.0.9 |
| Femto | Femto Bolt | 1.1.2 | 1.1.3 |
| Femto | Femto Mega | 1.3.0 | 1.3.1 |
| Femto | Femto Mega I | 2.0.4 | 2.0.4 |
| Astra | Astra 2 | 2.8.20 | 2.8.20 |
| Astra Mini | Astra mini Pro | 2.0.03 | 2.0.03 |
| Astra Mini | Astra mini S Pro | 2.0.03 | 2.0.03 |
| LiDAR | Pulsar SL450 | 2.2.4.5 | 2.2.4.5 |
| LiDAR | Pulsar ME450 | 1.0.0.6 | 1.0.0.6 |

### Supported Platforms

- Windows 10 or later: x86 and x64
- Linux x64: tested on Ubuntu 20.04, 22.04, 24.04
- Linux ARM64: tested on NVIDIA Jetson AGX Orin, Orin NX, Orin Nano, AGX Xavier, Xavier NX, Jetson Thor
- Android: tested on Android 13, see [OrbbecSDK-Android-Wrapper](https://github.com/orbbec/OrbbecSDK-Android-Wrapper/tree/v2-main)
- macOS: tested on Apple M2, macOS 13.2

## SDK Installation

### Install from Binary Packages

If you do not plan to modify SDK source code, install from pre-compiled packages:

- [OrbbecSDK_v2 Releases](https://github.com/orbbec/OrbbecSDK_v2/releases)

Package formats:

1. Windows x64: `OrbbecSDK_vx.x.x_win64.exe`
2. Linux x86_64: `OrbbecSDK_vx.x.x_amd64.deb`
3. Linux ARM64: `OrbbecSDK_vx.x.x_arm64.deb`

### Install from Source

If you need custom modifications or deep integration, build from source:

- Build guide: [docs/tutorial/building_orbbec_sdk.md](docs/tutorial/building_orbbec_sdk.md)

## Environment Setup

### Windows

Register metadata required by frame synchronization and timestamp correctness:

- [scripts/env_setup/obsensor_metadata_win10.md](scripts/env_setup/obsensor_metadata_win10.md)

### Linux

Install udev rules to access devices without running all apps as root:

```bash
cd scripts/env_setup
sudo chmod +x ./install_udev_rules.sh
sudo ./install_udev_rules.sh
sudo udevadm control --reload && sudo udevadm trigger
```

## Quick Start

Minimal C++ sample:

![QuickStart Example](docs/resource/QuickStart.jpg)

```cpp
// Create a pipeline.
ob::Pipeline pipe;

// Start the pipeline with default config.
pipe.start();

// Create a window for showing the frames, and set the size of the window.
ob_smpl::CVWindow win("QuickStart", 1280, 720, ob_smpl::ARRANGE_ONE_ROW);

while(win.run()) {
    // Wait for frameSet from the pipeline, the default timeout is 1000ms.
    auto frameSet = pipe.waitForFrameset();

    // Push the frames to the window for showing.
    win.pushFramesToView(frameSet);
}

// Stop the Pipeline, no frame data will be generated.
pipe.stop();
```

## Documentation

- Docs portal: [GitHub Pages](https://orbbec.github.io/OrbbecSDK_v2/)
- Installation and development guide: [docs/tutorial/installation_and_development_guide.md](docs/tutorial/installation_and_development_guide.md)
- API user guide: [Orbbec SDK v2 API User Guide](https://orbbec.github.io/docs/OrbbecSDKv2_API_User_Guide/)
- API reference: [Orbbec SDK v2 API Reference](https://orbbec.github.io/docs/OrbbecSDKv2/index.html)
- FAQ: [docs/FAQ.md](docs/FAQ.md)

## Tools

- Orbbec Viewer: [docs/tutorial/orbbecviewer.md](docs/tutorial/orbbecviewer.md)
- Depth Quality Tool: [DepthQualityTool](https://github.com/orbbec/OrbbecTools/releases/tag/DepthQualityTool)

![Orbbec Viewer](docs/resource/OrbbecViewer.jpg)

If firmware file picker does not appear on Linux in Orbbec Viewer, install Zenity:

```bash
sudo apt-get install zenity
```

## Examples

- Examples overview: [examples/README.md](examples/README.md)
- Wrappers overview: [wrappers/README.md](wrappers/README.md)
- LiDAR usage: [LiDAR_README.md](LiDAR_README.md)

## Contributing

Current focus is internal SDK development, but pull requests and suggestions are welcome.

- Contribution guide: [CONTRIBUTING.md](CONTRIBUTING.md)
- Issue tracker: [GitHub Issues](https://github.com/orbbec/OrbbecSDK_v2/issues)

## License

This project is licensed under MIT with additional third-party licenses.
See [LICENSE.txt](LICENSE.txt) for details.

## Links

- [Orbbec SDK v2 Open Source Repository](https://github.com/orbbec/OrbbecSDK_v2)
- [Orbbec SDK v1 Pre-Compiled Repository](https://github.com/orbbec/OrbbecSDK)
- [Orbbec Company Website](https://www.orbbec.com/)
- [Orbbec 3D Club](https://3dclub.orbbec3d.com)
