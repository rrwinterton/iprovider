#pragma once

#include <windows.h>
#include <WindowsPerformanceRecorderControl.h>
#include <wrl/client.h>
#include <string>
#include <vector>
using Microsoft::WRL::ComPtr;
namespace CoreEngine {
class PerfEngine {
 public:
  PerfEngine();
  ~PerfEngine();

  // Structure to hold performance engine configuration
  struct Config {
    bool isStartTrace = false;
    std::string profileName;
    std::string profileLevel;
    unsigned int duration = 0;
    std::string etlFileName; // Used by both Start (for auto-stop) and Stop

    bool isStopTrace = false;
  };

  // Parses command line arguments to generate a configuration
  static Config PerfEngineConfig(int argc, char** argv);

  // profileName: e.g., L"GeneralProfile", L"CPU", L"DiskIO"
  // profileLevel: e.g., L"Light", L"Verbose"
  // duration: time in seconds to record (0 for indefinite)
  // etlFileName: filename to use if duration > 0 (auto-stop)
  // Returns true if the trace started successfully
  bool StartTrace(const std::wstring& profileName = L"CPU",
                  const std::wstring& profileLevel = L"Light",
                  unsigned int duration = 0,
                  const std::wstring& etlFileName = L"etlTrace.etl");
  // etlFileName: Full path where the .etl file will be saved
  // Returns true if the trace was saved successfully
  bool StopTrace(const std::wstring& etlFileName);
  // Checks if a trace is currently active
  bool IsRecording() const { return m_isRecording; }

  // Returns the last result or error message
  const char* GetLastResult() const { return m_lastResult.c_str(); }

 private:
  ComPtr<IControlManager> m_controlManager;
  ComPtr<IProfileCollection> m_activeProfiles;
  bool m_isRecording;
  bool m_comInitialized;
  std::string m_lastResult;
  HRESULT InitializeControlManager();
};
}  // namespace CoreEngine