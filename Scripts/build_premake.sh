#!/bin/bash

set -e  # exit on error
set -o pipefail

# Go to repo root (script is inside /Scripts)
cd "$(dirname "$0")/.."

echo "=== Building Premake (Linux) ==="

# Ensure premake directory exists
if [ ! -d vendor/premake ]; then
    echo "Premake folder missing"
    exit 1
fi

cd vendor/premake

# Install uuid-dev if missing (Debian/Ubuntu) (premake dependency)
if ! dpkg -s uuid-dev >/dev/null 2>&1; then
    echo "Installing uuid-dev (requires sudo)..."
    sudo apt-get update
    sudo apt-get install -y uuid-dev
fi

# 1. Run Bootstrap
if [ ! -f Bootstrap.sh ]; then
    echo "Bootstrap.sh missing!"
    exit 1
fi

echo "Running Bootstrap..."
chmod +x Bootstrap.sh
./Bootstrap.sh

# 2. Build full Premake (gmake2)
echo "Generating makefiles..."
./build/bootstrap/premake5 gmake2

cd build/gmake2

echo "Building Premake..."
make config=release -j$(nproc)

# Check if output exists
if [ ! -f ../bin/release/premake5 ]; then
    echo "Premake build FAILED"
    exit 1
fi

echo "=== Premake5 built successfully ==="
exit 0
