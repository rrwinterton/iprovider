#include "api_exports.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <vector>

void test_math_engine() {
    std::cout << "Testing MathEngine..." << std::endl;
    EngineHandle handle = CreateMathEngine(10);
    assert(handle != nullptr);

    int result = MathEngine_Calculate(handle, 5);
    std::cout << "MathEngine result: " << result << " (expected 50)" << std::endl;
    assert(result == 50);

    DestroyMathEngine(handle);
    std::cout << "MathEngine test passed!" << std::endl;
}

void test_compress_engine() {
    std::cout << "Testing CompressEngine (Multi-file)..." << std::endl;
    
    // Create dummy files
    {
        std::ofstream f1("test1.txt");
        f1 << "Content of file 1";
        std::ofstream f2("test2.txt");
        f2 << "Content of file 2";
    }

    EngineHandle handle = CreateCompressEngine();
    assert(handle != nullptr);

    const wchar_t* inputs[] = { L"test1.txt", L"test2.txt" };
    const char* names[] = { "inner1.txt", "inner2.txt" };
    
    bool success = CompressEngine_CompressFileMapped(handle, inputs, 2, L"test_multi.zip", names, 2);
    std::cout << "CompressEngine success: " << (success ? "YES" : "NO") << std::endl;
    assert(success);

    DestroyCompressEngine(handle);
    std::cout << "CompressEngine test passed!" << std::endl;
}

void test_parse_config() {
    std::cout << "Testing CompressEngine_ParseConfig..." << std::endl;
    
    const char* argv[] = { "iprovider.dll", "-i", "file1.txt", "-i", "file2.txt", "-o", "out.zip", "-a", "arch1.txt", "-a", "arch2.txt" };
    int argc = sizeof(argv) / sizeof(argv[0]);

    CompressEngine_Config config;
    bool success = CompressEngine_ParseConfig(argc, (char**)argv, &config);
    
    assert(success);
    assert(config.inputFileCount == 2);
    assert(config.archiveNameCount == 2);
    std::cout << "Parsed input 1: " << config.inputFilePaths[0] << std::endl;
    std::cout << "Parsed input 2: " << config.inputFilePaths[1] << std::endl;
    std::cout << "Parsed output: " << config.outputFilePath << std::endl;

    CompressEngine_FreeConfig(&config);
    std::cout << "ParseConfig test passed!" << std::endl;

    std::cout << "\nTesting UploadEngine_ParseConfig..." << std::endl;
    // Create a dummy file to satisfy CLI::ExistingFile
    {
        std::ofstream f("upload_test.txt");
        f << "test data";
    }

    const char* upload_argv[] = { "iprovider.dll", "--upload", "--upload-location", "example.com", "--upload-url", "/api/upload", "--upload-file", "upload_test.txt" };
    int upload_argc = sizeof(upload_argv) / sizeof(upload_argv[0]);

    UploadEngine_Config upload_config;
    success = UploadEngine_ParseConfig(upload_argc, (char**)upload_argv, &upload_config);

    assert(success);
    assert(upload_config.doUpload == true);
    assert(std::string(upload_config.serverLocation) == "example.com");
    assert(std::string(upload_config.serverUrl) == "/api/upload");
    assert(std::string(upload_config.uploadFile) == "upload_test.txt");

    std::cout << "UploadEngine ParseConfig test passed!" << std::endl;
}

int main() {
    try {
        test_math_engine();
        test_compress_engine();
        test_parse_config();
        std::cout << "\nAll C-API tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
