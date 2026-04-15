#include "api_exports.h"
#include "math_engine.h"
#include "compress_engine.h"

extern "C" {

    // --- MathEngine Implementation ---

    API_EXPORT EngineHandle CreateMathEngine(int multiplier) {
        auto* engine = new CoreEngine::MathEngine(multiplier);
        return static_cast<EngineHandle>(engine);
    }

    API_EXPORT int MathEngine_Calculate(EngineHandle handle, int input) {
        if (!handle) return -1;
        auto* engine = static_cast<CoreEngine::MathEngine*>(handle);
        return engine->Calculate(input);
    }

    API_EXPORT void DestroyMathEngine(EngineHandle handle) {
        if (handle) {
            auto* engine = static_cast<CoreEngine::MathEngine*>(handle);
            delete engine;
        }
    }

    // --- CompressEngine Implementation ---

    API_EXPORT EngineHandle CreateCompressEngine() {
        auto* engine = new CoreEngine::CompressEngine();
        return static_cast<EngineHandle>(engine);
    }

    API_EXPORT bool CompressEngine_CompressFileMapped(EngineHandle handle, const wchar_t* inputFilePath, const wchar_t* outputFilePath) {
        if (!handle || !inputFilePath || !outputFilePath) return false;
        
        auto* engine = static_cast<CoreEngine::CompressEngine*>(handle);
        return engine->CompressFileMapped(inputFilePath, outputFilePath);
    }

    API_EXPORT void DestroyCompressEngine(EngineHandle handle) {
        if (handle) {
            auto* engine = static_cast<CoreEngine::CompressEngine*>(handle);
            delete engine;
        }
    }

}
