#define _CRT_SECURE_NO_WARNINGS
#include "api_exports.h"
#include "math_engine.h"
#include "compress_engine.h"
#include "socwatch_engine.h"
#include "perf_engine.h"
#include "upload_engine.h"
#include <cstring>
#include <string>
#include <vector>

// --- MathEngine Implementation ---

EngineHandle CreateMathEngine(int multiplier) {
    return static_cast<EngineHandle>(new CoreEngine::MathEngine(multiplier));
}

int MathEngine_Calculate(EngineHandle handle, int input) {
    if (handle) {
        return static_cast<CoreEngine::MathEngine*>(handle)->Calculate(input);
    }
    return 0;
}

void DestroyMathEngine(EngineHandle handle) {
    if (handle) {
        delete static_cast<CoreEngine::MathEngine*>(handle);
    }
}

// --- CompressEngine Implementation ---

bool CompressEngine_ParseConfig(int argc, char** argv, CompressEngine_Config* outConfig) {
    if (!outConfig) return false;
    try {
        auto config = CoreEngine::CompressEngine::CompressEngineConfig(argc, argv);
        
        outConfig->doCompress = config.doCompress;

        // Convert output path
        std::string outputStr(config.outputFilePath.begin(), config.outputFilePath.end());
        std::strncpy(outConfig->outputFilePath, outputStr.c_str(), sizeof(outConfig->outputFilePath) - 1);
        outConfig->outputFilePath[sizeof(outConfig->outputFilePath) - 1] = '\0';

        // Allocate and copy input file paths
        outConfig->inputFileCount = static_cast<int>(config.inputFilePaths.size());
        outConfig->inputFilePaths = new char*[outConfig->inputFileCount];
        for (int i = 0; i < outConfig->inputFileCount; ++i) {
            std::string path(config.inputFilePaths[i].begin(), config.inputFilePaths[i].end());
            outConfig->inputFilePaths[i] = new char[path.length() + 1];
            std::strcpy(outConfig->inputFilePaths[i], path.c_str());
        }

        // Allocate and copy archive names
        outConfig->archiveNameCount = static_cast<int>(config.archiveNames.size());
        outConfig->archiveNames = new char*[outConfig->archiveNameCount];
        for (int i = 0; i < outConfig->archiveNameCount; ++i) {
            outConfig->archiveNames[i] = new char[config.archiveNames[i].length() + 1];
            std::strcpy(outConfig->archiveNames[i], config.archiveNames[i].c_str());
        }

        return true;
    } catch (...) {
        return false;
    }
}

void CompressEngine_FreeConfig(CompressEngine_Config* config) {
    if (!config) return;
    if (config->inputFilePaths) {
        for (int i = 0; i < config->inputFileCount; ++i) {
            delete[] config->inputFilePaths[i];
        }
        delete[] config->inputFilePaths;
        config->inputFilePaths = nullptr;
    }
    if (config->archiveNames) {
        for (int i = 0; i < config->archiveNameCount; ++i) {
            delete[] config->archiveNames[i];
        }
        delete[] config->archiveNames;
        config->archiveNames = nullptr;
    }
}

EngineHandle CreateCompressEngine() {
    return static_cast<EngineHandle>(new CoreEngine::CompressEngine());
}

bool CompressEngine_CompressFileMapped(EngineHandle handle,
                                      const wchar_t** inputFilePaths,
                                      int inputFileCount,
                                      const wchar_t* outputFilePath,
                                      const char** archiveNames,
                                      int archiveNameCount) {
    if (handle) {
        std::vector<std::wstring> inputs;
        for (int i = 0; i < inputFileCount; ++i) {
            inputs.push_back(inputFilePaths[i]);
        }
        std::vector<std::string> names;
        for (int i = 0; i < archiveNameCount; ++i) {
            names.push_back(archiveNames[i]);
        }
        return static_cast<CoreEngine::CompressEngine*>(handle)->CompressFileMapped(
            inputs, outputFilePath, names);
    }
    return false;
}

void DestroyCompressEngine(EngineHandle handle) {
    if (handle) {
        delete static_cast<CoreEngine::CompressEngine*>(handle);
    }
}

// --- SocwatchEngine Implementation ---

bool SocwatchEngine_ParseConfig(int argc, char** argv, SocwatchEngine_Config* outConfig) {
    if (!outConfig) return false;
    try {
        auto config = CoreEngine::SocWatchEngine::SocWatchEngineConfig(argc, argv);
        outConfig->doSocwatch = config.doSocwatch;
        outConfig->duration = config.duration;
        std::strncpy(outConfig->outputFileName, config.outputFileName.c_str(), sizeof(outConfig->outputFileName) - 1);
        outConfig->outputFileName[sizeof(outConfig->outputFileName) - 1] = '\0';
        return true;
    } catch (...) {
        return false;
    }
}

EngineHandle CreateSocwatchEngine() {
    return static_cast<EngineHandle>(new CoreEngine::SocWatchEngine());
}

const char* SocwatchEngine_Run(EngineHandle handle, unsigned int durationInSeconds, const char* outputFileName) {
    if (handle) {
        return static_cast<CoreEngine::SocWatchEngine*>(handle)->Run(durationInSeconds, outputFileName);
    }
    return nullptr;
}

void DestroySocwatchEngine(EngineHandle handle) {
    if (handle) {
        delete static_cast<CoreEngine::SocWatchEngine*>(handle);
    }
}

// --- PerfEngine Implementation ---

bool PerfEngine_ParseConfig(int argc, char** argv, PerfEngine_Config* outConfig) {
    if (!outConfig) return false;
    try {
        auto config = CoreEngine::PerfEngine::PerfEngineConfig(argc, argv);
        outConfig->perf = config.perf;
        outConfig->localOnly = config.localOnly;
        outConfig->isStartTrace = config.isStartTrace;
        std::strncpy(outConfig->profileName, config.profileName.c_str(), sizeof(outConfig->profileName) - 1);
        outConfig->profileName[sizeof(outConfig->profileName) - 1] = '\0';
        std::strncpy(outConfig->profileLevel, config.profileLevel.c_str(), sizeof(outConfig->profileLevel) - 1);
        outConfig->profileLevel[sizeof(outConfig->profileLevel) - 1] = '\0';
        outConfig->duration = config.duration;
        outConfig->isStopTrace = config.isStopTrace;
        std::strncpy(outConfig->etlFile, config.etlFile.c_str(), sizeof(outConfig->etlFile) - 1);
        outConfig->etlFile[sizeof(outConfig->etlFile) - 1] = '\0';
        return true;
    } catch (...) {
        return false;
    }
}

EngineHandle CreatePerfEngine() {
    return static_cast<EngineHandle>(new CoreEngine::PerfEngine());
}

bool PerfEngine_StartTrace(EngineHandle handle,
                           const wchar_t* profileName,
                           const wchar_t* profileLevel,
                           unsigned int duration,
                           const wchar_t* etlFileName) {
    if (handle) {
        return static_cast<CoreEngine::PerfEngine*>(handle)->StartTrace(profileName, profileLevel, duration, etlFileName);
    }
    return false;
}

bool PerfEngine_StopTrace(EngineHandle handle, const wchar_t* etlFileName) {
    if (handle) {
        return static_cast<CoreEngine::PerfEngine*>(handle)->StopTrace(etlFileName);
    }
    return false;
}

bool PerfEngine_IsRecording(EngineHandle handle) {
    if (handle) {
        return static_cast<CoreEngine::PerfEngine*>(handle)->IsRecording();
    }
    return false;
}

const char* PerfEngine_GetLastResult(EngineHandle handle) {
    if (handle) {
        return static_cast<CoreEngine::PerfEngine*>(handle)->GetLastResult();
    }
    return nullptr;
}

void DestroyPerfEngine(EngineHandle handle) {
    if (handle) {
        delete static_cast<CoreEngine::PerfEngine*>(handle);
    }
}

// --- UploadEngine Implementation ---

bool UploadEngine_ParseConfig(int argc, char** argv, UploadEngine_Config* outConfig) {
    if (!outConfig) return false;
    try {
        auto config = CoreEngine::UploadEngine::UploadEngineConfig(argc, argv);
        outConfig->doUpload = config.doUpload;
        std::strncpy(outConfig->serverLocation, config.serverLocation.c_str(), sizeof(outConfig->serverLocation) - 1);
        outConfig->serverLocation[sizeof(outConfig->serverLocation) - 1] = '\0';
        std::strncpy(outConfig->serverUrl, config.serverUrl.c_str(), sizeof(outConfig->serverUrl) - 1);
        outConfig->serverUrl[sizeof(outConfig->serverUrl) - 1] = '\0';
        std::strncpy(outConfig->uploadFile, config.uploadFile.c_str(), sizeof(outConfig->uploadFile) - 1);
        outConfig->uploadFile[sizeof(outConfig->uploadFile) - 1] = '\0';
        std::strncpy(outConfig->filePrefix, config.filePrefix.c_str(), sizeof(outConfig->filePrefix) - 1);
        outConfig->filePrefix[sizeof(outConfig->filePrefix) - 1] = '\0';
        return true;
    } catch (...) {
        return false;
    }
}

EngineHandle CreateUploadEngine() {
    return static_cast<EngineHandle>(new CoreEngine::UploadEngine());
}

void UploadEngine_SetServerConfig(EngineHandle handle, const char* location, const char* url) {
    if (handle && location && url) {
        static_cast<CoreEngine::UploadEngine*>(handle)->SetServerConfig(location, url);
    }
}

void UploadEngine_SetUploadPrefix(EngineHandle handle, const char* prefix) {
    if (handle && prefix) {
        static_cast<CoreEngine::UploadEngine*>(handle)->SetUploadPrefix(prefix);
    }
}

bool UploadEngine_UploadFile(EngineHandle handle, const char* filePath) {
    if (handle && filePath) {
        return static_cast<CoreEngine::UploadEngine*>(handle)->UploadFile(filePath);
    }
    return false;
}

void DestroyUploadEngine(EngineHandle handle) {
    if (handle) {
        delete static_cast<CoreEngine::UploadEngine*>(handle);
    }
}

const IProviderAPI* GetIProviderAPI() {
    static const IProviderAPI api = {
        CreateMathEngine,
        MathEngine_Calculate,
        DestroyMathEngine,
        CompressEngine_ParseConfig,
        CompressEngine_FreeConfig,
        CreateCompressEngine,
        CompressEngine_CompressFileMapped,
        DestroyCompressEngine,
        SocwatchEngine_ParseConfig,
        CreateSocwatchEngine,
        SocwatchEngine_Run,
        DestroySocwatchEngine,
        PerfEngine_ParseConfig,
        CreatePerfEngine,
        PerfEngine_StartTrace,
        PerfEngine_StopTrace,
        PerfEngine_IsRecording,
        PerfEngine_GetLastResult,
        DestroyPerfEngine,
        UploadEngine_ParseConfig,
        CreateUploadEngine,
        UploadEngine_SetServerConfig,
        UploadEngine_SetUploadPrefix,
        UploadEngine_UploadFile,
        DestroyUploadEngine
    };
    return &api;
}

