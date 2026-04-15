#include "math_engine.h"

namespace CoreEngine {
    MathEngine::MathEngine(int multiplier) : m_multiplier(multiplier) {}

    int MathEngine::Calculate(int input) {
        return input * m_multiplier;
    }
}
