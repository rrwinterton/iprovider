#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <CLI/CLI.hpp>

// Explicitly link the WinHTTP library
#pragma comment(lib, "winhttp.lib")
#include "upload_engine.h"

namespace CoreEngine {

UploadEngine::Config UploadEngine::UploadEngineConfig(int argc, char** argv) {
    CLI::App app{"Upload Engine Command Line Interface"};

    UploadEngine::Config config;

    app.add_flag("--upload", config.doUpload, "Identify upload operation");

    app.add_option("-l,--upload-location", config.serverLocation, "Server location (e.g., example.com)")
       ->required();

    app.add_option("-u,--upload-url", config.serverUrl, "Server URL path (e.g., /upload)")
       ->required();

    app.add_option("-f,--upload-file", config.uploadFile, "Path to the file to upload")
       ->required()
       ->check(CLI::ExistingFile);

    app.add_option("-p,--upload-prefix", config.filePrefix, "Prefix for the uploaded file name");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        throw;
    }

    return config;
}

UploadEngine::UploadEngine() {}
UploadEngine::~UploadEngine() {}

void UploadEngine::SetServerConfig(const std::string& location, const std::string& url) {
    m_serverLocation = location;
    m_serverUrl = url;
    std::cout << "[UploadEngine] Server config set: " << location << " (" << url << ")" << std::endl;
}

void UploadEngine::SetUploadPrefix(const std::string& prefix) {
    m_filePrefix = prefix;
}

std::wstring ToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    struct tm timeinfo;
    if (localtime_s(&timeinfo, &in_time_t) == 0) {
        ss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");
    } else {
        ss << "unknown_time";
    }
    return ss.str();
}

bool UploadEngine::UploadFile(const std::string& filePath) {
    if (m_serverUrl.empty()) {
        std::cerr << "[UploadEngine] Error: Server URL not set." << std::endl;
        return false;
    }

    // Extract filename and create prefixed/timestamped version
    std::string originalFilename = std::filesystem::path(filePath).filename().string();
    std::string timestamp = GetTimestamp();
    std::string remoteFilename = m_filePrefix + "_" + timestamp + "_" + originalFilename;

    std::cout << "[UploadEngine] Uploading file: " << filePath << " as " << remoteFilename << " to " << m_serverUrl << "..." << std::endl;
    
    bool bResults = false;
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

    // 1. Open the file to determine size and prepare for reading
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open local file: " << filePath << std::endl;
        return false;
    }
    
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 2. Initialize WinHTTP Session
    hSession = WinHttpOpen(L"WinHTTP Upload Client/1.0",  
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME, 
                           WINHTTP_NO_PROXY_BYPASS, 0);

    // 3. Connect to the Server (HTTPS port)
    if (hSession) {
        std::wstring wLocation = ToWString(m_serverLocation);
        hConnect = WinHttpConnect(hSession, wLocation.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    }

    // 4. Create the POST Request
    if (hConnect) {
        std::wstring wUrl = ToWString(m_serverUrl);
        hRequest = WinHttpOpenRequest(hConnect, L"POST", wUrl.c_str(),
                                      NULL, WINHTTP_NO_REFERER, 
                                      WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                      WINHTTP_FLAG_SECURE);
    }

    // 5. Send the Request
    if (hRequest) {
        std::wstring headers = L"Content-Type: application/octet-stream\r\n";
        headers += L"X-Original-Filename: " + ToWString(remoteFilename) + L"\r\n";
        
        bResults = WinHttpSendRequest(hRequest,
                                      headers.c_str(), (DWORD)headers.length(),
                                      WINHTTP_NO_REQUEST_DATA, 0, 
                                      (DWORD)fileSize, 0);
    }

    // 6. Write Data in Chunks
    if (bResults) {
        const size_t bufferSize = 8192; // 8KB chunk size
        char buffer[bufferSize];
        DWORD bytesWritten = 0;

        while (file.read(buffer, bufferSize) || file.gcount() > 0) {
            if (!WinHttpWriteData(hRequest, buffer, (DWORD)file.gcount(), &bytesWritten)) {
                std::cerr << "Error writing data: " << GetLastError() << std::endl;
                bResults = false;
                break;
            }
        }
    }

    // 7. Await the Server Response
    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    // 8. Output HTTP Status Code for debugging
    if (bResults) {
        DWORD dwStatusCode = 0;
        DWORD dwSize = sizeof(dwStatusCode);
        WinHttpQueryHeaders(hRequest, 
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                            WINHTTP_HEADER_NAME_BY_INDEX, 
                            &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
        
        std::cout << "Upload complete. Server HTTP Status: " << dwStatusCode << std::endl;
        
        bResults = (dwStatusCode >= 200 && dwStatusCode < 300);
    } else {
        DWORD err = GetLastError();
        std::cerr << "WinHTTP Error: " << err;
        if (err == 12175) std::cerr << " (SECURE_FAILURE - Likely SSL/TLS issue)";
        std::cerr << std::endl;
    }

    // 9. Cleanup handles
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return bResults;
}

} // namespace CoreEngine
