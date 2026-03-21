"""Hardware P0 smoke tests for RC gate.

These tests are intentionally minimal and runner-friendly. They provide
P0-tagged JUnit evidence for the RC policy gate when `require_hw_p0=true`.
"""

from __future__ import annotations

import os
import shutil
import subprocess
from pathlib import Path

import pytest


@pytest.mark.hardware
@pytest.mark.p0
def test_TC_HW_P0_001_orbbec_usb_visible() -> None:
    """Verify an Orbbec USB device is visible on Linux runner via lsusb."""
    assert os.name == "posix", "Hardware P0 USB check must run on Linux runner"

    lsusb = shutil.which("lsusb")
    assert lsusb is not None, "lsusb is required on hardware runner"

    proc = subprocess.run([lsusb], capture_output=True, text=True, check=False)
    output = proc.stdout.lower()

    # Common Orbbec vendor id is 2bc5.
    assert "2bc5" in output, f"No Orbbec USB device found in lsusb output: {proc.stdout}"


@pytest.mark.hardware
@pytest.mark.p0
def test_TC_HW_P0_002_device_health_script_passes() -> None:
    """Verify repository health check script passes on hardware runner."""
    repo_root = Path(__file__).resolve().parents[2]
    script = repo_root / "scripts" / "device_health_check.sh"
    assert script.exists(), f"Missing script: {script}"

    proc = subprocess.run(
        ["bash", str(script)],
        cwd=str(repo_root),
        capture_output=True,
        text=True,
        check=False,
    )

    assert proc.returncode == 0, (
        "device_health_check.sh failed\n"
        f"stdout:\n{proc.stdout}\n"
        f"stderr:\n{proc.stderr}"
    )
