@echo off
setlocal enabledelayedexpansion

if exist CMakeUserPresets.json del /f /q CMakeUserPresets.json
if exist CMakePresets.json del /f /q CMakePresets.json
if exist build rmdir /s /q build

set "MISSING_TOOLS="

where conan >nul 2>&1
if errorlevel 1 set "MISSING_TOOLS=!MISSING_TOOLS! conan"

where cmake >nul 2>&1
if errorlevel 1 set "MISSING_TOOLS=!MISSING_TOOLS! cmake"

where ninja >nul 2>&1
if errorlevel 1 set "MISSING_TOOLS=!MISSING_TOOLS! ninja"

where cl >nul 2>&1
set "HAS_COMPILER=%errorlevel%"
if %HAS_COMPILER% neq 0 (
    where gcc >nul 2>&1
    if errorlevel 1 set "MISSING_TOOLS=!MISSING_TOOLS! compiler(cl/gcc)"
)

if not "!MISSING_TOOLS!"=="" (
    echo Error: The following required tool(s) are missing: !MISSING_TOOLS! >&2
    echo Please install them and ensure they are added to your system PATH. >&2
    exit /b 1
)

where ccache >nul 2>&1
if not errorlevel 1 (
    echo --> Optional tool found: ccache
) else (
    echo --> Note: ccache not found (continuing without it).
)

timeout /t 2 /nobreak >nul

conan profile path default >nul 2>&1
if %errorlevel% neq 0 (
    conan profile detect --force
)

set CONAN_DIR=conan

echo --> [1/4] x64 Debug...
conan install .\%CONAN_DIR% -pr:h=.\%CONAN_DIR%\profiles\win_x64_msvc -s build_type=Debug -b=missing --update
if %errorlevel% neq 0 exit /b %errorlevel%

echo --> [2/4] x64 Release...
conan install .\%CONAN_DIR% -pr:h=.\%CONAN_DIR%\profiles\win_x64_msvc -s build_type=Release -b=missing --update
if %errorlevel% neq 0 exit /b %errorlevel%

echo --> [3/4] x86 Debug...
conan install .\%CONAN_DIR% -pr:h=.\%CONAN_DIR%\profiles\win_x86_msvc -s arch=x86 -s build_type=Debug -b=missing --update
if %errorlevel% neq 0 exit /b %errorlevel%

echo --> [4/4] x86 Release...
conan install .\%CONAN_DIR% -pr:h=.\%CONAN_DIR%\profiles\win_x86_msvc -s arch=x86 -s build_type=Release -b=missing --update
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo === Done! All build trees generated. ===
echo You can now reload the CMakeLists.txt in your IDE or configure and build manually:
echo   - cmake --list-presets              | to list all presets
echo   - cmake --preset x64-debug          | to configure the preset
echo   - cmake --build --preset x64-debug  | to build the preset