#include "perf_engine.h"
#include <iostream>
#include <initguid.h>
#include <iomanip>

// WPR Control API GUIDs
DEFINE_GUID(CLSID_WPRControl, 0x971A7808, 0x88BE, 0x4AAD, 0x9E, 0x1E, 0x7C, 0x7E, 0x25, 0x85, 0x12, 0xE3);
DEFINE_GUID(IID_IControlManager, 0xC66BC1C2, 0xD913, 0x4BF0, 0x9E, 0x08, 0x01, 0x45, 0x23, 0xE4, 0xA2, 0x05);
DEFINE_GUID(CLSID_CProfile, 0xD66D26BF, 0x6098, 0x4B78, 0x9D, 0x94, 0x7B, 0xC2, 0x19, 0x61, 0x2A, 0xE4);
DEFINE_GUID(IID_IProfile2, 0xC732A38E, 0x0699, 0x4ACC, 0x90, 0x70, 0xA0, 0xA4, 0x3F, 0x56, 0x8E, 0x34);
DEFINE_GUID(CLSID_CProfileCollection, 0xB98B53F3, 0x83BA, 0x4837, 0x89, 0x46, 0xE8, 0x86, 0xBE, 0x8D, 0x40, 0x03);
DEFINE_GUID(IID_IProfileCollection2, 0x47C60A6D, 0x30A2, 0x46C9, 0x85, 0xAC, 0x79, 0xEE, 0xD0, 0xD5, 0x84, 0xE4);
DEFINE_GUID(CLSID_CTraceMergeProperties, 0xCAB2BDD5, 0x5B3B, 0x40AC, 0x98, 0x5F, 0x2E, 0xA4, 0x9E, 0x43, 0x0F, 0xE4);
DEFINE_GUID(IID_ITraceMergeProperties, 0x629E2BBB, 0x9629, 0x45D8, 0x83, 0x14, 0x5D, 0x72, 0x32, 0x4A, 0xE3, 0x3F);

typedef HRESULT (WINAPI *WPRCCreateInstanceUnderInstanceNameFunc)(
    BSTR bstrInstanceName,
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
);

namespace CoreEngine {

PerfEngine::PerfEngine() : m_isRecording(false), m_comInitialized(false) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr) || hr == S_FALSE) {
        m_comInitialized = true;
    }
}

PerfEngine::~PerfEngine() {
    if (m_isRecording) {
        StopTrace(L"emergency_backup.etl");
    }
    m_activeProfiles.Reset();
    m_controlManager.Reset();
    if (m_comInitialized) {
        CoUninitialize();
    }
}

HRESULT PerfEngine::InitializeControlManager() {
    if (m_controlManager) return S_OK;

    HMODULE hWpr = GetModuleHandleW(L"WindowsPerformanceRecorderControl.dll");
    if (!hWpr) hWpr = LoadLibraryW(L"WindowsPerformanceRecorderControl.dll");
    if (!hWpr) return HRESULT_FROM_WIN32(GetLastError());

    WPRCCreateInstanceUnderInstanceNameFunc pFunc = (WPRCCreateInstanceUnderInstanceNameFunc)GetProcAddress(hWpr, "WPRCCreateInstanceUnderInstanceName");
    if (!pFunc) return HRESULT_FROM_WIN32(GetLastError());

    return pFunc(NULL, CLSID_WPRControl, NULL, CLSCTX_INPROC_SERVER, IID_IControlManager, (void**)&m_controlManager);
}

bool PerfEngine::StartTrace(const std::wstring& profileName, const std::wstring& profileLevel) {
    if (m_isRecording) return false;

    InitializeControlManager();

    // 1. Try wpr.exe first as it is more robust for session management
    std::wstring pName = profileName.empty() ? L"GeneralProfile" : profileName;
    std::wstring cmdStart = L"wpr -start " + pName + L" -filemode";
    
    // Attempt to cancel any existing first
    _wsystem(L"wpr -cancel >nul 2>&1");
    
    if (_wsystem(cmdStart.c_str()) == 0) {
        m_isRecording = true;
        return true;
    }

    // 2. Fallback to API if wpr.exe failed (e.g. not in PATH)
    if (!m_controlManager) return false;

    HMODULE hWpr = GetModuleHandleW(L"WindowsPerformanceRecorderControl.dll");
    WPRCCreateInstanceUnderInstanceNameFunc pFunc = (WPRCCreateInstanceUnderInstanceNameFunc)GetProcAddress(hWpr, "WPRCCreateInstanceUnderInstanceName");

    if (FAILED(pFunc(NULL, CLSID_CProfileCollection, NULL, CLSCTX_INPROC_SERVER, IID_IProfileCollection2, (void**)&m_activeProfiles))) return false;

    ComPtr<IProfile2> pProfile;
    if (FAILED(pFunc(NULL, CLSID_CProfile, NULL, CLSCTX_INPROC_SERVER, IID_IProfile2, (void**)&pProfile))) return false;

    std::wstring pLevel = profileLevel.empty() ? L"Verbose" : profileLevel;
    std::wstring fullProfileName = pName + L"." + pLevel + L".Memory";

    BSTR bstrPid = SysAllocString(fullProfileName.c_str());
    BSTR bstrEmpty = SysAllocString(L"");
    HRESULT hr = pProfile->LoadFromFile(bstrPid, bstrEmpty);
    if (FAILED(hr)) {
        fullProfileName = pName + L"." + pLevel + L".File";
        SysFreeString(bstrPid);
        bstrPid = SysAllocString(fullProfileName.c_str());
        hr = pProfile->LoadFromFile(bstrPid, bstrEmpty);
    }
    SysFreeString(bstrPid);
    SysFreeString(bstrEmpty);

    if (SUCCEEDED(hr)) {
        if (SUCCEEDED(m_activeProfiles->Add(pProfile.Get(), VARIANT_TRUE))) {
            if (SUCCEEDED(m_controlManager->Start(m_activeProfiles.Get()))) {
                m_isRecording = true;
                return true;
            }
        }
    }

    return false;
}

bool PerfEngine::StopTrace(const std::wstring& etlFileName) {
    wchar_t fullPath[MAX_PATH];
    if (!GetFullPathNameW(etlFileName.c_str(), MAX_PATH, fullPath, NULL)) {
        wcscpy_s(fullPath, etlFileName.c_str());
    }

    // Ensure the directory exists
    wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR];
    _wsplitpath_s(fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
    std::wstring dirPath = std::wstring(drive) + dir;
    if (!dirPath.empty()) CreateDirectoryW(dirPath.c_str(), NULL);

    // 1. Try wpr.exe first as it handles merging and session cleanup better
    std::wstring cmdStop = L"wpr -stop \"" + std::wstring(fullPath) + L"\"";
    if (_wsystem(cmdStop.c_str()) == 0) {
        m_isRecording = false;
        if (m_activeProfiles) m_activeProfiles.Reset();
        return true;
    }

    // 2. Fallback to API
    if (!m_controlManager) return false;

    if (!m_activeProfiles) {
        m_controlManager->Query(&m_activeProfiles, VARIANT_TRUE);
    }
    if (!m_activeProfiles) return false;

    HMODULE hWpr = GetModuleHandleW(L"WindowsPerformanceRecorderControl.dll");
    WPRCCreateInstanceUnderInstanceNameFunc pFunc = (WPRCCreateInstanceUnderInstanceNameFunc)GetProcAddress(hWpr, "WPRCCreateInstanceUnderInstanceName");

    ComPtr<ITraceMergeProperties> pMergeProps;
    if (SUCCEEDED(pFunc(NULL, CLSID_CTraceMergeProperties, NULL, CLSCTX_INPROC_SERVER, IID_ITraceMergeProperties, (void**)&pMergeProps))) {
        BSTR bstrFile = SysAllocString(fullPath);
        HRESULT hr = m_controlManager->Stop(bstrFile, m_activeProfiles.Get(), pMergeProps.Get());
        SysFreeString(bstrFile);
        if (SUCCEEDED(hr)) {
            m_isRecording = false;
            m_activeProfiles.Reset();
            return true;
        }
    }

    return false;
}

} // namespace CoreEngine
