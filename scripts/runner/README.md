# Runner Management

`scripts/runner/runner_pools.json` is the SDK default global runner configuration.

## What This Controls

- Example test runner assignment (`.github/workflows/examples-validation.yml`)
- `hw_full_test` runner assignment (`.github/workflows/daily-hw-smoke.yml`)
- Runner bootstrap labels for one-click registration scripts

## File Layout

- `runner_pools.json`: global runner pool definitions (platform, labels, models, capabilities)
- `generate_hw_full_test_matrix.py`: generate CI matrix for `hw_full_test` from global pools
- `create_github_runner.sh`: one-click Linux runner setup from a pool
- `create_github_runner.ps1`: one-click Windows runner setup from a pool

## runner_pools.json Fields

- `name`: pool identifier
- `platform`: `windows_x64` / `linux_x86_64` / `linux_arm64`
- `runs_on`: GitHub `runs-on` label array
- `device_series` / `device_models`: matching scope
- `capabilities`: feature tags (for matrix filtering)
- `device_count`: minimum available devices on runner
- `priority`: scheduler preference (lower means more preferred)
- `allow_destructive`: whether destructive cases can run on this pool

## One-click Runner Setup

### Linux

```bash
bash scripts/runner/create_github_runner.sh \
  --pool linux-femto \
  --repo <owner/repo> \
  --token <runner_registration_token>
```

### Windows (PowerShell)

```powershell
pwsh -File scripts/runner/create_github_runner.ps1 `
  -Pool windows-femto `
  -Repo <owner/repo> `
  -Token <runner_registration_token>
```

Notes:
- Get registration token from GitHub repo settings or `gh api`.
- Scripts read labels from `runner_pools.json` and register runner with the same label set.

## Generate HW Matrix Locally

```bash
python scripts/runner/generate_hw_full_test_matrix.py \
  --runner-pools scripts/runner/runner_pools.json \
  --platform linux_x86_64 \
  --required-capabilities depth,color \
  --output-matrix reports/hw_discovery/hw_matrix.json
```

## Update Flow

1. Edit `scripts/runner/runner_pools.json`.
2. Re-run examples/hw workflows.
3. Register new self-hosted runners with matching labels using the one-click scripts.
