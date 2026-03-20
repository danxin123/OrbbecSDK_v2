#!/usr/bin/env bash
set -euo pipefail

# Standardized infrastructure health check for Gemini 335 USB runner.
# Exit code:
#   0 -> infrastructure ready
#   1 -> infrastructure not ready

if ! command -v lsusb >/dev/null 2>&1; then
  echo "[HEALTH FAIL] Device infrastructure not ready."
  echo "[HEALTH FAIL] This is an INFRASTRUCTURE failure, not a test failure."
  echo "[HEALTH FAIL] Check device connections, power, and runner configuration."
  exit 1
fi

if ! lsusb | grep -q "2bc5"; then
  echo "[HEALTH FAIL] Device infrastructure not ready."
  echo "[HEALTH FAIL] This is an INFRASTRUCTURE failure, not a test failure."
  echo "[HEALTH FAIL] Check device connections, power, and runner configuration."
  exit 1
fi

echo "[HEALTH OK] Device infrastructure is ready."
exit 0
