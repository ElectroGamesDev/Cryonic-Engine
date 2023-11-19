#pragma once

#include <filesystem>

class Utilities {
public:
    static void OpenPathInExplorer(std::filesystem::path path);
    static std::string SelectFolderDialog(const std::filesystem::path& projectPath);
    static void HideFile(std::filesystem::path path);
};