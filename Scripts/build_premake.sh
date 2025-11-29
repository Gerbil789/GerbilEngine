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

# 1. Run bootstrap
if [ ! -f bootstrap.sh ]; then
    echo "bootstrap.sh missing!"
    exit 1
fi

echo "Running bootstrap..."
chmod +x bootstrap.sh
./bootstrap.sh

# After bootstrap, premake creates: build/bootstrap/premake5
if [ ! -f build/bootstrap/premake5 ]; then
    echo "Bootstrap FAILED (premake5 missing)"
    exit 1
fi

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
