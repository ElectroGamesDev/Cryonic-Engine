#include "ConsoleLogger.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

bool showDebugMessages = true;

void ConsoleLogger::InfoLog(const std::string& message, bool devMessage)
{
    if (devMessage && !showDebugMessages) return;
    std::cout << "[INFO] " << message << std::endl;
}

void ConsoleLogger::WarningLog(const std::string& message, bool devMessage)
{
    if (devMessage && !showDebugMessages) return;
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 14); // Set color to yellow
#endif
    std::cout << "[WARNING] " << message << std::endl;
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, 15); // Reset color
#endif
}

void ConsoleLogger::ErrorLog(const std::string& message, bool devMessage)
{
    if (devMessage && !showDebugMessages) return;
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12); // Set color to red
#endif
    std::cerr << "[ERROR] " << message << std::endl;
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, 15); // Reset color
#endif
}
