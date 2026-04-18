# iprovider
A C++ instrumentation provider library (DLL) for the `ireporter` project. These providers are primarily validated within the `itemplate` testing framework.

## Project Structure

```text
iprovider/
├── include/
│   ├── CLI/
│   │   └── CLI.hpp             # Command line parsing library
│   ├── api_exports.h           # C-compatible API exports
│   ├── compress_engine.h       # ZIP compression logic
│   ├── math_engine.h           # Basic math operations
│   ├── miniz.h                 # miniz library header
│   ├── perf_engine.h           # WPR performance tracing
│   └── socwatch_engine.h       # Intel SocWatch integration
├── src/
│   ├── api_exports.cpp         # C-API bridge implementation
│   ├── compress_engine.cpp     # Memory-mapped file compression
│   ├── math_engine.cpp         # Math implementation
│   ├── miniz.c                 # miniz implementation
│   ├── perf_engine.cpp         # WPR control implementation
│   └── socwatch_engine.cpp     # SocWatch execution logic
├── .clang-format
├── build.bat                   # Build script (Ninja/Clang++)
├── CMakeLists.txt              # Project configuration
├── GEMINI.md                   # Project context for Gemini
└── README.md
```

## Available Engines

- **MathEngine**: Performs basic calculations and scaling.
- **CompressEngine**: Uses `miniz` and Windows memory-mapped files to compress data into ZIP archives.
- **PerfEngine**: Interfaces with the Windows Performance Recorder (WPR) to start and stop system traces.
- **SocWatchEngine**: Automates the execution of the Intel SocWatch tool for power and performance analysis.

## Building the Project

The project uses CMake with a Ninja generator and requires `clang++`.

```powershell
.\build.bat
```
