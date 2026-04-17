#include "socwatch_engine.h"
#include <windows.h>
#include <string>

namespace CoreEngine {

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
  
const char* SocWatchEngine::Run() {
  STARTUPINFOW si;
  PROCESS_INFORMATION pi;
  m_lastResult = "Starting socWatch: ";

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Target executable (make sure to use wide strings or L"")
  WCHAR cmd[] = L"./socwatch.exe -m -f sys -t 11 -s 1 -o ireport1";

  // Create the process silently
  BOOL success = CreateProcessW(
      NULL,   // Application name (can pass NULL if command line is full path)
      cmd,    // Command line
      NULL,   // Process handle not inheritable
      NULL,   // Thread handle not inheritable
      FALSE,  // Set handle inheritance to FALSE
      CREATE_NO_WINDOW,  // <-- THE MAGIC FLAG: Prevents console window
      NULL,              // Use parent's environment block
      NULL,              // Use parent's starting directory
      &si,               // Pointer to STARTUPINFO structure
      &pi                // Pointer to PROCESS_INFORMATION structure
  );

  if (success) {
    // Optional: Wait until child process exits
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles to prevent memory leaks
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    m_lastResult += "Successfully launched.";
  } else {
    m_lastResult += "CreateProcess failed (";
    m_lastResult += std::to_string(GetLastError());
    m_lastResult += ").";
  }

  return m_lastResult.c_str();
}

}  // namespace CoreEngine
