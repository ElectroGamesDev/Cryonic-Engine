#pragma once

#include "ProjectManager.h"
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
    Platformer25D
};

struct TemplateData
{
    std::string name;
    std::string description;
    Templates _template;
};

struct ProjectData
{
    std::string name;
    std::filesystem::path path;
    TemplateData templateData;
    
};

class ProjectManager {
public:
    static int CreateProject(ProjectData projectData);
    static void CleanupBuildFolder(std::filesystem::path path);
    static void CopyApiFiles(std::filesystem::path source, std::filesystem::path destination);
    //static bool ExtractZip(const char* zip_path, const char* dest_folder);
    static void BuildToWindows(ProjectData projectData);
    static void SaveProject();
    static ProjectData LoadProject(std::filesystem::path path);
};
