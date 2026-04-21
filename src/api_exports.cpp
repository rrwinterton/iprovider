#include "api_exports.h"
#include <cstring>
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

API_EXPORT bool CompressEngine_ParseConfig(int argc, char** argv, CompressEngine_Config* outConfig) {
  if (!outConfig) return false;

  try {
    CoreEngine::CompressEngine::Config cppConfig = CoreEngine::CompressEngine::CompressEngineConfig(argc, argv);
    
    // Convert wstring to string for the C struct
    std::string inputPath(cppConfig.inputFilePath.begin(), cppConfig.inputFilePath.end());
    std::string outputPath(cppConfig.outputFilePath.begin(), cppConfig.outputFilePath.end());

    strncpy_s(outConfig->inputFilePath, inputPath.c_str(), _TRUNCATE);
    strncpy_s(outConfig->outputFilePath, outputPath.c_str(), _TRUNCATE);
    strncpy_s(outConfig->archiveName, cppConfig.archiveName.c_str(), _TRUNCATE);
    
    return true;
  } catch (...) {
    return false;
  }
}

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

API_EXPORT bool SocwatchEngine_ParseConfig(int argc, char** argv, SocwatchEngine_Config* outConfig) {
  if (!outConfig) return false;

  try {
    CoreEngine::SocWatchEngine::Config cppConfig = CoreEngine::SocWatchEngine::SocWatchEngineConfig(argc, argv);
    
    outConfig->duration = cppConfig.duration;
    strncpy_s(outConfig->outputFileName, cppConfig.outputFileName.c_str(), _TRUNCATE);
    
    return true;
  } catch (...) {
    return false;
  }
}

API_EXPORT EngineHandle CreateSocwatchEngine() {
  auto* engine = new CoreEngine::SocWatchEngine();
  return static_cast<EngineHandle>(engine);
}

API_EXPORT const char* SocwatchEngine_Run(EngineHandle handle, unsigned int durationInSeconds, const char* outputFileName) {
  if (!handle || !outputFileName)
    return nullptr;
  auto* engine = static_cast<CoreEngine::SocWatchEngine*>(handle);
  return engine->Run(durationInSeconds, std::string(outputFileName));
}

API_EXPORT void DestroySocwatchEngine(EngineHandle handle) {
  if (handle) {
    auto* engine = static_cast<CoreEngine::SocWatchEngine*>(handle);
    delete engine;
  }
}

// --- PerfEngine Implementation ---

API_EXPORT bool PerfEngine_ParseConfig(int argc, char** argv, PerfEngine_Config* outConfig) {
  if (!outConfig) return false;

  try {
    CoreEngine::PerfEngine::Config cppConfig = CoreEngine::PerfEngine::PerfEngineConfig(argc, argv);
    
    outConfig->isStartTrace = cppConfig.isStartTrace;
    strncpy_s(outConfig->profileName, cppConfig.profileName.c_str(), _TRUNCATE);
    strncpy_s(outConfig->profileLevel, cppConfig.profileLevel.c_str(), _TRUNCATE);
    outConfig->duration = cppConfig.duration;
    
    outConfig->isStopTrace = cppConfig.isStopTrace;
    strncpy_s(outConfig->etlFileName, cppConfig.etlFileName.c_str(), _TRUNCATE);
    
    return true;
  } catch (...) {
    return false;
  }
}

API_EXPORT EngineHandle CreatePerfEngine() {
  auto* engine = new CoreEngine::PerfEngine();
  return static_cast<EngineHandle>(engine);
}

API_EXPORT bool PerfEngine_StartTrace(EngineHandle handle,
                                      const wchar_t* profileName,
                                      const wchar_t* profileLevel,
                                      unsigned int duration,
                                      const wchar_t* etlFileName) {
  if (!handle || !profileName || !profileLevel || !etlFileName)
    return false;
  auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
  return engine->StartTrace(profileName, profileLevel, duration, etlFileName);
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

API_EXPORT const char* PerfEngine_GetLastResult(EngineHandle handle) {
  if (!handle)
    return nullptr;
  auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
  return engine->GetLastResult();
}

API_EXPORT void DestroyPerfEngine(EngineHandle handle) {
  if (handle) {
    auto* engine = static_cast<CoreEngine::PerfEngine*>(handle);
    delete engine;
  }
}
}
