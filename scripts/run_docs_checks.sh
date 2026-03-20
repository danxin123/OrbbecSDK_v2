#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

echo "[INFO] Installing markdown-link-check"
npm --silent install --no-save markdown-link-check@^3.12.2

echo "[INFO] Collecting markdown files"
mapfile -t md_files < <(find . -type f -name "*.md" \
  -not -path "./.git/*" \
  -not -path "./3rdparty/*")

if [[ ${#md_files[@]} -eq 0 ]]; then
  echo "[INFO] No markdown files found, skip"
  exit 0
fi

echo "[INFO] Running markdown link checks"
for f in "${md_files[@]}"; do
  echo "[INFO] Checking ${f}"
  npx --yes markdown-link-check \
    --config "scripts/mlc-config.json" \
    --quiet \
    "${f}"
done

echo "[SUCCESS] Markdown checks completed"
