cmake_minimum_required(VERSION 3.20)

# Enforce Clang++
set(CMAKE_CXX_COMPILER "clang++")

project(iProviderDllProject VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the DLL target
add_library(iprovider SHARED
    src/math_engine.cpp
    src/compress_engine.cpp
    src/socwatch_engine.cpp
    src/perf_engine.cpp
    src/api_exports.cpp
    src/miniz.c
)

# Tell the compiler where the headers are
target_include_directories(iprovider PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    "C:/Program Files (x86)/Windows Kits/10/Windows Performance Toolkit"
)

# Link required Windows libraries for PerfEngine
# Using the GUID-based CoCreateInstance means we don't necessarily need a .lib
# but we link Ole32 and any other needed system libs.
target_link_libraries(iprovider PRIVATE
    Ole32
    OleAut32
)

# Define a macro so the code knows it is BUILDING the DLL, not consuming it.
# This triggers __declspec(dllexport) instead of __declspec(dllimport).
target_compile_definitions(iprovider PRIVATE UNMANGLED_LIB_EXPORTS)
