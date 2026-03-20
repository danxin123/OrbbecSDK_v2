#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

echo "[INFO] Configure and build C++ examples (required)"
cmake -S . -B build_examples \
  -DOB_BUILD_EXAMPLES=ON \
  -DOB_BUILD_TESTS=OFF \
  -DOB_BUILD_TOOLS=OFF \
  -DOB_BUILD_DOCS=OFF \
  -DOB_BUILD_PCL_EXAMPLES=OFF \
  -DOB_BUILD_OPEN3D_EXAMPLES=OFF
cmake --build build_examples -j

echo "[INFO] Configure and build optional PCL/Open3D examples (non-blocking)"
set +e
cmake -S . -B build_examples_optional \
  -DOB_BUILD_EXAMPLES=ON \
  -DOB_BUILD_TESTS=OFF \
  -DOB_BUILD_TOOLS=OFF \
  -DOB_BUILD_DOCS=OFF \
  -DOB_BUILD_PCL_EXAMPLES=ON \
  -DOB_BUILD_OPEN3D_EXAMPLES=ON
cmake --build build_examples_optional -j
opt_rc=$?
set -e

if [[ ${opt_rc} -ne 0 ]]; then
  echo "[WARN] Optional PCL/Open3D examples failed, continue"
fi

echo "[SUCCESS] Examples smoke completed"
