#include "compress_engine.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <CLI/CLI.hpp>
#include "miniz.h"

namespace CoreEngine {

CompressEngine::Config CompressEngine::CompressEngineConfig(int argc, char** argv) {
    CLI::App app{"Compress Engine Command Line Interface"};

    CompressEngine::Config config;
    std::vector<std::string> inputPaths;
    std::string outputPath;

    app.add_option("-i,--input", inputPaths, "Input file paths")
       ->required();

    app.add_option("-o,--output", outputPath, "Output archive file path")
       ->required();

    app.add_option("-a,--archiveName", config.archiveNames, "Names of the files inside the archive")
       ->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        throw;
    }

    for (const auto& path : inputPaths) {
        config.inputFilePaths.push_back(std::wstring(path.begin(), path.end()));
    }
    config.outputFilePath = std::wstring(outputPath.begin(), outputPath.end());

    return config;
}

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
FileMappedCompressor::FileMappedCompressor(const std::vector<std::wstring>& inputFilePaths,
                                           const std::wstring& outputFilePath,
                                           const std::vector<std::string>& archiveNames)
    : m_inputFilePaths(inputFilePaths),
      m_outputFilePath(outputFilePath),
      m_archiveNames(archiveNames) {}

bool FileMappedCompressor::Execute() {
  if (m_inputFilePaths.size() != m_archiveNames.size()) {
#ifdef DEBUG_PRINTS
    std::cerr << "Mismatch between input files and archive names count.\n";
#endif
    return false;
  }

  // Convert wstring output path to string for miniz (which uses char*)
  std::string outputFilePathStr(m_outputFilePath.begin(),
                                m_outputFilePath.end());

  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_writer_init_file(&zip_archive, outputFilePathStr.c_str(), 0)) {
#ifdef DEBUG_PRINTS
    std::cerr << "Failed to initialize ZIP writer.\n";
#endif
    return false;
  }

  for (size_t i = 0; i < m_inputFilePaths.size(); ++i) {
    MemoryMappedFile input(m_inputFilePaths[i]);
    if (!input.IsValid()) {
#ifdef DEBUG_PRINTS
      std::wcerr << L"Failed to map input file: " << m_inputFilePaths[i] << L"\n";
#endif
      continue;
    }

    if (!mz_zip_writer_add_mem(&zip_archive, m_archiveNames[i].c_str(),
                               input.GetData(), input.GetSize(),
                               MZ_DEFAULT_COMPRESSION)) {
#ifdef DEBUG_PRINTS
      std::cerr << "Failed to add file to ZIP: " << m_archiveNames[i] << "\n";
#endif
      mz_zip_writer_finalize_archive(&zip_archive);
      mz_zip_writer_end(&zip_archive);
      return false;
    }
  }

  if (!mz_zip_writer_finalize_archive(&zip_archive)) {
#ifdef DEBUG_PRINTS
    std::cerr << "Failed to finalize ZIP archive.\n";
#endif
    mz_zip_writer_end(&zip_archive);
    return false;
  }

  mz_zip_writer_end(&zip_archive);

#ifdef DEBUG_PRINTS
  std::cout
      << "Successfully created ZIP archive with multiple files.\n";
#endif
  return true;
}

// CompressEngine implementation
bool CompressEngine::CompressFileMapped(const std::vector<std::wstring>& inputFilePaths,
                                        const std::wstring& outputFilePath,
                                        const std::vector<std::string>& archiveNames) {
  FileMappedCompressor compressor(inputFilePaths, outputFilePath, archiveNames);
  return compressor.Execute();
}
}  // namespace CoreEngine
