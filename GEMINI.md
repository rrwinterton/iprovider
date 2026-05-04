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
- `build.bat`: Main build entry point.
- `include/api_exports.h`: The unmangled C-API header (the primary consumer interface).
- `src/api_exports.cpp`: Implementation of the C-to-C++ bridge.
- `include/math_engine.h` / `src/math_engine.cpp`: Internal Math engine (for testing).
- `include/compress_engine.h" / `src/compress_engine.cpp`: Logic for file compression.
- `include/socwatch_engine.h" / `src/socwatch_engine.cpp`: Power and performance monitoring via SocWatch.
- `include/perf_engine.h" / `src/perf_engine.cpp`: Performance tracing via Windows Performance Toolkit.
- `tests/test_api.cpp`: C-API validation and integration tests.

## C-API Export List

The following functions and types are exported using `extern "C"` to ensure unmangled names:

### Math Engine
- `EngineHandle CreateMathEngine(int multiplier)`
- `int MathEngine_Calculate(EngineHandle handle, int input)`
- `void DestroyMathEngine(EngineHandle handle)`

### Compress Engine
**Configuration Struct:**
```c
typedef struct {
    bool doCompress;
    char** inputFilePaths;
    int inputFileCount;
    char outputFilePath[260];
    char** archiveNames;
    int archiveNameCount;
} CompressEngine_Config;
```
**Functions:**
- `bool CompressEngine_ParseConfig(int argc, char** argv, CompressEngine_Config* outConfig)`
- `void CompressEngine_FreeConfig(CompressEngine_Config* config)`
- `EngineHandle CreateCompressEngine()`
- `bool CompressEngine_CompressFileMapped(EngineHandle handle, const wchar_t** inputFilePaths, int inputFileCount, const wchar_t* outputFilePath, const char** archiveNames, int archiveNameCount)`
- `void DestroyCompressEngine(EngineHandle handle)`

**CLI Parameters (ireporter.exe):**

| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--compress` | | Flag | No | Enables the compression operation. |
| `--compress-input` | `-i` | String | **Yes** | Input file paths (supports multiple entries). |
| `--compress-output` | `-o` | String | **Yes** | Path for the resulting ZIP archive. |
| `--compress-archive` | `-a` | String | **Yes** | Internal names for files inside the archive (must match input count). |

Example: `ireporter.exe --compress -i file1.txt -o out.zip -a arch1.txt`

### Socwatch Engine
**Configuration Struct:**
```c
typedef struct {
    bool doSocwatch;
    unsigned int duration;
    char outputFileName[260];
} SocwatchEngine_Config;
```
**Functions:**
- `bool SocwatchEngine_ParseConfig(int argc, char** argv, SocwatchEngine_Config* outConfig)`
- `EngineHandle CreateSocwatchEngine()`
- `const char* SocwatchEngine_Run(EngineHandle handle, unsigned int durationInSeconds, const char* outputFileName)`
- `void DestroySocwatchEngine(EngineHandle handle)`

**CLI Parameters (ireporter.exe):**

| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--socwatch` | | Flag | No | Identifies the SocWatch operation. |
| `--sw-duration` | `-d` | Int | **Yes** | Duration of the run in seconds. |
| `--sw-output" | `-o` | String | **Yes** | Output CSV filename (**without** extension). |

Example: `ireporter.exe --socwatch -d 30 -o results`

### Perf Engine
**Configuration Struct:**
```c
typedef struct {
    bool perf;
    bool localOnly;
    bool isStartTrace;
    char profileName[256];
    char profileLevel[256];
    unsigned int duration;
    bool isStopTrace;
    char etlFile[260];
} PerfEngine_Config;
```
**Functions:**
- `bool PerfEngine_ParseConfig(int argc, char** argv, PerfEngine_Config* outConfig)`
- `EngineHandle CreatePerfEngine()`
- `bool PerfEngine_StartTrace(EngineHandle handle, const wchar_t* profileName, const wchar_t* profileLevel, unsigned int duration, const wchar_t* etlFileName)`
- `bool PerfEngine_StopTrace(EngineHandle handle, const wchar_t* etlFileName)`
- `bool PerfEngine_IsRecording(EngineHandle handle)`
- `const char* PerfEngine_GetLastResult(EngineHandle handle)`
- `void DestroyPerfEngine(EngineHandle handle)`

**CLI Parameters (ireporter.exe):**

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

Example: `ireporter.exe StartTrace --perf -n CPU -l Verbose -d 10 -f trace.etl`

**Subcommand: `StopTrace`**
| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--etlFile` | `-f` | String | **Yes** | Output path where the trace will be saved. |

Example: `ireporter.exe StopTrace -f trace.etl`

### Upload Engine
**Configuration Struct:**
```c
typedef struct {
    bool doUpload;
    char serverLocation[260];
    char serverUrl[260];
    char uploadFile[260];
    char filePrefix[260];
} UploadEngine_Config;
```
**Functions:**
- `bool UploadEngine_ParseConfig(int argc, char** argv, UploadEngine_Config* outConfig)`
- `EngineHandle CreateUploadEngine()`
- `void UploadEngine_SetServerConfig(EngineHandle handle, const char* location, const char* url)`
- `void UploadEngine_SetUploadPrefix(EngineHandle handle, const char* prefix)`
- `bool UploadEngine_UploadFile(EngineHandle handle, const char* filePath)`
- `void DestroyUploadEngine(EngineHandle handle)`

**CLI Parameters (ireporter.exe):**

| Parameter | Short | Type | Required | Description |
| :--- | :--- | :--- | :--- | :--- |
| `--upload` | | Flag | No | Identifies the upload operation. |
| `--upload-location` | `-l` | String | **Yes** | Server location (e.g., `example.com`). |
| `--upload-url` | `-u` | String | **Yes** | Server URL path (e.g., `/upload`). |
| `--upload-file` | `-f` | String | **Yes** | Path to the file to upload (**Validation: Must exist**). |
| `--upload-prefix` | `-p` | String | No | Prefix for the remote filename (default: `iprovider`). |

Example: `ireporter.exe --upload -l example.com -u /api/upload -f data.txt -p MyPrefix`

**Filename Pattern:** The remote filename is generated as `{prefix}_{YYYYMMDD_HHMMSS}_{original_name}` and sent in the `X-Original-Filename` header.

## Integration Context

- **ireporter**: The primary consumer of this instrumentation provider.
- **itemplate**: The testing framework where this provider is validated.
