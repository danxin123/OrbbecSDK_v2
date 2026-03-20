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
| Config object API safety | Yes | Yes | Yes | Yes | Added no-hardware Config API checks: enable/disable stream calls, align/depth-scale/aggregate settings, enabled-profile-list query. |
| Version query | Yes | N/A | Yes | Yes | Added C++/C API cross-check for major/minor/patch/stage/full version composition. |
| Logging system | Yes | N/A | Yes | Yes | Added no-hardware logger severity/console/callback/external-message path assertions with callback marker verification. |
| Error handling basics | Yes | N/A | Yes | Yes | Added deterministic invalid playback path exception assertions with message/function/type validation. |
| Data structures validation | Yes | N/A | Yes | Yes | Added no-hardware read/write validation for depth work mode, net IP, HDR, ROI, point/IMU vectors, property ranges, and filter schema item. |
| Point cloud conversion | Yes | Yes | Yes | Partial | Example exists; no strict regression assertions yet. |
| Post-processing filters | Yes | Yes | Yes | Partial | Needs deterministic parameterized test suite. |
| Depth work mode and preset | Yes | Yes | Yes | No | Add property read/write and rollback tests. |
| Multi-device sync | Yes | Yes | Yes | No | Hardware-dependent, deferred to smoke/nightly. |

## Gap List (P0/P1/P2)

### API But No Example

- None in current capability-level baseline.

### Example But No Document

- Needs full inventory sweep to validate across all example directories.

### Document But Command May Be Stale

- README/tutorial command samples still require periodic command-level execution audit in CI.

### P0

- Completed: frame validity and timestamp monotonicity assertions in no-hardware playback tests.
- Completed: no-hardware metadata validations on recorded playback files (value/sequence/raw buffer/C API update safety).

### P1

- Add hardware recording integrity test: create bag, replay bag, verify frame count threshold.
- Add deterministic filter result checks for key filters (align, point cloud, format convert).

### P2

- Expand matrix coverage from major capability areas to full API inventory.
- Add script-based matrix generation to avoid manual drift.

## Next Expansion Plan (Step 7 Full Scale)

1. Parse API symbols from include/libobsensor/h and include/libobsensor/hpp.
2. Build per-feature rows from docs/api and docs/tutorial references.
3. Map example coverage by scanning examples/ and matching keywords.
4. Map test coverage by scanning tests/ target and testcase IDs.
5. Generate prioritized missing-list for API/Example/Doc/Test dimensions.
