#!/usr/bin/env bash
set -e

BUILD_DIR=build-release

# ===== BUILD
mkdir -p ${BUILD_DIR}
cmake -S . -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Release
cmake --build ${BUILD_DIR}
echo "[+] Build via CMAKE is completed"
# =====

# ===== DEPLOY
cp ${BUILD_DIR}/threfd /usr/local/bin/threfd
cp threfd.service /etc/systemd/system/threfd.service
echo "[+] Deploy for systemd is completed"
echo "[!] Create /etc/thermal-reflex/config.json before launch, example available: config.json.example"
# =====

