#include "api_exports.h"
#include "math_engine.h"

extern "C" {

    API_EXPORT EngineHandle CreateMathEngine(int multiplier) {
        // Create the C++ object and cast it to a void pointer
        auto* engine = new CoreEngine::MathEngine(multiplier);
        return static_cast<EngineHandle>(engine);
    }

    API_EXPORT int MathEngine_Calculate(EngineHandle handle, int input) {
        if (!handle) return -1; // Basic safety check
        
        // Cast the void pointer back to the C++ class
        auto* engine = static_cast<CoreEngine::MathEngine*>(handle);
        return engine->Calculate(input);
    }

    API_EXPORT void DestroyMathEngine(EngineHandle handle) {
        if (handle) {
            auto* engine = static_cast<CoreEngine::MathEngine*>(handle);
            delete engine;
        }
    }

}
