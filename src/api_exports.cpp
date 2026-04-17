#include "api_exports.h"
#include "compress_engine.h"
#include "math_engine.h"
#include "perf_engine.h"
#include "socwatch_engine.h"

extern "C" {

// --- MathEngine Implementation ---

API_EXPORT EngineHandle CreateMathEngine(int multiplier) {
  auto* engine = new CoreEngine::MathEngine(multiplier);
  return static_cast<EngineHandle>(engine);
}

API_EXPORT int MathEngine_Calculate(EngineHandle handle, int input) {
  if (!handle)
    return -1;
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

API_EXPORT bool CompressEngine_CompressFileMapped(EngineHandle handle,
                                                  const wchar_t* inputFilePath,
                                                  const wchar_t* outputFilePath,
                                                  const char* archiveName) {
  if (!handle || !inputFilePath || !outputFilePath || !archiveName)
    return false;

  auto* engine = static_cast<CoreEngine::CompressEngine*>(handle);
  return engine->CompressFileMapped(inputFilePath, outputFilePath, archiveName);
}

API_EXPORT void DestroyCompressEngine(EngineHandle handle) {
  if (handle) {
    auto* engine = static_cast<CoreEngine::CompressEngine*>(handle);
    delete engine;
  }
}

// --- SocwatchEngine Implementation ---

API_EXPORT EngineHandle CreateSocwatchEngine() {
  auto* engine = new CoreEngine::SocWatchEngine();
  return static_cast<EngineHandle>(engine);
}

API_EXPORT const char* SocwatchEngine_Run(EngineHandle handle) {
  if (!handle)
    return nullptr;
  auto* engine = static_cast<CoreEngine::SocWatchEngine*>(handle);
  return engine->Run();
}

API_EXPORT void DestroySocwatchEngine(EngineHandle handle) {
  if (handle) {
    auto* engine = static_cast<CoreEngine::SocWatchEngine*>(handle);
    delete engine;
  }
}

// --- PerfEngine Implementation ---

API_EXPORT EngineHandle CreatePerfEngine() {
  auto* engine = new CoreEngine::PerfEngine();
  return static_cast<EngineHandle>(engine);
}

API_EXPORT bool PerfEngine_StartTrace(EngineHandle handle,
                                      const wchar_t* profileName,
                                      const wchar_t* profileLevel) {
  if (!handle || !profileName || !profileLevel)
    return false;
  auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
  return engine->StartTrace(profileName, profileLevel);
}

API_EXPORT bool PerfEngine_StopTrace(EngineHandle handle,
                                     const wchar_t* etlFileName) {
  if (!handle || !etlFileName)
    return false;
  auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
  return engine->StopTrace(etlFileName);
}

API_EXPORT bool PerfEngine_IsRecording(EngineHandle handle) {
  if (!handle)
    return false;
  auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
  return engine->IsRecording();
}

API_EXPORT void DestroyPerfEngine(EngineHandle handle) {
  if (handle) {
    auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
    delete engine;
  }
}
}
