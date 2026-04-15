@echo off
set BUILD_DIR=build

:: Configure with CMake, forcing the Ninja generator and Clang compiler
cmake -S . -B %BUILD_DIR% -G Ninja ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_BUILD_TYPE=Release

:: Build the DLL
cmake --build %BUILD_DIR%

echo.
echo Build complete. Look in the %BUILD_DIR% folder for iprovider.dll
