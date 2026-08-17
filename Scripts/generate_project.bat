@echo off
pushd ..\
call scripts\premake\premake5.exe --version
call scripts\premake\premake5.exe vs2022
popd
PAUSE
