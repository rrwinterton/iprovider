#include "compress_engine.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "miniz.h"

namespace CoreEngine {

// Helper RAII class for memory mapping
class MemoryMappedFile {
 public:
  MemoryMappedFile(const std::wstring& filePath)
      : m_hFile(INVALID_HANDLE_VALUE),
        m_hMapFile(NULL),
        m_pMapAddress(NULL),
        m_fileSize(0) {
    m_hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE)
      return;

    LARGE_INTEGER li;
    if (!GetFileSizeEx(m_hFile, &li) || li.QuadPart == 0)
      return;
    m_fileSize = static_cast<size_t>(li.QuadPart);

    m_hMapFile = CreateFileMappingW(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (m_hMapFile == NULL)
      return;

    m_pMapAddress = MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, 0);
  }

  ~MemoryMappedFile() {
    if (m_pMapAddress)
      UnmapViewOfFile(m_pMapAddress);
    if (m_hMapFile)
      CloseHandle(m_hMapFile);
    if (m_hFile != INVALID_HANDLE_VALUE)
      CloseHandle(m_hFile);
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

// FileMappedCompressor implementation
FileMappedCompressor::FileMappedCompressor(const std::wstring& inputFilePath,
                                           const std::wstring& outputFilePath,
                                           const std::string& archiveName)
    : m_inputFilePath(inputFilePath),
      m_outputFilePath(outputFilePath),
      m_archiveName(archiveName) {}

bool FileMappedCompressor::Execute() {
  MemoryMappedFile input(m_inputFilePath);
  if (!input.IsValid()) {
    std::cerr << "Failed to map input file.\n";
    return false;
  }

  // Convert wstring output path to string for miniz (which uses char*)
  std::string outputFilePathStr(m_outputFilePath.begin(),
                                m_outputFilePath.end());

  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_writer_init_file(&zip_archive, outputFilePathStr.c_str(), 0)) {
    std::cerr << "Failed to initialize ZIP writer.\n";
    return false;
  }

  if (!mz_zip_writer_add_mem(&zip_archive, m_archiveName.c_str(),
                             input.GetData(), input.GetSize(),
                             MZ_DEFAULT_COMPRESSION)) {
    std::cerr << "Failed to add file to ZIP.\n";
    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);
    return false;
  }

  if (!mz_zip_writer_finalize_archive(&zip_archive)) {
    std::cerr << "Failed to finalize ZIP archive.\n";
    mz_zip_writer_end(&zip_archive);
    return false;
  }

  mz_zip_writer_end(&zip_archive);

  std::cout
      << "Successfully created standard ZIP file via single-file miniz.\n";
  return true;
}

// CompressEngine implementation
bool CompressEngine::CompressFileMapped(const std::wstring& inputFilePath,
                                        const std::wstring& outputFilePath,
                                        const std::string& archiveName) {
  FileMappedCompressor compressor(inputFilePath, outputFilePath, archiveName);
  return compressor.Execute();
}
}  // namespace CoreEngine
