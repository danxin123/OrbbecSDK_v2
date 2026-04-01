param(
    [Parameter(Mandatory = $true)][string]$Pool,
    [Parameter(Mandatory = $true)][string]$Repo,
    [Parameter(Mandatory = $true)][string]$Token,
    [string]$RunnerName = "",
    [string]$RunnerVersion = "2.325.0",
    [string]$WorkDir = "_work",
    [string]$ConfigPath = "scripts/runner/runner_pools.json"
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($RunnerName)) {
    $RunnerName = "$Pool-$env:COMPUTERNAME-$([DateTimeOffset]::UtcNow.ToUnixTimeSeconds())"
}

$python = "python"
$labelScript = @"
import json
import sys
from pathlib import Path
cfg = json.loads(Path(sys.argv[1]).read_text(encoding='utf-8'))
pool = sys.argv[2]
for item in cfg.get('pools', []):
    if item.get('name') == pool:
        print(','.join(item.get('runs_on', [])))
        break
else:
    raise SystemExit(f'Pool not found: {pool}')
"@

$labels = & $python -c $labelScript $ConfigPath $Pool
if ([string]::IsNullOrWhiteSpace($labels)) {
    throw "Failed to resolve labels for pool: $Pool"
}

$archive = "actions-runner-win-x64-$RunnerVersion.zip"
$url = "https://github.com/actions/runner/releases/download/v$RunnerVersion/$archive"

$runnerDir = Join-Path (Get-Location) ".runner"
New-Item -ItemType Directory -Force -Path $runnerDir | Out-Null
Set-Location $runnerDir

if (!(Test-Path $archive)) {
    Write-Host "Downloading runner: $url"
    Invoke-WebRequest -Uri $url -OutFile $archive
}

if (!(Test-Path "./config.cmd")) {
    Expand-Archive -Path $archive -DestinationPath . -Force
}

& .\config.cmd `
  --url "https://github.com/$Repo" `
  --token "$Token" `
  --name "$RunnerName" `
  --labels "$labels" `
  --work "$WorkDir" `
  --unattended `
  --replace

Write-Host "Runner configured: $RunnerName"
Write-Host "Pool labels: $labels"
Write-Host "Start with: .\\run.cmd"
