#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$SCRIPT_DIR/.." >/dev/null
./vendor/premake/premake5 gmake
popd >/dev/null