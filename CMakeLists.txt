cmake_minimum_required(VERSION 3.20)

# Enforce Clang++ if not specified
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()

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

# Robustly find Windows Performance Toolkit
set(WPT_PATH "C:/Program Files (x86)/Windows Kits/10/Windows Performance Toolkit")
if(NOT EXISTS "${WPT_PATH}")
    if(DEFINED ENV{ProgramFiles\(x86\)})
        set(WPT_PATH "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/Windows Performance Toolkit")
    endif()
endif()

# Tell the compiler where the headers are
target_include_directories(iprovider PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    "${WPT_PATH}"
)

# Link required Windows libraries for PerfEngine
target_link_libraries(iprovider PRIVATE
    Ole32
    OleAut32
)

# Warning flags for Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(iprovider PRIVATE -Wall -Wextra)
endif()

# Debug prints option
option(ENABLE_DEBUG_PRINTS "Enable debug print statements" OFF)
if(ENABLE_DEBUG_PRINTS)
    target_compile_definitions(iprovider PRIVATE DEBUG_PRINTS)
endif()

# Define a macro so the code knows it is BUILDING the DLL, not consuming it.
# This triggers __declspec(dllexport) instead of __declspec(dllimport).
target_compile_definitions(iprovider PRIVATE UNMANGLED_LIB_EXPORTS)

# --- Tests ---
add_executable(test_api tests/test_api.cpp)
target_include_directories(test_api PRIVATE include)
target_link_libraries(test_api PRIVATE iprovider)

# Ensure the DLL is copied to the same directory as the executable
add_custom_command(TARGET test_api POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:iprovider>
        $<TARGET_FILE_DIR:test_api>
)

# --- iprovider EXE ---
add_executable(iprovider_exe src/iprovider_main.cpp)
set_target_properties(iprovider_exe PROPERTIES OUTPUT_NAME iprovider)
target_include_directories(iprovider_exe PRIVATE include)
# We don't link iprovider.lib here because we use LoadLibrary in the code,
# but we need the DLL to be in the same folder.
add_custom_command(TARGET iprovider_exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:iprovider>
        $<TARGET_FILE_DIR:iprovider_exe>
)
