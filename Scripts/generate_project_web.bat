@echo off
pushd ..\
call vendor\premake\bin\release\premake5.exe --version
call vendor\premake\bin\release\premake5.exe gmake --os=linux
popd
PAUSE