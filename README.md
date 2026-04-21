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

## Exported API

The `iprovider.dll` exports a C-compatible (unmangled) API defined in `api_exports.h`.

| Engine | Function | Description |
| :--- | :--- | :--- |
| **Math** | `CreateMathEngine(int multiplier)` | Creates a MathEngine instance. |
| | `MathEngine_Calculate(EngineHandle handle, int input)` | Performs calculation using the engine. |
| | `DestroyMathEngine(EngineHandle handle)` | Destroys the MathEngine instance. |
| **Compress** | `CompressEngine_ParseConfig(int argc, char** argv, CompressEngine_Config* outConfig)` | Parses CLI arguments into a config struct. |
| | `CompressEngine_FreeConfig(CompressEngine_Config* config)` | Frees memory allocated for the config struct. |
| | `CreateCompressEngine()` | Creates a CompressEngine instance. |
| | `CompressEngine_CompressFileMapped(EngineHandle handle, const wchar_t** inputFilePaths, int inputFileCount, const wchar_t* outputFilePath, const char** archiveNames, int archiveNameCount)` | Compresses multiple files using memory-mapping. |
| | `DestroyCompressEngine(EngineHandle handle)` | Destroys the CompressEngine instance. |
| **SocWatch** | `SocwatchEngine_ParseConfig(int argc, char** argv, SocwatchEngine_Config* outConfig)` | Parses CLI arguments into a config struct. |
| | `CreateSocwatchEngine()` | Creates a SocWatchEngine instance. |
| | `SocwatchEngine_Run(EngineHandle handle, unsigned int durationInSeconds, const char* outputFileName)` | Runs the SocWatch tool. |
| | `DestroySocwatchEngine(EngineHandle handle)` | Destroys the SocWatchEngine instance. |
| **Perf** | `PerfEngine_ParseConfig(int argc, char** argv, PerfEngine_Config* outConfig)` | Parses CLI arguments into a config struct. |
| | `CreatePerfEngine()` | Creates a PerfEngine instance. |
| | `PerfEngine_StartTrace(EngineHandle handle, const wchar_t* profileName, const wchar_t* profileLevel, unsigned int duration, const wchar_t* etlFileName)` | Starts a performance trace. |
| | `PerfEngine_StopTrace(EngineHandle handle, const wchar_t* etlFileName)` | Stops a trace and saves to ETL. |
| | `PerfEngine_IsRecording(EngineHandle handle)` | Checks if a trace is active. |
| | `PerfEngine_GetLastResult(EngineHandle handle)` | Gets the last result message. |
| | `DestroyPerfEngine(EngineHandle handle)` | Destroys the PerfEngine instance. |

## CLI Configuration Parsing

The library uses `CLI11` for robust command-line argument validation within the `*_ParseConfig` functions.

### `PerfEngine_ParseConfig`
Supports two subcommands (exactly one must be provided):

**Subcommand: `StartTrace`**
- `-n, --profileName` (Required): Name of the trace profile (e.g., `GeneralProfile`, `CPU`).
- `-l, --profileLevel` (Required): Detail level of the profile (e.g., `Light`, `Verbose`).
- `-d, --duration` (Optional): Duration in seconds (default: `0` for indefinite).
- `-f, --etlFileName` (Optional): Output path for the saved `.etl` file (used if duration > 0).

**Subcommand: `StopTrace`**
- `-f, --etlFileName` (Required): Full output path for the saved `.etl` file.

---

### `SocwatchEngine_ParseConfig`
- `-d, --duration` (Required): Duration of the SocWatch run in seconds.
- `-o, --output` (Required): Output CSV file name (without extension).

---

### `CompressEngine_ParseConfig`
- `-i, --input` (Required, Multiple): Paths to the input files to be compressed.
- `-o, --output` (Required): Path to the output ZIP archive.
- `-a, --archiveName` (Required, Multiple): Names the files will take inside the archive.
