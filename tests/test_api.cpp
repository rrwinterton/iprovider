#include <iostream>
#include <cassert>
#include <vector>
#include "api_exports.h"

void test_math_engine() {
    std::cout << "Testing MathEngine..." << std::endl;
    EngineHandle handle = CreateMathEngine(10);
    assert(handle != nullptr);
    int result = MathEngine_Calculate(handle, 5);
    assert(result == 50);
    DestroyMathEngine(handle);
    std::cout << "MathEngine test passed!" << std::endl;
}

void test_socwatch_config() {
    std::cout << "Testing SocwatchEngine_ParseConfig..." << std::endl;
    const char* argv[] = {"test.exe", "-d", "10", "-o", "report"};
    SocwatchEngine_Config config;
    bool success = SocwatchEngine_ParseConfig(5, (char**)argv, &config);
    assert(success);
    assert(config.duration == 10);
    assert(std::string(config.outputFileName) == "report");
    std::cout << "SocwatchEngine_ParseConfig test passed!" << std::endl;
}

void test_compress_config() {
    std::cout << "Testing CompressEngine_ParseConfig..." << std::endl;
    const char* argv[] = {"test.exe", "-i", "input.txt", "-o", "output.zip", "-a", "arch"};
    CompressEngine_Config config;
    bool success = CompressEngine_ParseConfig(7, (char**)argv, &config);
    assert(success);
    assert(std::string(config.inputFilePath) == "input.txt");
    assert(std::string(config.outputFilePath) == "output.zip");
    assert(std::string(config.archiveName) == "arch");
    std::cout << "CompressEngine_ParseConfig test passed!" << std::endl;
}

void test_perf_config() {
    std::cout << "Testing PerfEngine_ParseConfig..." << std::endl;
    const char* argv[] = {"test.exe", "StartTrace", "-n", "CPU", "-l", "Light", "-d", "5", "-f", "trace.etl"};
    PerfEngine_Config config;
    bool success = PerfEngine_ParseConfig(10, (char**)argv, &config);
    assert(success);
    assert(config.isStartTrace);
    assert(std::string(config.profileName) == "CPU");
    assert(config.duration == 5);
    assert(std::string(config.etlFileName) == "trace.etl");
    std::cout << "PerfEngine_ParseConfig test passed!" << std::endl;
}

int main() {
    try {
        test_math_engine();
        test_socwatch_config();
        test_compress_config();
        test_perf_config();
        std::cout << "All API tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
