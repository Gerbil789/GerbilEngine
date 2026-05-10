#!/bin/bash

pushd .. >/dev/null
./vendor/premake/bin/release/premake5 gmake
popd >/dev/null