#pragma once

#include <filesystem>
#include <json.hpp>
#include <vector>

namespace Utilities
{
    void OpenPathInExplorer(std::filesystem::path path);
    std::string SelectFolderDialog(const std::filesystem::path& projectPath);
    void HideFile(std::filesystem::path path);
    int GetNumberOfCores();
    std::filesystem::path CreateUniqueFile(std::filesystem::path path, std::string name, std::string extension);
    nlohmann::json GetExposedVariables(std::filesystem::path path);
    std::vector<std::string> GetGltfAnimationNames(std::filesystem::path path);
    bool CreateDataFile(std::filesystem::path path);
    bool ImportFile(std::filesystem::path filePath, std::filesystem::path importPath);
    void TerminateProcess(int dwProcessId, int uExitCode); // A TerminateProcess function that actually works unlike Window's TermianteProcess()
};