# Enumerate & Control

Browse connected **devices**, their **sensors**, **stream profiles**, and **get/set device properties** — all from the terminal.

## Features

- List all connected Orbbec devices with PID, SN, and connection type
- Drill down into sensors → stream profiles (format, resolution, FPS)
- View and modify device properties (exposure, gain, laser power, etc.)

## No OpenCV Required

This example is purely CLI-based with no external dependencies.

## Usage

```
=== Enumerate & Control ===
  0. Gemini 2 (PID: 0x0670, SN: AY3D41100B3, USB)

Select device index, or 'q' to quit: 0

  1. Enumerate sensors & stream profiles
  2. Get/Set device properties
  b. Back to device selection
  Choice: 2

--- Properties (42) ---
  0. LDP_BOOL [R/W] Bool(0/1)
  1. LASER_BOOL [R/W] Bool(0/1)
  ...

Usage: <index> get | <index> set <value> | '?' to list | 'b' to go back
> 1 get
  LASER_BOOL = 1
> 1 set 0
  Set LASER_BOOL = 0
```

## Key Concepts

- **`ob::Context::queryDeviceList()`** — Discover connected devices
- **`Device::getSensorList()`** → **`Sensor::getStreamProfileList()`** — Browse capabilities
- **`Device::getIntProperty()` / `setIntProperty()`** — Read/write device parameters
- **`OBPropertyItem`** — Property descriptor with name, type, permission, and range
