#!/bin/bash

pushd .. >/dev/null
./vendor/premake/bin/release/premake5 gmake2
popd >/dev/null