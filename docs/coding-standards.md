# OrbbecSDK v2 Coding Standards

This document defines the coding standards for the OrbbecSDK v2 project. All new code and modifications to existing code must follow these conventions.

---

## 1. Formatting

Formatting is enforced by tooling. Do not hand-format code — run the formatter instead.

### C/C++ — clang-format

Configuration: `.clang-format` (Google-based style with project overrides)

Key settings:
- **Indent**: 4 spaces, no tabs
- **Column limit**: 160
- **Braces**: Attached (K&R), `else`/`catch` on new line (`BeforeElse: true`, `BeforeCatch: true`)
- **Pointer alignment**: Right (`int *p`, not `int* p`)
- **Space before parens**: Never (`if(cond)`, not `if (cond)`)
- **Namespace indentation**: None
- **Access modifier offset**: -4 (aligned with class keyword)

```bash
# Format a file
clang-format -i src/core/frame/Frame.cpp

# Check without modifying (CI-friendly)
clang-format --dry-run --Werror src/core/frame/Frame.cpp

# Format all changed files
git diff --name-only --diff-filter=ACMR '*.cpp' '*.hpp' '*.h' | xargs clang-format -i
```

### CMake — cmake-format

Configuration: `.cmake-format.py`

Key settings:
- **Line width**: 120
- **Indent**: 4 spaces, no tabs
- **Line ending**: Unix (LF)
- **Command case**: canonical

```bash
cmake-format -i CMakeLists.txt
```

---

## 2. Naming Conventions

| Element | Style | Example |
|---------|-------|---------|
| Class / Struct | PascalCase | `VideoFrame`, `DeviceMonitor`, `AlgParamManager` |
| Function / Method | camelCase | `getWidth()`, `setDataSize()`, `enableHeartbeat()` |
| Member variable | snake_case + `_` suffix | `dataSize_`, `vendorDataPort_`, `heartbeatEnabled_` |
| Local variable | camelCase | `streamProfileFilter`, `depthWorkModeManager` |
| Constant / constexpr | UPPER_SNAKE_CASE | `INTERFACE_COLOR`, `MAX_RECV_DATA_SIZE` |
| Enum type | PascalCase with `OB` prefix | `OBFrameType`, `OBStreamType` |
| Enum value | UPPER_SNAKE_CASE with `OB_` prefix | `OB_FORMAT_YUYV`, `OB_STREAM_DEPTH` |
| Macro | UPPER_SNAKE_CASE with `OB_` prefix | `OB_PATH_MAX`, `OB_WIDTH_ANY` |
| Namespace | lowercase | `libobsensor`, `ob` |
| Type alias (using) | PascalCase | `FrameBufferReclaimFunc` |
| C API opaque type | snake_case with `ob_` prefix + `_t` | `ob_device_t`, `ob_frame_t` |
| C API function | snake_case with `ob_` prefix | `ob_create_config()`, `ob_delete_device()` |
| File name | PascalCase for classes, snake_case for utilities | `Frame.hpp`, `DeviceMonitor.cpp`, `test_common.hpp` |

### Getter / Setter naming

- Getters: `getProperty()` (not `property()` or `GetProperty()`)
- Setters: `setProperty(value)` (not `property(value)`)
- Boolean queries: `isEnabled()`, `hasMetadata()` (not `getIsEnabled()`)

### No `m_` prefix

Member variables use trailing underscore (`dataSize_`), never `m_dataSize` or `m_data_size`.

---

## 3. File Structure

### Header files

```cpp
// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

/**
 * @file ClassName.hpp
 * @brief One-line description.
 */
#pragma once

// 1. Project headers (relative paths)
#include "Frame.hpp"
#include "logger/Logger.hpp"

// 2. Third-party headers
#include <spdlog/spdlog.h>

// 3. Standard library headers
#include <memory>
#include <string>
#include <vector>

namespace libobsensor {

class ClassName {
public:
    // public interface

protected:
    // protected members

private:
    // private implementation
};

}  // namespace libobsensor
```

### Key rules

- **Always `#pragma once`** — no `#ifndef` include guards.
- **Copyright header** on every file: `// Copyright (c) Orbbec Inc. All Rights Reserved.` + `// Licensed under the MIT License.`
- **Include ordering**: own header first (in .cpp), then project headers, third-party, standard library. Do not reorder with clang-format (`SortIncludes: false`).
- **One class per file** preferred. File name matches class name.

---

## 4. C++ Language Usage

### C++ standard

The project targets **C++11**. Do not use C++14/17/20 features.

### Smart pointers

- Prefer `std::shared_ptr` for objects with shared ownership (devices, frames, stream profiles).
- Use `std::weak_ptr` to break cyclic references (e.g., `StreamProfile` referencing its owning `Sensor`).
- Use `std::unique_ptr` when single ownership is clear.
- Create via `std::make_shared<T>(...)` / `std::make_unique<T>(...)`, not `new`.

### Destructors

- Virtual destructors must be `noexcept`: `virtual ~ClassName() noexcept;`
- Use `override` on all overridden virtual methods.

### Const correctness

- Mark methods `const` when they don't mutate the object.
- Pass large objects by `const &`.
- Return `std::shared_ptr<const T>` for read-only shared access.

### Type casting

- Use C++ casts (`static_cast`, `dynamic_cast`, `reinterpret_cast`), not C-style casts.
- clang-tidy enforces this via `cppcoreguidelines-pro-type-cstyle-cast`.

### Error handling

Use the SDK exception hierarchy (defined in `src/shared/exception/ObException.hpp`):

```
libobsensor_exception (base)
 ├─ recoverable_exception
 │   ├─ invalid_value_exception       — bad parameter
 │   ├─ unsupported_operation_exception — feature not available
 │   ├─ wrong_api_call_sequence_exception
 │   ├─ not_implemented_exception
 │   └─ access_denied_exception
 └─ unrecoverable_exception
     ├─ io_exception                  — I/O error
     ├─ memory_exception              — allocation failure
     ├─ camera_disconnected_exception — device lost
     └─ pal_exception                 — platform layer error
```

Rules:
- Throw the most specific exception type.
- Use `utils::string::to_string() << ...` for building exception messages.
- In the C API layer (`src/impl/`), catch all exceptions and convert to `ob_error*`.

### Logging

Use the `LOG_*` macros from `src/shared/logger/Logger.hpp`:

```cpp
LOG_TRACE("detailed tracing: {}", value);     // debug builds only
LOG_DEBUG("Sensor {} created!", sensorType);   // internal debugging
LOG_INFO("Device connected: {}", deviceName);  // user-facing
LOG_WARN("Cache full, dropping frame");        // warnings
LOG_ERROR("Failed to open device: {}", err);   // errors
LOG_FATAL("Unrecoverable state");              // critical
```

Rules:
- Use `fmt` format syntax (`{}` placeholders), not `printf` style.
- `LOG_TRACE` is stripped in release builds — use for hot paths.
- `LOG_WARN` and above should be actionable, not noisy.

---

## 5. C API Conventions

The public C API (`include/libobsensor/h/`) follows a distinct style:

- All functions prefixed with `ob_`.
- Opaque pointer types: `ob_device_t*`, `ob_frame_t*`, etc.
- Error reporting via out-parameter: `ob_error **error` as the last parameter.
- Every `ob_create_*` has a corresponding `ob_delete_*`.
- No exceptions cross the C API boundary.

```c
ob_error *error = NULL;
ob_device_t *device = ob_create_device(list, 0, &error);
if(error) {
    // handle error
}
// ...
ob_delete_device(device, &error);
```

---

## 6. Testing Conventions

See `CONTRIBUTING.md` for full details. Summary:

- **Naming**: `test_TC_<MODULE>_<CASE_ID>_<DESCRIPTION>` (e.g., `test_TC_CPP_010_001_frame_basic_integrity`)
- **No-hardware tests** go in `tests/nohw_full_test/` — these run in CI on every PR gate.
- **Hardware tests** go in `tests/hw_full_test/` — require a connected device.
- Prefer **shared fixtures/helpers** over duplicated setup per test.
- Return `0` for pass, non-zero for fail (in addition to GoogleTest assertions).

---

## 7. CMake Conventions

- Use `cmake-format` for formatting (config in `.cmake-format.py`).
- New test targets must be wired into `tests/CMakeLists.txt` (auto-discovered via subdirectory).
- Use existing CMake options (defined in `cmake/options.cmake`) rather than adding ad-hoc variables.
- Internal libraries use `ob::` namespace aliases (e.g., `ob::shared`, `ob::core`, `ob::device`).

---

## 8. Static Analysis

### clang-tidy

Configuration: `.clang-tidy`. Covers bugprone, cppcoreguidelines, modernize, performance, and readability checks.

```bash
# Enable during build
cmake -S . -B build -DOB_ENABLE_CLANG_TIDY=ON
cmake --build build

# Run on a single file
clang-tidy -p build src/core/frame/Frame.cpp
```

Header filter: `^(src|include)/.*` — only project headers are checked, third-party is excluded.

### Compiler warnings

All platforms compile with warnings-as-errors:
- **Clang**: `-Weverything` with project-specific suppressions, `-Werror`
- **GCC**: `-Wall -Wextra -Werror` (GCC >= 5.0)
- **MSVC**: `/W4 /WX`

If your code introduces a new warning, **fix the code**, do not suppress the warning unless there is a justified reason documented in `cmake/compiler_flags.cmake`.

---

## Quick Reference Card

```
File header:       // Copyright (c) Orbbec Inc. All Rights Reserved.
                   // Licensed under the MIT License.
Include guard:     #pragma once
Class:             PascalCase                    → VideoStreamProfile
Method:            camelCase                     → getStreamProfile()
Member:            snake_case + _                → stream_profile_
Local:             camelCase                     → streamProfile
Constant:          UPPER_SNAKE_CASE              → MAX_FRAME_SIZE
Enum:              OB_ prefix + UPPER_SNAKE_CASE → OB_STREAM_DEPTH
C function:        ob_ prefix + snake_case       → ob_create_device()
Indent:            4 spaces (no tabs)
Line length:       160 (C++), 120 (CMake)
Pointer:           int *p (right-aligned)
Braces:            Attached, else/catch on new line
Space before ():   Never → if(cond), for(...)
C++ standard:      C++11
```
