#!/bin/sh

# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

set -eu

SCRIPT_DIR=$(dirname "$0")
SCRIPT_DIR=$(cd "$SCRIPT_DIR" && pwd)
OS_NAME=$(uname -s)

echo "[OrbbecSDK] Environment setup started for ${OS_NAME}"

if [ "$OS_NAME" = "Linux" ]; then
    if [ "$(id -u)" -ne 0 ]; then
        echo "[OrbbecSDK] Re-running with sudo to install udev rules..."
        exec sudo sh "$0" "$@"
    fi

    sh "${SCRIPT_DIR}/install_udev_rules.sh"
    echo "[OrbbecSDK] Linux environment setup completed."
    exit 0
fi

if [ "$OS_NAME" = "Darwin" ]; then
    find "$SCRIPT_DIR" -type f | while IFS= read -r file; do
        case "$file" in
            *.dylib|*/ob_*)
                xattr -d com.apple.quarantine "$file" 2>/dev/null || true
                ;;
        esac
    done

    echo "[OrbbecSDK] Cleared macOS quarantine attributes where applicable."
    echo "[OrbbecSDK] macOS environment setup completed."
    exit 0
fi

echo "[OrbbecSDK] Unsupported platform: ${OS_NAME}"
echo "[OrbbecSDK] On Windows, run scripts/env_setup/setup.ps1 instead."
exit 1

