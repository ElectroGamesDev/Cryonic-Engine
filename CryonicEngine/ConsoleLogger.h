#pragma once

#include <string>

class ConsoleLogger
{
public:
    static void InfoLog(const std::string& message, bool devMessage = true);
    static void WarningLog(const std::string& message, bool devMessage = true);
    static void ErrorLog(const std::string& message, bool devMessage = true);

    enum class ConsoleLogType {
        WARNING,
        INFO,
        ERROR_
    };
    struct ConsoleLog {
        std::string message;
        ConsoleLogType type;
    };
};
