#!/bin/bash

# Get the directory where THIS script lives
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Resolve the absolute path to your engine root directory
ENGINE_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

"$ENGINE_ROOT/vendor/premake/premake5" --file="$ENGINE_ROOT/premake5.lua" gmake