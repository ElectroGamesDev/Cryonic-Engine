#pragma once

#include <filesystem>
#include <json.hpp>

class Utilities {
public:
    static void OpenPathInExplorer(std::filesystem::path path);
    static std::string SelectFolderDialog(const std::filesystem::path& projectPath);
    static void HideFile(std::filesystem::path path);
    static int GetNumberOfCores();
    static std::filesystem::path CreateUniqueFile(std::filesystem::path path, std::string name, std::string extension);
    static nlohmann::json GetExposedVariables(std::filesystem::path path);
};