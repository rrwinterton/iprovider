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
- **UploadEngine**: Handles secure file uploads to a remote server using WinHTTP.

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
| **Upload** | `UploadEngine_ParseConfig(int argc, char** argv, UploadEngine_Config* outConfig)` | Parses CLI arguments into a config struct. |
| | `CreateUploadEngine()` | Creates an UploadEngine instance. |
| | `UploadEngine_SetServerConfig(EngineHandle handle, const char* location, const char* url)` | Configures the upload server. |
| | `UploadEngine_SetUploadPrefix(EngineHandle handle, const char* prefix)` | Sets a prefix for the remote filename. |
| | `UploadEngine_UploadFile(EngineHandle handle, const char* filePath)` | Uploads a file to the server. |
| | `DestroyUploadEngine(EngineHandle handle)` | Destroys the UploadEngine instance. |

## CLI Configuration Parsing (ireporter.exe)

The library uses `CLI11` for robust command-line argument validation within the `*_ParseConfig` functions. These are intended to be used by the `ireporter.exe` consumer.

### Compress Engine
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--compress` | | Flag | No | Enables the compression operation. |
| `--compress-input` | `-i` | String | **Yes** | Input file paths (supports multiple entries). |
| `--compress-output` | `-o` | String | **Yes** | Path for the resulting ZIP archive. |
| `--compress-archive` | `-a` | String | **Yes** | Internal names for files inside the archive (must match input count). |

Example: `ireporter.exe --compress -i test.txt -o out.zip -a internal.txt`

### Perf Engine
**Global Flags:**
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--perf` | | Flag | No | Identifies performance operation. |
| `--localOnly` | | Flag | No | Perform local only trace. |

**Subcommand: `StartTrace`**
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--profileName` | `-n` | String | **Yes** | Name of the trace profile (e.g., `CPU`) or path to a `.wprp` file. |
| `--profileLevel` | `-l` | String | **Yes** | Detail level (e.g., `Verbose`, `Light`, or `1-5`). |
| `--perf-duration`| `-d` | Int | No | Duration in seconds (0 for indefinite). |
| `--etlFile` | `-f` | String | No | Output path (required if duration > 0). |

Example: `ireporter.exe StartTrace --perf -n CPU -l Verbose -d 10 -f results.etl`

**Subcommand: `StopTrace`**
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--etlFile` | `-f` | String | **Yes** | Output path where the trace will be saved. |

Example: `ireporter.exe StopTrace -f results.etl`

### Socwatch Engine
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--socwatch` | | Flag | No | Identifies the SocWatch operation. |
| `--sw-duration` | `-d` | Int | **Yes** | Duration of the run in seconds. |
| `--sw-output` | `-o` | String | **Yes** | Output CSV filename (**without** extension). |

Example: `ireporter.exe --socwatch -d 60 -o power_report`

### Upload Engine
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--upload` | | Flag | No | Identifies the upload operation. |
| `--upload-location` | `-l` | String | **Yes** | Server location (e.g., `myserver.com`). |
| `--upload-url` | `-u` | String | **Yes** | Server URL path (e.g., `/api/upload`). |
| `--upload-file` | `-f` | String | **Yes** | Path to the file to upload (**Validation: Must exist**). |
| `--upload-prefix` | `-p` | String | No | Prefix for the remote filename (default: `iprovider`). |

Example: `ireporter.exe --upload -l example.com -u /upload -f report.zip -p MyReport`

**Filename Generation:**
The engine generates a unique remote filename using: `{prefix}_{timestamp}_{original_filename}`.
This value is sent to the server in the `X-Original-Filename` HTTP header.
