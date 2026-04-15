#pragma once

#include <string>

namespace CoreEngine {
    
    // Class representing the file-mapped compression logic
    class FileMappedCompressor {
    public:
        FileMappedCompressor(const std::wstring& inputFilePath, const std::wstring& outputFilePath);
        ~FileMappedCompressor() = default;

        bool Execute();

    private:
        std::wstring m_inputFilePath;
        std::wstring m_outputFilePath;
    };

    // Main engine class exposed by compress_engine.h
    class CompressEngine {
    public:
        CompressEngine() = default;
        ~CompressEngine() = default;

        bool CompressFileMapped(const std::wstring& inputFilePath, const std::wstring& outputFilePath);
    };
}
