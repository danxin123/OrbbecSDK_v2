# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Orbbec SDK v2 is an open-source C/C++ SDK (C++11) for Orbbec RGB-D cameras and LiDAR devices. It provides both C and C++ APIs for device control, frame capture, depth processing, and recording/playback.

## Build Commands

```bash
# Configure (Linux)
cmake -S . -B build -DOB_BUILD_TESTS=ON

# Configure (Windows - from Developer Command Prompt or with MSVC)
cmake -S . -B build -DOB_BUILD_TESTS=ON

# Build
cmake --build build --parallel 4              # Linux
cmake --build build --config Release          # Windows

# Install
cmake --install build --prefix build/install              # Linux
cmake --install build --config Release --prefix build/install  # Windows

# Build only the no-hardware test target
cmake --build build --target ob_nohw_full_test --parallel 4
```

### Key CMake Options

| Option | Default | Purpose |
|--------|---------|---------|
| `OB_BUILD_TESTS` | ON | Build GoogleTest suites |
| `OB_BUILD_EXAMPLES` | ON | Build example programs |
| `OB_BUILD_TOOLS` | ON | Build tools (benchmark, firmware updater) |
| `OB_BUILD_DOCS` | ON | Build Doxygen API docs |
| `OB_BUILD_USB_PAL` | ON | USB/UVC/HID transport |
| `OB_BUILD_NET_PAL` | ON | Ethernet/GVCP/RTSP transport |
| `OB_ENABLE_CLANG_TIDY` | OFF | Static analysis |
| `OB_ENABLE_SANITIZER` | OFF | Runtime sanitizers |

## Testing

Tests use GoogleTest v1.15.2 (fetched automatically via FetchContent).

```bash
# Run all no-hardware tests (safe without a device connected)
./build/bin/ob_nohw_full_test

# Run with JUnit XML output
./build/bin/ob_nohw_full_test --gtest_output=xml:report.xml

# Run a single test by name
./build/bin/ob_nohw_full_test --gtest_filter="TestSuite.test_name"

# Run tests matching a pattern
./build/bin/ob_nohw_full_test --gtest_filter="*frame*"
```

**Important**: The test binary must run from a directory where `extensions/` is a subdirectory (the SDK locates extension libraries relative to CWD). After `cmake --install`, run from the install prefix directory.

### Test naming convention

`test_TC_<MODULE>_<CASE_ID>_<DESCRIPTION>` (e.g., `test_TC_CPP_010_001_frame_basic_integrity`)

### Test categories

- `tests/nohw_full_test/` - No-hardware tests (run in CI on every PR gate)
- `tests/hw_full_test/` - Require a physical device
- `tests/perf_test/` - Performance benchmarks

## Code Style

- **Clang-format**: Google-based style, 160 column limit, 4-space indent. Config in `.clang-format`.
- **Clang-tidy**: Enabled with `-DOB_ENABLE_CLANG_TIDY=ON`. Config in `.clang-tidy`.

## Architecture

### API Layers

- **C++ API** (`include/libobsensor/hpp/`): Entry point `ObSensor.hpp`. Main classes: `Context`, `Device`, `Pipeline`, `Sensor`, `Frame`, `Filter`.
- **C API** (`include/libobsensor/h/`): Entry point `ObSensor.h`. Mirrors C++ API for language bindings.
- **C wrapper impl** (`src/impl/`): Bridges the C API to internal C++ implementation.

### Internal Modules (src/)

The SDK library is composed of internal static libraries linked into a single shared library:

| Module | Namespace | Purpose |
|--------|-----------|---------|
| `shared` | `ob::shared` | Logger (spdlog), exceptions, environment config, XML/JSON utils |
| `core` | `ob::core` | Frame types, stream profiles, frame processing |
| `filter` | `ob::filter` | Image processing filters (public + proprietary via extensions) |
| `platform` | `ob::platform` | Transport abstraction: USB (UVC/HID/vendor) and Ethernet (GigE/RTSP/RTP/mDNS) |
| `device` | `ob::device` | Device implementations, protocol handling, component model |
| `pipeline` | `ob::pipeline` | High-level capture pipeline with auto-configuration |
| `media` | `ob::media` | Recording/playback in ROS .bag format |

### Device Model

Each device family has its own subdirectory under `src/device/` (e.g., `gemini330/`, `femtobolt/`, `femtomega/`, `astra2/`, `lidar/`). Device implementations extend base classes and register via the device manager (`src/device/devicemanager/`). The component model in `src/device/component/` provides reusable building blocks (property servers, stream processors, etc.).

### Extension Libraries

Proprietary functionality (depth processing, firmware update, advanced filters) lives in `extensions/` as prebuilt binaries loaded at runtime via `dylib`. These are platform-specific shared libraries.

### Third-Party Dependencies

All vendored in `3rdparty/`: libusb, libuvc, libjpeg-turbo, libyuv, spdlog, tinyxml2, jsoncpp, live555, rosbag. No external package manager (conan/vcpkg) is used.

## CI

PR gate (`.github/workflows/pr-gate.yml`) builds on Ubuntu 22.04 and Windows 2022, runs `ob_nohw_full_test`, and publishes JUnit reports. Hardware smoke tests and nightly regression run on separate device-connected runners.

## Contributing

API changes require automated tests. If a test can't be added in the same PR, document why and the follow-up plan. See `CONTRIBUTING.md` for templates and the full checklist.
