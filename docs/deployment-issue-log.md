# Deployment Issue Log

This file tracks blockers found while executing the deployment/implementation document.

## Open Items

1. Plan vs repo structure mismatch
- Problem: The deployment plan references ci/scripts and ci/configs paths, but this repository uses scripts/ and currently has no ci/ directory.
- Impact: Workflows/scripts must be adapted to repository-local paths.
- Suggested follow-up: Decide whether to standardize all repos on ci/ layout or keep per-repo script paths.

2. ROS steps intentionally skipped by execution decision
- Problem: User requested to skip ROS steps 8-10 for current execution cycle.
- Impact: ROS Docker build, launch baseline and ROS gate acceptance are intentionally deferred.
- Suggested follow-up: Execute steps 8-10 later in OrbbecSDK_ROS1 and OrbbecSDK_ROS2 when ROS scope is resumed.

3. Hardware runner dependency
- Problem: Daily/Nightly hardware smoke needs self-hosted runner labels and online devices.
- Impact: Workflows are created but acceptance cannot be fully validated from hosted environment alone.
- Suggested follow-up: Validate scheduled history on self-hosted runner with attached devices.

4. Missing hardware-specific test targets in current repository
- Problem: Historical workflow references (frame_test/total_test/align_test/hdr_test) are not present in current tests tree.
- Impact: CI build failed until target references were aligned to existing targets.
- Suggested follow-up: Add dedicated hardware test targets in this repo, or keep workflow aligned to available executable targets.

5. RC workflow threshold gate partially closed
- Problem: RC workflow originally lacked explicit numeric policy enforcement.
- Current state: scripts/rc_policy_gate.py has been added and integrated into release-candidate.yml with min success-rate gate and optional benchmark 3-sigma checks.
- Current improvement: rc-linux now emits and uploads JUnit XML artifact; rc-summary downloads artifacts before gate evaluation.
- Remaining gap: P0 and benchmark checks are only fully enforceable when RC pipeline emits standardized P0-tagged JUnit and benchmark CSV artifacts from real runs.

7. RC summary correctness (closed)
- Problem: release-candidate summary previously printed static PASS text.
- Current state: summary now uses real needs.<job>.result values.
- Remaining gap: none for summary accuracy.

6. device_health_check.sh path mismatch in installer context
- Problem: Deployment doc references ci/scripts/device_health_check.sh, but this repository has no ci/ directory or that script.
- Impact: Jetson one-click installer cannot wire the exact script by default.
- Suggested follow-up: Installer uses a fallback lsusb-based health check; optionally provide REPO_DEVICE_HEALTH_SCRIPT env var to wire an external health script path.
