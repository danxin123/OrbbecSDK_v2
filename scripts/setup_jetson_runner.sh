#!/usr/bin/env bash
set -euo pipefail

# One-click installer for Jetson Orin Nano self-hosted runner (Gemini 335 USB)
# Usage:
#   GITHUB_URL="https://github.com/<owner>/<repo>" \
#   RUNNER_TOKEN="<token>" \
#   bash scripts/setup_jetson_runner.sh

if [[ -z "${GITHUB_URL:-}" ]]; then
  echo "[ERROR] GITHUB_URL is required, e.g. https://github.com/xcy2011sky/OrbbecSDK_v2"
  exit 1
fi

if [[ -z "${RUNNER_TOKEN:-}" ]]; then
  echo "[ERROR] RUNNER_TOKEN is required. Get it from GitHub: Settings -> Actions -> Runners -> New self-hosted runner"
  exit 1
fi

RUNNER_VERSION="${RUNNER_VERSION:-2.317.0}"
RUNNER_NAME="${RUNNER_NAME:-$(hostname)-orin-nano}"
RUNNER_LABELS="${RUNNER_LABELS:-self-hosted,linux,sdk,usb,gemini-335}"
RUNNER_DIR="${RUNNER_DIR:-/opt/actions-runner/orbbec-gemini335}"
RUNNER_WORK="${RUNNER_WORK:-_work}"
HEALTH_CRON="${HEALTH_CRON:-*/5 * * * *}"

# If you have sdk_test repo with ci/scripts/device_health_check.sh, point this var to it.
# Example: REPO_DEVICE_HEALTH_SCRIPT=/home/ubuntu/sdk_test/ci/scripts/device_health_check.sh
REPO_DEVICE_HEALTH_SCRIPT="${REPO_DEVICE_HEALTH_SCRIPT:-}"

CURRENT_USER="${SUDO_USER:-$USER}"
ARCH="$(uname -m)"
OS_ID="$(. /etc/os-release && echo "$ID")"
OS_VER="$(. /etc/os-release && echo "$VERSION_ID")"

echo "[INFO] OS=${OS_ID} ${OS_VER}, ARCH=${ARCH}, USER=${CURRENT_USER}"

if [[ "${ARCH}" != "aarch64" ]]; then
  echo "[WARN] Expected aarch64 for Jetson, current arch is ${ARCH}. Continue anyway."
fi

if [[ "${OS_ID}" != "ubuntu" ]]; then
  echo "[WARN] This script is tested for Ubuntu 22.04. Continue anyway."
fi

echo "[INFO] Installing dependencies"
sudo apt-get update
sudo apt-get install -y \
  curl \
  tar \
  git \
  ca-certificates \
  build-essential \
  gcc \
  g++ \
  cmake \
  python3 \
  python3-venv \
  python3-pip \
  usbutils \
  cron

echo "[INFO] Preparing runner directory: ${RUNNER_DIR}"
sudo mkdir -p "${RUNNER_DIR}"
sudo chown -R "${CURRENT_USER}:${CURRENT_USER}" "${RUNNER_DIR}"
cd "${RUNNER_DIR}"

RUNNER_TGZ="actions-runner-linux-arm64-${RUNNER_VERSION}.tar.gz"
RUNNER_URL="https://github.com/actions/runner/releases/download/v${RUNNER_VERSION}/${RUNNER_TGZ}"

if [[ ! -f "${RUNNER_TGZ}" ]]; then
  echo "[INFO] Downloading GitHub Actions runner ${RUNNER_VERSION}"
  curl -fL -o "${RUNNER_TGZ}" "${RUNNER_URL}"
fi

if [[ ! -f "./config.sh" ]]; then
  echo "[INFO] Extracting runner package"
  tar xzf "${RUNNER_TGZ}"
fi

echo "[INFO] Configuring runner"
./config.sh \
  --unattended \
  --replace \
  --url "${GITHUB_URL}" \
  --token "${RUNNER_TOKEN}" \
  --name "${RUNNER_NAME}" \
  --labels "${RUNNER_LABELS}" \
  --work "${RUNNER_WORK}"

echo "[INFO] Installing and starting runner service"
sudo ./svc.sh install "${CURRENT_USER}"
sudo ./svc.sh start

echo "[INFO] Configuring udev rules for Orbbec USB (VID 2bc5)"
UDEV_RULE_FILE="/etc/udev/rules.d/99-orbbec-usb.rules"
if [[ -n "${REPO_DEVICE_HEALTH_SCRIPT}" ]]; then
  REPO_ROOT="$(cd "$(dirname "${REPO_DEVICE_HEALTH_SCRIPT}")/../.." && pwd)"
else
  REPO_ROOT="$(pwd)"
fi

SDK_UDEV_RULE="${REPO_ROOT}/sdk/cpp/misc/udev/99-orbbec-usb.rules"
if [[ -f "${SDK_UDEV_RULE}" ]]; then
  sudo cp "${SDK_UDEV_RULE}" "${UDEV_RULE_FILE}"
else
  cat <<'EOF' | sudo tee "${UDEV_RULE_FILE}" >/dev/null
SUBSYSTEM=="usb", ATTR{idVendor}=="2bc5", MODE="0666", GROUP="plugdev"
EOF
fi

sudo udevadm control --reload-rules
sudo udevadm trigger

if getent group plugdev >/dev/null; then
  sudo usermod -aG plugdev "${CURRENT_USER}" || true
fi

echo "[INFO] Installing health-check wrapper script"
cat <<'EOF' | sudo tee /usr/local/bin/orbbec_device_health_check.sh >/dev/null
#!/usr/bin/env bash
set -euo pipefail

timestamp="$(date -Iseconds)"
if lsusb | grep -q "2bc5"; then
  echo "[${timestamp}] [OK] Orbbec device detected"
else
  echo "[${timestamp}] [WARN] Orbbec device not detected"
fi

if [[ -n "${REPO_DEVICE_HEALTH_SCRIPT:-}" && -x "${REPO_DEVICE_HEALTH_SCRIPT}" ]]; then
  "${REPO_DEVICE_HEALTH_SCRIPT}" || true
fi
EOF
sudo chmod +x /usr/local/bin/orbbec_device_health_check.sh

echo "[INFO] Configuring cron task: ${HEALTH_CRON}"
( crontab -l 2>/dev/null | grep -v 'orbbec_device_health_check.sh' ; echo "${HEALTH_CRON} /usr/local/bin/orbbec_device_health_check.sh >> /var/log/orbbec_device_health.log 2>&1" ) | crontab -

if systemctl is-active --quiet cron; then
  echo "[INFO] cron service is active"
else
  sudo systemctl enable cron
  sudo systemctl start cron
fi

echo "[INFO] Verifying runner service"
sudo ./svc.sh status || true

echo "[INFO] Verifying USB device presence"
if lsusb | grep -q "2bc5"; then
  echo "[SUCCESS] lsusb check passed: found Orbbec device (2bc5)"
else
  echo "[WARN] lsusb check did not find Orbbec device (2bc5). Check cable/power/udev."
fi

echo "[SUCCESS] Setup finished"
echo "[INFO] Validate in GitHub UI: Settings -> Actions -> Runners"
echo "[INFO] Expected labels: ${RUNNER_LABELS}"
echo "[INFO] If group change was applied, re-login may be required for ${CURRENT_USER}."
