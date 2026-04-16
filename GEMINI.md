# Gemini Context: iprovider

`iprovider` is a C++ instrumentation provider library (DLL) for the `ireporter` project. It exposes a C-compatible API to allow for easy integration and testing, specifically within the `itemplate` testing framework.

## Project Architecture

- **C++ Core Engine**: The core logic is implemented in C++ (e.g., `CoreEngine::MathEngine`).
- **C-API Bridge**: A C-compatible wrapper layer (`api_exports.h`/`.cpp`) exposes the C++ functionality to consumers without requiring C++ name mangling or ABI compatibility issues.
- **Opaque Handles**: Consumers interact with the engine using opaque pointers (`EngineHandle`), which are managed by lifecycle functions (`CreateMathEngine`, `DestroyMathEngine`).

## Build System

- **Compiler**: Clang++ (enforced in `CMakeLists.txt` and `build.bat`).
- **Generator**: Ninja (specified in `build.bat`).
- **Build Script**: `build.bat` automates the CMake configuration and build process.
- **Output**: The project produces `iprovider.dll` and `iprovider.lib` in the `build/` directory.

### Build Commands
To build the project in Release mode:
```powershell
.\build.bat
```

To build the project in Debug mode:
```powershell/command prompt
cmake -S . -B build_debug -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
cmake --build build_debug
```

To build the project in Release mode with symbols:
```powershell/command prompt
cmake -S . -B build_relwithdebinfo -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build_relwithdebinfo
```

## Coding Conventions

- **Namespaces**: Core implementation should reside within the `CoreEngine` namespace.
- **File Structure**:
  - `include/`: Header files for the core engine and the C-API exports.
  - `src/`: Implementation files.
- **C-API Wrapper**:
  - Functions in `api_exports.h` must be wrapped in `extern "C"`.
  - Use `API_EXPORT` macro for DLL export/import.
  - Always provide a `Destroy*` function for any `Create*` function to prevent memory leaks.

## Key Files

- `CMakeLists.txt`: Project configuration and compiler enforcement.
- `include/api_exports.h`: The unmangled C-API header (the primary consumer interface).
- `include/math_engine.h`: Internal C++ engine definition.
- `src/api_exports.cpp`: Implementation of the C-to-C++ bridge.
- `build.bat`: Main build entry point.

## Integration Context

- **ireporter**: The primary consumer of this instrumentation provider.
- **itemplate**: The testing framework where this provider is validated.
