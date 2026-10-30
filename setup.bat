@echo off
setlocal enabledelayedexpansion

if exist CMakeUserPresets.json del CMakeUserPresets.json
if exist CMakePresets.json del CMakePresets.json
if exist build rmdir /s /q build

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