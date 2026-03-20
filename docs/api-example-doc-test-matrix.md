# API-Example-Doc-Test Mapping Matrix (Initial)

This is the initial baseline for Step 7. It tracks whether key SDK capability areas have API, example, documentation, and automated regression tests.

| Capability | API | Example | Doc | Auto Test | Notes |
|---|---|---|---|---|---|
| Context and device discovery | Yes | Yes | Yes | Partial | Existing tests build and executable checks exist. |
| Pipeline start/stop | Yes | Yes | Yes | Partial | No-hardware executable checks exist. |
| Frame acquisition | Yes | Yes | Yes | Partial | Add assertions on output frame count in tests. |
| Playback from bag | Yes | Yes | Yes | Yes | Added no-hardware playback smoke test with tests/rosbag sample. |
| Recording to bag | Yes | Yes | Yes | No | Needs hardware test and file integrity checks. |
| Metadata access | Yes | Yes | Yes | Yes | Added playback no-hardware assertions for availability, sequence monotonicity, raw buffer consistency, unsupported-field safety, and C API metadata update/readback. |
| Point cloud conversion | Yes | Yes | Yes | Partial | Example exists; no strict regression assertions yet. |
| Post-processing filters | Yes | Yes | Yes | Partial | Needs deterministic parameterized test suite. |
| Depth work mode and preset | Yes | Yes | Yes | No | Add property read/write and rollback tests. |
| Multi-device sync | Yes | Yes | Yes | No | Hardware-dependent, deferred to smoke/nightly. |

## Gap List (P0/P1/P2)

### P0

- Completed: frame validity and timestamp monotonicity assertions in no-hardware playback tests.
- Completed: no-hardware metadata validations on recorded playback files (value/sequence/raw buffer/C API update safety).

### P1

- Add hardware recording integrity test: create bag, replay bag, verify frame count threshold.
- Add deterministic filter result checks for key filters (align, point cloud, format convert).

### P2

- Expand matrix coverage from major capability areas to full API inventory.
- Add script-based matrix generation to avoid manual drift.
