@echo off
set BUILD_TYPE=Release
if "%1"=="Debug" set BUILD_TYPE=Debug
if "%1"=="debug" set BUILD_TYPE=Debug

set BUILD_DIR=build_%BUILD_TYPE%

echo Building %BUILD_TYPE% version...

:: Configure with CMake
cmake -S . -B %BUILD_DIR% -G Ninja ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

:: Build the DLL
cmake --build %BUILD_DIR%

echo.
echo Build complete. Look in the %BUILD_DIR% folder for iprovider.dll
