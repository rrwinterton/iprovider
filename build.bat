@echo off
setlocal

set BUILD_TYPE=Release
set DO_CLEAN=0
set ENABLE_DEBUG_PRINTS=0

:parse_args
if "%1"=="" goto end_parse_args
if /I "%1"=="Debug" set BUILD_TYPE=Debug
if /I "%1"=="RelWithDebInfo" set BUILD_TYPE=RelWithDebInfo
if /I "%1"=="RelSymbols" set BUILD_TYPE=RelWithDebInfo
if /I "%1"=="clean" set DO_CLEAN=1
if /I "%1"=="debug_prints" set ENABLE_DEBUG_PRINTS=1
shift
goto parse_args
:end_parse_args

set BUILD_DIR=build_%BUILD_TYPE%

if %DO_CLEAN%==1 (
    echo Cleaning %BUILD_DIR%...
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
)

echo Building %BUILD_TYPE% version...
if %ENABLE_DEBUG_PRINTS%==1 echo Debug prints are ENABLED.

:: Configure with CMake
set EXTRA_OPTS=
if %ENABLE_DEBUG_PRINTS%==1 set EXTRA_OPTS=-DENABLE_DEBUG_PRINTS=ON

cmake -S . -B %BUILD_DIR% -G Ninja ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    %EXTRA_OPTS%

:: Build the DLL
cmake --build %BUILD_DIR%

echo.
echo Build complete. Look in the %BUILD_DIR% folder for iprovider.dll
endlocal
