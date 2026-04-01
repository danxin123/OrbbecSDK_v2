#!/usr/bin/env bash
set -euo pipefail

# One-click GitHub self-hosted runner setup from scripts/runner/runner_pools.json
# Usage:
#   bash scripts/runner/create_github_runner.sh \
#     --pool windows-femto \
#     --repo owner/repo \
#     --token <registration-token> \
#     --runner-name my-runner

POOL_NAME=""
REPO=""
TOKEN=""
RUNNER_NAME=""
RUNNER_VERSION="2.325.0"
WORK_DIR="_work"
CONFIG_PATH="scripts/runner/runner_pools.json"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --pool) POOL_NAME="$2"; shift 2 ;;
    --repo) REPO="$2"; shift 2 ;;
    --token) TOKEN="$2"; shift 2 ;;
    --runner-name) RUNNER_NAME="$2"; shift 2 ;;
    --runner-version) RUNNER_VERSION="$2"; shift 2 ;;
    --work-dir) WORK_DIR="$2"; shift 2 ;;
    --config) CONFIG_PATH="$2"; shift 2 ;;
    *) echo "Unknown argument: $1"; exit 1 ;;
  esac
done

if [[ -z "$POOL_NAME" || -z "$REPO" || -z "$TOKEN" ]]; then
  echo "Missing required args: --pool --repo --token"
  exit 1
fi

if [[ -z "$RUNNER_NAME" ]]; then
  RUNNER_NAME="${POOL_NAME}-$(hostname)-$(date +%s)"
fi

LABELS=$(python - <<'PY' "$CONFIG_PATH" "$POOL_NAME"
import json
import sys
from pathlib import Path

config = json.loads(Path(sys.argv[1]).read_text(encoding='utf-8'))
pool_name = sys.argv[2]
for pool in config.get('pools', []):
    if pool.get('name') == pool_name:
        print(','.join(pool.get('runs_on', [])))
        break
else:
    raise SystemExit(f'Pool not found: {pool_name}')
PY
)

if [[ -z "$LABELS" ]]; then
  echo "No labels resolved for pool: $POOL_NAME"
  exit 1
fi

ARCHIVE="actions-runner-linux-x64-${RUNNER_VERSION}.tar.gz"
URL="https://github.com/actions/runner/releases/download/v${RUNNER_VERSION}/${ARCHIVE}"

mkdir -p .runner
cd .runner

if [[ ! -f "$ARCHIVE" ]]; then
  echo "Downloading runner: $URL"
  curl -fL -o "$ARCHIVE" "$URL"
fi

if [[ ! -d "bin" ]]; then
  tar xzf "$ARCHIVE"
fi

./config.sh \
  --url "https://github.com/${REPO}" \
  --token "$TOKEN" \
  --name "$RUNNER_NAME" \
  --labels "$LABELS" \
  --work "$WORK_DIR" \
  --unattended \
  --replace

echo "Runner configured: $RUNNER_NAME"
echo "Pool labels: $LABELS"
echo "Start with: ./run.sh"
