@echo off

setlocal enabledelayedexpansion

REM Go to repo root (the script is inside /Scripts)
cd /d "%~dp0.."

echo === Building Premake ===

REM Load VS environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Move into premake dir
cd vendor\premake || (echo Premake folder missing & exit /b 1)


REM Build bootstrap
call bootstrap.bat
if %errorlevel% neq 0 (
    echo Bootstrap FAILED
    exit /b 1
)

REM Enter bootstrap project directory
cd build\bootstrap || (echo Missing build/bootstrap folder & exit /b 1)

REM REM Build full premake using MSBuild
cd build\bootstrap
msbuild Premake5.sln /t:Premake5 /m /p:Configuration=Release /p:Platform=x64

if %errorlevel% neq 0 (
    echo Premake build FAILED
    exit /b 1
)

echo === Premake5 built successfully ===

endlocal