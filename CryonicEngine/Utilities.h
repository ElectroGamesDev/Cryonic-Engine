#pragma once

#include <filesystem>
#include <json.hpp>

namespace Utilities
{
    void OpenPathInExplorer(std::filesystem::path path);
    std::string SelectFolderDialog(const std::filesystem::path& projectPath);
    void HideFile(std::filesystem::path path);
    int GetNumberOfCores();
    std::filesystem::path CreateUniqueFile(std::filesystem::path path, std::string name, std::string extension);
    nlohmann::json GetExposedVariables(std::filesystem::path path);
};