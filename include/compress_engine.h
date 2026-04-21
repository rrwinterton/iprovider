#pragma once

#include <string>

namespace CoreEngine {

// Class representing the file-mapped compression logic
class FileMappedCompressor {
 public:
  FileMappedCompressor(const std::wstring& inputFilePath,
                       const std::wstring& outputFilePath,
                       const std::string& archiveName);
  ~FileMappedCompressor() = default;

  bool Execute();

 private:
  std::wstring m_inputFilePath;
  std::wstring m_outputFilePath;
  std::string m_archiveName;
};

// Main engine class exposed by compress_engine.h
class CompressEngine {
 public:
  /**
   * @brief Structure to hold CompressEngine configuration.
   */
  struct Config {
    std::wstring inputFilePath;
    std::wstring outputFilePath;
    std::string archiveName;
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

  bool CompressFileMapped(const std::wstring& inputFilePath,
                          const std::wstring& outputFilePath,
                          const std::string& archiveName);
};
}  // namespace CoreEngine
