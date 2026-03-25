# Copyright (c) Orbbec Inc. All Rights Reserved.
# Licensed under the MIT License.

param(
    [ValidateSet("install", "install_all", "remove", "remove_all")]
    [string]$Operation = "install_all"
)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$metadataScript = Join-Path $scriptDir "obsensor_metadata_win10.ps1"

if (-not (Test-Path $metadataScript)) {
    throw "Metadata setup script not found: $metadataScript"
}

Write-Host "[OrbbecSDK] Environment setup started for Windows"
Write-Host "[OrbbecSDK] Applying metadata registration with operation: $Operation"

& powershell -ExecutionPolicy Bypass -File $metadataScript -op $Operation
$exitCode = $LASTEXITCODE

if ($exitCode -ne 0) {
    throw "Windows environment setup failed with exit code $exitCode"
}

Write-Host "[OrbbecSDK] Windows environment setup completed."