#!/bin/bash

pushd .. >/dev/null
./vendor/bin/premake/linux/premake5
popd >/dev/null

read -p "Press [Enter] to continue..."