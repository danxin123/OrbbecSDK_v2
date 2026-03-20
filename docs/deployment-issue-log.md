# Deployment Issue Log

This file tracks blockers found while executing the deployment/implementation document.

## Open Items

1. Plan vs repo structure mismatch
- Problem: The deployment plan references ci/scripts and ci/configs paths, but this repository uses scripts/ and currently has no ci/ directory.
- Impact: Workflows/scripts must be adapted to repository-local paths.
- Suggested follow-up: Decide whether to standardize all repos on ci/ layout or keep per-repo script paths.

2. ROS execution prerequisites unavailable in current repo
- Problem: Steps requiring ROS Docker builds and ROS launch/topic baseline tests cannot be fully executed in this repository alone.
- Impact: Step 8-10 full acceptance cannot be completed here.
- Suggested follow-up: Execute those steps in OrbbecSDK_ROS1 and OrbbecSDK_ROS2 repositories.

3. Hardware runner dependency
- Problem: Daily/Nightly hardware smoke needs self-hosted runner labels and online devices.
- Impact: Workflows are created but cannot be validated locally on hosted Windows environment.
- Suggested follow-up: Validate on self-hosted runners with target labels and attached devices.

4. Benchmark and threshold gate config dependency
- Problem: Step 21 expects centralized threshold config and benchmark analyzer wiring not yet present in this repository.
- Impact: Release gate currently validates build/no-hardware regression, but P0 percentage and 3-sigma benchmark gate are partial.
- Suggested follow-up: Add shared test-suite config and benchmark pipeline integration.

5. ROS steps intentionally skipped by execution decision
- Problem: User requested to skip ROS steps 8-10 for current execution cycle.
- Impact: ROS Docker build, launch baseline and ROS gate acceptance are intentionally deferred.
- Suggested follow-up: Execute steps 8-10 later in OrbbecSDK_ROS1 and OrbbecSDK_ROS2 when ROS scope is resumed.
