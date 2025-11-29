@echo off
pushd ..\
call vendor\premake\bin\release\premake5.exe vs2022
popd
PAUSE