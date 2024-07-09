#pragma once

#include <string>
#include <vector>
#include <utility>

class ConsoleLogger
{
public:
    static void InfoLog(const std::string& message, bool devMessage = true);
    static void WarningLog(const std::string& message, bool devMessage = true);
    static void ErrorLog(const std::string& message, bool devMessage = true);

    enum class ConsoleLogType {
        INFO,
        WARNING,
        ERROR_
    };
    struct ConsoleLog {
        std::string message;
        ConsoleLogType type;
    };

    static std::vector<std::pair<std::string, ConsoleLogType>> logs;
    static bool showDebugMessages;
};
