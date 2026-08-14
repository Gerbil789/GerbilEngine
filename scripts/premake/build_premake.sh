#!/bin/bash

set -e  # exit on error (exit if any command returns a non-zero exit code)
set -o pipefail

# Go to repo root (back one level from script)
cd "$(dirname "$0")/.."

# Ensure premake directory exists
if [ ! -d vendor/premake ]; then
    echo "Premake directory missing"
    exit 1
fi

cd vendor/premake

if [ ! -f Bootstrap.sh ]; then
    echo "Bootstrap.sh missing!"
    exit 1
fi

echo "Running Bootstrap"
chmod +x Bootstrap.sh
./Bootstrap.sh

echo "Building bootstrap premake5"
make -C build/bootstrap # -C = Change directory before running make

echo "=== Premake5 built successfully ==="
exit 0