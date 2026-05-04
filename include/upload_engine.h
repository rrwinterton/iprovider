#pragma once
#include <string>

namespace CoreEngine {

class UploadEngine {
public:
    struct Config {
        bool doUpload = false;
        std::string serverLocation;
        std::string serverUrl;
        std::string uploadFile;
        std::string filePrefix;
    };

    static Config UploadEngineConfig(int argc, char** argv);

    UploadEngine();
    ~UploadEngine();

    void SetServerConfig(const std::string& location, const std::string& url);
    void SetUploadPrefix(const std::string& prefix);
    bool UploadFile(const std::string& filePath);

private:
    std::string m_serverLocation = "rrwinterton.com";
    std::string m_serverUrl = "/ireporter/ireporter.cgi/upload";
    std::string m_filePrefix = "iprovider";
};

} // namespace CoreEngine
