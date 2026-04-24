#include <iostream>
#include <windows.h>
#include <vector>
#include "api_exports.h"

void test_math_engine(const IProviderAPI* api) {
    std::cout << "\n--- Testing MathEngine ---" << std::endl;
    EngineHandle math = api->CreateMathEngine(5);
    int result = api->MathEngine_Calculate(math, 10);
    std::cout << "MathEngine result (5 * 10): " << result << " (Expected: 50)" << std::endl;
    api->DestroyMathEngine(math);
}

void test_socwatch_engine(const IProviderAPI* api) {
    std::cout << "\n--- Testing SocwatchEngine CLI Parsing ---" << std::endl;
    const char* argv[] = { "iprovider.exe", "--socwatch", "--sw-duration", "5", "--sw-output", "rrw" };
    int argc = sizeof(argv) / sizeof(argv[0]);

    SocwatchEngine_Config config;
    if (api->SocwatchEngine_ParseConfig(argc, (char**)argv, &config)) {
        std::cout << "Parse Success!" << std::endl;
        std::cout << "  doSocwatch: " << (config.doSocwatch ? "True" : "False") << std::endl;
        std::cout << "  duration:   " << config.duration << std::endl;
        std::cout << "  output:     " << config.outputFileName << std::endl;
    } else {
        std::cerr << "Parse Failed for SocwatchEngine!" << std::endl;
    }
}

void test_compress_engine(const IProviderAPI* api) {
    std::cout << "\n--- Testing CompressEngine CLI Parsing ---" << std::endl;
    const char* argv[] = { 
        "iprovider.exe", "--compress", 
        "--compress-input", "test1.txt", 
        "--compress-input", "test2.txt", 
        "--compress-output", "out.zip", 
        "--compress-archive", "a1.txt", 
        "--compress-archive", "a2.txt" 
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    CompressEngine_Config config;
    if (api->CompressEngine_ParseConfig(argc, (char**)argv, &config)) {
        std::cout << "Parse Success!" << std::endl;
        std::cout << "  doCompress: " << (config.doCompress ? "True" : "False") << std::endl;
        std::cout << "  Input count: " << config.inputFileCount << std::endl;
        std::cout << "  Output:      " << config.outputFilePath << std::endl;
        api->CompressEngine_FreeConfig(&config);
    } else {
        std::cerr << "Parse Failed for CompressEngine!" << std::endl;
    }
}

void test_perf_engine(const IProviderAPI* api) {
    std::cout << "\n--- Testing PerfEngine CLI Parsing ---" << std::endl;
    const char* argv[] = { 
        "iprovider.exe", "StartTrace", 
        "--perf", "--localOnly", 
        "--profileName", "CPU", 
        "--profileLevel", "Verbose", 
        "--perf-duration", "10", 
        "--etlFile", "rrw.etl" 
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    PerfEngine_Config config;
    if (api->PerfEngine_ParseConfig(argc, (char**)argv, &config)) {
        std::cout << "Parse Success!" << std::endl;
        std::cout << "  perf:         " << (config.perf ? "True" : "False") << std::endl;
        std::cout << "  localOnly:    " << (config.localOnly ? "True" : "False") << std::endl;
        std::cout << "  profile:      " << config.profileName << std::endl;
        std::cout << "  level:        " << config.profileLevel << std::endl;
        std::cout << "  duration:     " << config.duration << std::endl;
        std::cout << "  etlFile:      " << config.etlFile << std::endl;
    } else {
        std::cerr << "Parse Failed for PerfEngine!" << std::endl;
    }
}

int main() {
    std::cout << "iprovider.exe - Engine Integration Test" << std::endl;

    HMODULE hLib = LoadLibraryA("iprovider.dll");
    if (!hLib) {
        std::cerr << "Failed to load iprovider.dll (Error: " << GetLastError() << ")" << std::endl;
        return 1;
    }

    typedef const IProviderAPI* (*GetIProviderAPIFunc)();
    GetIProviderAPIFunc getApi = (GetIProviderAPIFunc)GetProcAddress(hLib, "GetIProviderAPI");

    if (!getApi) {
        std::cerr << "Failed to find GetIProviderAPI" << std::endl;
        FreeLibrary(hLib);
        return 1;
    }

    const IProviderAPI* api = getApi();
    
    test_math_engine(api);
    test_socwatch_engine(api);
    test_compress_engine(api);
    test_perf_engine(api);

    std::cout << "\nAll dynamic tests completed!" << std::endl;
    FreeLibrary(hLib);
    return 0;
}
