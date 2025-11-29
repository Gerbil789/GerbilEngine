@echo off
REM Load VS environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Build bootstrap
cd ..\vendor\premake
call bootstrap.bat

REM REM Build full premake using MSBuild
cd build\bootstrap
msbuild Premake5.sln /t:Premake5 /m /p:Configuration=Release /p:Platform=x64

REM Copy final exe to repo root or vendor/bin
echo Premake5.exe built successfully

PAUSE