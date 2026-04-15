#pragma once

// Handle Windows DLL export/import semantics
#ifdef _WIN32
    #ifdef UNMANGLED_LIB_EXPORTS
        #define API_EXPORT __declspec(dllexport)
    #else
        #define API_EXPORT __declspec(dllimport)
    #endif
#else
    // Fallback for Linux/macOS if you cross-compile later
    #define API_EXPORT __attribute__((visibility("default")))
#endif

// Define an opaque type for our C++ class pointer
typedef void* EngineHandle;

// The extern "C" block disables C++ name mangling for these functions
extern "C" {
    // 1. Function to create the class
    API_EXPORT EngineHandle CreateMathEngine(int multiplier);

    // 2. Function to use the class
    API_EXPORT int MathEngine_Calculate(EngineHandle handle, int input);

    // 3. Function to destroy the class (prevents memory leaks)
    API_EXPORT void DestroyMathEngine(EngineHandle handle);

    // --- CompressEngine Exports ---

    // 4. Function to create the CompressEngine
    API_EXPORT EngineHandle CreateCompressEngine();

    // 5. Function to use the CompressEngine
    API_EXPORT bool CompressEngine_CompressFileMapped(EngineHandle handle, const wchar_t* inputFilePath, const wchar_t* outputFilePath);

    // 6. Function to destroy the CompressEngine
    API_EXPORT void DestroyCompressEngine(EngineHandle handle);
}
