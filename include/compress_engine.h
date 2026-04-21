#pragma once

#include <string>
#include <vector>

namespace CoreEngine {

// Class representing the file-mapped compression logic
class FileMappedCompressor {
 public:
  FileMappedCompressor(const std::vector<std::wstring>& inputFilePaths,
                       const std::wstring& outputFilePath,
                       const std::vector<std::string>& archiveNames);
  ~FileMappedCompressor() = default;

  bool Execute();

 private:
  std::vector<std::wstring> m_inputFilePaths;
  std::wstring m_outputFilePath;
  std::vector<std::string> m_archiveNames;
};

// Main engine class exposed by compress_engine.h
class CompressEngine {
 public:
  /**
   * @brief Structure to hold CompressEngine configuration.
   */
  struct Config {
    std::vector<std::wstring> inputFilePaths;
    std::wstring outputFilePath;
    std::vector<std::string> archiveNames;
  };

  /**
   * @brief Parses command line arguments to generate a configuration.
   * @param argc Argument count.
   * @param argv Argument vector.
   * @return Config The parsed configuration.
   */
  static Config CompressEngineConfig(int argc, char** argv);

  CompressEngine() = default;
  ~CompressEngine() = default;

  bool CompressFileMapped(const std::vector<std::wstring>& inputFilePaths,
                          const std::wstring& outputFilePath,
                          const std::vector<std::string>& archiveNames);
};
}  // namespace CoreEngine
