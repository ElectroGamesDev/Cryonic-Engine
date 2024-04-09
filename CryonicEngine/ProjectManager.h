#pragma once

#include "ConsoleLogger.h"
//#include "CryonicEngine.h"
#include <filesystem>
#include <fstream>
#include "Scenes/SceneManager.h"
//#include "Editor.h"
//#include "miniz.h"
#include "Utilities.h"

enum Templates
{
    Blank3D,
    Blank2D,
    Sidescroller3D
};

struct TemplateData
{
    std::string name;
    std::string description;
    Templates _template;
};

struct ProjectData
{
    // General
    std::string name;
    std::string version;
    std::string author;
    std::filesystem::path path;
    TemplateData templateData;
    bool is3D;
    std::string iconPath;

    // Window Settings
    bool resizableWindow = false;
    int displayMode = 0; // 0 = borderless window, 1 = fullscreen, 2 = windowed
    Vector2 minimumResolution = { 100, 100 };
    Vector2 windowResolution = { 1920, 1080 };
    int maxFPS = 60;
    bool runInBackground = false; // Whether the game should run in the background
    bool vsync = false;

    // Graphics
    bool antialiasing = false; // Only MSAA 4x is support currently
    bool highDPI = true;

    // Physics
    Vector2 physicsTimeStep = {1, 60};
    int velocityIterations = 8;
    int positionIterations = 3;
};

class ProjectManager {
public:
    static void GenerateExposedVariablesFunctions(std::filesystem::path path);
    static void SaveProjectData(ProjectData projectData);
    static ProjectData LoadProjectData(std::filesystem::path projectPath);
    static int CreateProject(ProjectData projectData);
    static void CleanupBuildFolder(std::filesystem::path path);
    static void CopyApiFiles(std::filesystem::path source, std::filesystem::path destination);
    static void CopyAssetFiles(std::filesystem::path destination);
    //static bool ExtractZip(const char* zip_path, const char* dest_folder);
    static bool SetGameSettings(std::filesystem::path gameFile);
    static void BackupCMakeFiles(std::filesystem::path buildPath, std::filesystem::path backupPath);
    static void RestoreCMakeFiles(std::filesystem::path buildPath, std::filesystem::path backupPath);
    static void BuildToWindows(ProjectData projectData, bool debug);
    static void SaveProject();
    static ProjectData LoadProject(std::filesystem::path path);
    
    static ProjectData projectData;
};
