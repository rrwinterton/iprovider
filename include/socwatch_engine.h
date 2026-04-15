#pragma once

#include <string>

namespace CoreEngine {

    /**
     * @brief Class responsible for executing the SocWatch tool silently.
     */
    class SocWatchEngine {
    public:
        /**
         * @brief Executes the SocWatch process and returns status information.
         * @return std::string status/error info from the execution.
         */
        const char* Run();

    private:
        std::string m_lastResult;
    };

} // namespace CoreEngine
