#include "compress_engine.h"
#include <windows.h>
#include <compressapi.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "Cabinet.lib")

namespace CoreEngine {

    // Helper RAII class for memory mapping
    class MemoryMappedFile {
    public:
        MemoryMappedFile(const std::wstring& filePath)
            : m_hFile(INVALID_HANDLE_VALUE), m_hMapFile(NULL), m_pMapAddress(NULL), m_fileSize(0) {
            
            m_hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (m_hFile == INVALID_HANDLE_VALUE) return;

            LARGE_INTEGER li;
            if (!GetFileSizeEx(m_hFile, &li) || li.QuadPart == 0) return;
            m_fileSize = static_cast<size_t>(li.QuadPart);

            m_hMapFile = CreateFileMappingW(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
            if (m_hMapFile == NULL) return;

            m_pMapAddress = MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, 0);
        }

        ~MemoryMappedFile() {
            if (m_pMapAddress) UnmapViewOfFile(m_pMapAddress);
            if (m_hMapFile) CloseHandle(m_hMapFile);
            if (m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
        }

        bool IsValid() const { return m_pMapAddress != NULL; }
        const void* GetData() const { return m_pMapAddress; }
        size_t GetSize() const { return m_fileSize; }

    private:
        HANDLE m_hFile;
        HANDLE m_hMapFile;
        void* m_pMapAddress;
        size_t m_fileSize;
    };

    // Helper RAII class for compressor handle
    class CompressorHandle {
    public:
        CompressorHandle(DWORD algorithm) : m_handle(NULL) {
            CreateCompressor(algorithm, NULL, &m_handle);
        }
        ~CompressorHandle() {
            if (m_handle) CloseCompressor(m_handle);
        }
        COMPRESSOR_HANDLE Get() const { return m_handle; }
        bool IsValid() const { return m_handle != NULL; }

    private:
        COMPRESSOR_HANDLE m_handle;
    };

    // FileMappedCompressor implementation
    FileMappedCompressor::FileMappedCompressor(const std::wstring& inputFilePath, const std::wstring& outputFilePath)
        : m_inputFilePath(inputFilePath), m_outputFilePath(outputFilePath) {}

    bool FileMappedCompressor::Execute() {
        MemoryMappedFile input(m_inputFilePath);
        if (!input.IsValid()) {
            std::cerr << "Failed to map input file.\n";
            return false;
        }

        CompressorHandle compressor(COMPRESS_ALGORITHM_MSZIP);
        if (!compressor.IsValid()) {
            std::cerr << "Failed to create compressor.\n";
            return false;
        }

        SIZE_T compressedDataSize = 0;
        // Query required output buffer size
        if (!Compress(compressor.Get(), input.GetData(), input.GetSize(), NULL, 0, &compressedDataSize)) {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
                std::cerr << "Failed to query compressed size.\n";
                return false;
            }
        }

        std::vector<BYTE> outBuffer(compressedDataSize);
        if (!Compress(compressor.Get(), input.GetData(), input.GetSize(), outBuffer.data(), outBuffer.size(), &compressedDataSize)) {
            std::cerr << "Compression failed.\n";
            return false;
        }

        HANDLE hFileOut = CreateFileW(m_outputFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFileOut == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create output file.\n";
            return false;
        }

        DWORD bytesWritten = 0;
        BOOL success = WriteFile(hFileOut, outBuffer.data(), static_cast<DWORD>(compressedDataSize), &bytesWritten, NULL);
        CloseHandle(hFileOut);

        if (!success) {
            std::cerr << "Failed to write to output file.\n";
            return false;
        }

        std::cout << "Successfully compressed via memory map.\n";
        return true;
    }

    // CompressEngine implementation
    bool CompressEngine::CompressFileMapped(const std::wstring& inputFilePath, const std::wstring& outputFilePath) {
        FileMappedCompressor compressor(inputFilePath, outputFilePath);
        return compressor.Execute();
    }
}
