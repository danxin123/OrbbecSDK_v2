# Deployment Progress

This document tracks execution status against the quality implementation deployment plan.

## Status Legend

- DONE: implemented in current repository and pushed
- SKIPPED: intentionally skipped in this execution cycle
- PARTIAL: implemented but acceptance criteria not fully verifiable yet
- TODO: not started in this repository

## Steps

1. PR gate baseline: DONE
2. Python no-hardware split: DONE
3. Playback reference data + regression: DONE
4. Contributor testing process: DONE
5. Docs check workflow: DONE
6. Examples smoke workflow: DONE
7. API/Example/Doc/Test mapping baseline: DONE
8. ROS dockerized build environment: SKIPPED
9. ROS build/launch gate in PR: SKIPPED
10. ROS topic/param baseline compare: SKIPPED
11. Dashboard page skeleton: DONE
12. Dashboard update script: DONE
13. Dashboard auto-update workflow: DONE
14. First self-hosted runner onboarding: DONE
15. Daily HW smoke workflow: PARTIAL
16. Nightly regression workflow: PARTIAL
17. Basic module test implementation batch: PARTIAL
18. Data-link module test batch: PARTIAL
19. Advanced feature module test batch: PARTIAL
20. Property/error/ROS completion batch: PARTIAL (ROS skipped)
21. Release candidate gate workflow: PARTIAL
22. Release checklist and known issues templates: DONE

## Notes

- ROS steps 8-10 are skipped by execution decision for this cycle.
- Step 14 is completed (first self-hosted runner setup confirmed).
- Hardware-dependent acceptance for steps 15-16 still requires stable online devices and scheduled run history.
- Step 21 still needs numeric policy enforcement (P0 threshold and benchmark 3-sigma gate).
