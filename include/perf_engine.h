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
  // profileName: e.g., L"GeneralProfile", L"CPU", L"DiskIO"
  // profileLevel: e.g., L"Light", L"Verbose"
  // Returns true if the trace started successfully
  bool StartTrace(const std::wstring& profileName,
                  const std::wstring& profileLevel = L"Light");
  // etlFileName: Full path where the .etl file will be saved
  // Returns true if the trace was saved successfully
  bool StopTrace(const std::wstring& etlFileName);
  // Checks if a trace is currently active
  bool IsRecording() const { return m_isRecording; }

 private:
  ComPtr<IControlManager> m_controlManager;
  ComPtr<IProfileCollection> m_activeProfiles;
  bool m_isRecording;
  bool m_comInitialized;
  HRESULT InitializeControlManager();
};
}  // namespace CoreEngine