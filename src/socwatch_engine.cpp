#include "socwatch_engine.h"
#include <windows.h>
#include <string>
#include <vector>
#include <CLI/CLI.hpp>

namespace CoreEngine {

SocWatchEngine::Config SocWatchEngine::SocWatchEngineConfig(int argc, char** argv) {
    CLI::App app{"SocWatch Engine Command Line Interface"};

    SocWatchEngine::Config config;

    app.add_option("-d,--duration", config.duration, "Duration of the SocWatch run in seconds")
       ->required();

    app.add_option("-o,--output", config.outputFileName, "Output CSV file name (without extension)")
       ->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        throw;
    }

    return config;
}

bool SocWatchEngine::IsFileReady(const std::wstring& filePath) {
     // Attempt to open the file with NO sharing allowed
     HANDLE hFile = CreateFileW(
         filePath.c_str(),
         GENERIC_READ,    // Access type
         0,               // dwShareMode = 0 (Exclusive access)
         NULL,            // Security attributes
         OPEN_EXISTING,   // Only open if it exists
         FILE_ATTRIBUTE_NORMAL,
         NULL
     );

     if (hFile != INVALID_HANDLE_VALUE) {
         CloseHandle(hFile); // Success! No other process is holding the file.
         return true;
     }

     // If it failed because another process has it open
     if (GetLastError() == ERROR_SHARING_VIOLATION) {
         return false;
     }

     return false; // Other error (e.g., file not found)
 }
  
const char* SocWatchEngine::Run(unsigned int durationInSeconds, const std::string& outputFileName) {
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  m_lastResult = "Starting SocWatch: ";

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Construct command line
  // Example: socwatch.exe -m -f sys -t 11 -s 1 -o ireport1
  std::wstring wOutputFileName(outputFileName.begin(), outputFileName.end());
  std::wstring cmd = L"socwatch.exe -m -f sys -t " + std::to_wstring(durationInSeconds) + 
                     L" -s 1 -o " + wOutputFileName;

  // CreateProcessW requires a modifiable buffer for the command line
  std::vector<wchar_t> cmdBuffer(cmd.begin(), cmd.end());
  cmdBuffer.push_back(L'\0');

  // Create the process silently
  BOOL success = CreateProcessW(
      NULL,               // Application name
      cmdBuffer.data(),   // Command line
      NULL,               // Process handle not inheritable
      NULL,               // Thread handle not inheritable
      FALSE,              // Set handle inheritance to FALSE
      CREATE_NO_WINDOW,
      NULL,               // Use parent's environment block
      NULL,               // Use parent's starting directory
      &si,                // Pointer to STARTUPINFO structure
      &pi                 // Pointer to PROCESS_INFORMATION structure
  );

  if (success) {
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        if (exitCode != 0) {
            m_lastResult += "Process exited with non-zero code: " + std::to_string(exitCode) + ". ";
        }
    } else {
        m_lastResult += "Failed to get exit code. ";
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Check if the expected output file was created
    std::wstring expectedFile = wOutputFileName + L".csv";
    if (IsFileReady(expectedFile)) {
        m_lastResult += "Successfully launched and output file created.";
    } else {
        m_lastResult += "Process finished but '" + outputFileName + ".csv' was not found or is inaccessible.";
    }
  } else {
    m_lastResult += "CreateProcess failed (Error: ";
    m_lastResult += std::to_string(GetLastError());
    m_lastResult += "). Ensure socwatch.exe is in the PATH or current directory.";
  }

  return m_lastResult.c_str();
}

}  // namespace CoreEngine
