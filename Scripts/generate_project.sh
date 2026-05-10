#!/bin/bash

pushd .. >/dev/null
./vendor/premake/premake5 gmake
popd >/dev/null