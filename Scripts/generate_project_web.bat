@echo off
pushd ..\
call vendor\premake\premake5.exe --version
call vendor\premake\premake5.exe gmake --os=linux
popd
PAUSE