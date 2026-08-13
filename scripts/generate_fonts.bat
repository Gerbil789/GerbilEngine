@echo off
setlocal enabledelayedexpansion

REM Set paths relative to parent directory
pushd ..\

set "FONT_DIR=resources\fonts"
set "GEN_TOOL=scripts\msdf-atlas-gen\msdf-atlas-gen.exe"
set "OUTPUT_IMG=%FONT_DIR%\atlas.png"
set "OUTPUT_JSON=%FONT_DIR%\atlas.json"

set "CMD=%GEN_TOOL%"
set "FIRST=1"

for %%f in ("%FONT_DIR%\*.ttf") do (
    set "FONT_NAME=%%~nf"
    
    if "!FIRST!"=="1" (
        set "CMD=!CMD! -font %%f -fontname "!FONT_NAME!""
        set "FIRST=0"
    ) else (
        set "CMD=!CMD! -and -font %%f -fontname "!FONT_NAME!""
    )
)

set "CMD=!CMD! -type msdf -format png -dimensions 1024 1024 -size 32 -pxrange 4 -outerpxpadding 2 -imageout %OUTPUT_IMG% -json %OUTPUT_JSON%"

echo Running command:
echo !CMD!
echo.

REM Execute the generated command
call !CMD!

popd
PAUSE