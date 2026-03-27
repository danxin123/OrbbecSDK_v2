# Scenario Tests

## Overview

This directory contains application-scenario tests for OrbbecSDK. Unlike unit-style API validation, these tests verify whether the SDK behaves correctly across a complete user-facing workflow.

The current focus is log completeness: whether the SDK emits enough log information across the full device lifecycle to support issue diagnosis and root-cause analysis.

## Current Test Cases

### TC_SCENARIO_01 — Full Lifecycle Log Completeness

This test verifies that SDK log output is present across the full device lifecycle, from SDK startup to device teardown.

#### Purpose

The goal is to ensure that engineers can use SDK logs to understand what happened during a complex real-world workflow, including device discovery, streaming, filter processing, reboot, disconnect, reconnect, and cleanup.

#### Test Flow

The test executes the following phases in order:

1. **Context initialization**
   Creates an `ob::Context` instance after registering a DEBUG-level log callback and checks that log entries are generated during SDK initialization.

2. **Device enumeration**
   Queries the device list, opens the first available device, reads device information, and checks that the enumeration phase produces log output.

3. **Stream start**
   Creates an `ob::Pipeline`, enables depth and color streams, starts streaming, waits for a valid `FrameSet`, and checks that stream start and frame acquisition produce log output.

4. **Filter processing**
   Extracts a depth frame from the acquired `FrameSet` and attempts filter processing using:
   - `DecimationFilter`
   - `PointCloudFilter`

   If filter creation and processing succeed, the test expects filter-related log output to be present.

5. **Stream stop**
   Stops the pipeline, releases the pipeline object, and checks that stream shutdown produces log output.

6. **Device reboot**
   Registers a device-changed callback, triggers `device->reboot()`, and releases stale device handles immediately.

7. **Device offline detection**
   Waits for the `removed` callback signal and treats it as confirmation that the device went offline. This phase is expected to produce log output related to reboot or disconnection.

8. **Device online detection**
   Waits for the `added` callback signal, then re-enumerates devices and reopens the device by serial number. This phase is expected to produce log output related to reconnection and rediscovery.

9. **Context destruction**
   Releases the device, device list, and context objects, then checks that teardown produces log output.

#### Validation Rules

The test uses a thread-safe in-memory log collector to capture SDK logs through `ob::Context::setLoggerToCallback()`.

Validation is performed at two levels:

1. **Per-phase log existence**
    Each lifecycle phase records its start and end log indices. The test verifies that the phase produced new log entries and prints the full log slice for that phase during the test run.

2. **Matched lifecycle rules**
    The test does not stop at checking that logs exist. It also verifies that key operations have matching begin/end or create/destroy log pairs.

The current matched rules include:

- **Device enumeration**
   Checks that the enumeration phase contains device-discovery related keywords such as device list, query results, or device identity fields.

- **Stream start**
   Checks ordered log pairs such as:
   - `Try to start streams!` -> `Start streams done!`
   - `Try to start stream: ... Depth` -> `Stream state changed to STREAMING@Depth`
   - `Try to start stream: ... Color` -> `Stream state changed to STREAMING@Color`
   - `Pipeline start done!`

- **Filter processing**
   Checks balanced and ordered lifecycle logs for the filters exercised by the test:
   - `Filter DecimationFilter created` -> `Filter DecimationFilter destroyed`
   - `Filter PointCloudFilter created` -> `Filter PointCloudFilter destroyed`

   This is used as evidence that filter handling has both pre-processing and post-processing lifecycle visibility.

- **Stream stop**
   Checks ordered log pairs such as:
   - `Try to stop streams!` -> `Stop streams done!`
   - `Try to stop pipeline!` -> `Stop pipeline done!`
   - `Pipeline destroyed!`

- **Reboot / offline / online**
   Checks that the reboot cycle contains removal-side logs before add-side logs, for example:
   - `removed: 1, added: 0`
   - `removed: 0, added: 1`

   This ensures the logs reflect the full disconnect/reconnect lifecycle in the correct order.

- **Context destruction**
   Checks that the teardown phase contains destruction-side keywords such as `destroyed`, `exit`, or other cleanup-related logs, so that initialization and teardown are both represented in the final trace.

At the end of the run, the test prints a summary including:

- Total number of captured log entries
- Log count by severity level
- Log count for each lifecycle phase
- Missing-log markers for any phase that produced zero entries

If a phase is missing logs or a required pair does not match, the test fails with the relevant phase label and reason.

## Build Target

This directory builds the following test executable:

```bash
ob_scenario_test
```

## Requirements

- A physical device must be connected.
- The device must support normal enumeration, streaming, reboot, disconnect, and reconnect behavior.
- The runtime environment must allow the device to come back online after reboot within the expected timeout window.

## Running the Tests

Run all scenario tests:

```bash
./ob_scenario_test
```

Run only the log completeness test:

```bash
./ob_scenario_test --gtest_filter="TC_SCENARIO_LogCompleteness.TC_SCENARIO_01_full_lifecycle_log_completeness"
```

## Notes

- The current scenario test set contains one test case.
- Additional scenario tests can be added later for multi-device workflows, recovery scenarios, long-duration stability, and cross-feature interaction coverage.