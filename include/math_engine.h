#pragma once

namespace CoreEngine {
class MathEngine {
 public:
  MathEngine(int multiplier);
  ~MathEngine() = default;

  int Calculate(int input);

 private:
  int m_multiplier;
};
}  // namespace CoreEngine
