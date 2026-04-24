#pragma once

#include <string>

namespace CoreEngine {

/**
 * @brief Class responsible for executing the SocWatch tool silently.
 */
class SocWatchEngine {
 public:
  /**
   * @brief Structure to hold SocWatch engine configuration.
   */
  struct Config {
    bool doSocwatch = false;
    unsigned int duration = 0;
    std::string outputFileName;
  };

  /**
   * @brief Parses command line arguments to generate a configuration.
   * @param argc Argument count.
   * @param argv Argument vector.
   * @return Config The parsed configuration.
   */
  static Config SocWatchEngineConfig(int argc, char** argv);

  /**
   * @brief Executes the SocWatch process and returns status information.
   * @param durationInSeconds The time in seconds for the SocWatch run.
   * @param outputFileName The name of the output CSV file (without extension).
   * @return std::string status/error info from the execution.
   */
  const char* Run(unsigned int durationInSeconds, const std::string& outputFileName);

 private:
  bool IsFileReady(const std::wstring& filePath);
  std::string m_lastResult;
};

}  // namespace CoreEngine
