#include "ProjectManager.h"
#include "ConsoleLogger.h"
#include <cstdlib>
#include <direct.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Scenes/SceneManager.h"
#include "Utilities.h"
#include "Components/ScriptLoader.h"
#include "json.hpp"

using json = nlohmann::json;

ProjectData ProjectManager::projectData;

void ProjectManager::CopyApiFiles(std::filesystem::path source, std::filesystem::path destination)
{
    // Editor and IconManager needed for gizmos
    std::vector<std::string> filesToCopy = { "CryonicAPI", "CryonicCore", "resources", "Scenes", "ConsoleLogger", "FontManager", "GameObject", "Components", "ShaderManager", "InputSystem", "RaylibInputWrapper", "Wrappers", "RaylibCameraWrapper", "RaylibDrawWrapper", "RaylibLightWrapper", "RaylibModelWrapper", "RaylibShaderWrapper", "RaylibWrapper"};
     
    if (!std::filesystem::exists(destination))
        std::filesystem::create_directories(destination);

    for (const auto& file : std::filesystem::directory_iterator(source))
    {
        if (std::find(filesToCopy.begin(), filesToCopy.end(), file.path().stem()) != filesToCopy.end())
        {
            if (file.is_directory())
            {
                if (!std::filesystem::exists(destination / file.path().filename()))
                    std::filesystem::create_directory(destination / file.path().filename());
                std::filesystem::copy(file.path(), destination / file.path().filename(), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
            }
            else
                std::filesystem::copy(file.path(), destination);
        }
    }
}

int ProjectManager::CreateProject(ProjectData projectData) // Todo: Add try-catch
{
    projectData.path = projectData.path / projectData.name;
    //std::filesystem::path path = projectData.path / projectData.name;
    ConsoleLogger::InfoLog("Project \"" + projectData.name + "\" is being created at the path \"" + projectData.path.string() + "\"");

    std::filesystem::create_directory(projectData.path);
    if (!std::filesystem::exists(projectData.path)) return 1;

    std::filesystem::create_directory(projectData.path / "api");
    std::filesystem::create_directory(projectData.path / "Settings");
    std::filesystem::create_directory(projectData.path / "Plugins");
    std::filesystem::create_directory(projectData.path / "Assets");
    std::filesystem::create_directory(projectData.path / "Assets" / "Scripts");
    std::filesystem::create_directory(projectData.path / "Assets" / "Scenes");

    Utilities::HideFile(projectData.path / "api");

    CopyApiFiles(std::filesystem::path(__FILE__).parent_path(), projectData.path / "api");

    switch (projectData.templateData._template)
    {
    case Sidescroller3D:
    case Blank3D:
        std::filesystem::create_directory(projectData.path / "Assets" / "Models");
        break;
    case Blank2D:
        std::filesystem::create_directory(projectData.path / "Assets" / "Sprites");
        break;
    default:
        break;
    }

    SaveProjectData(projectData);

    return 0;
}

void ProjectManager::CleanupBuildFolder(std::filesystem::path path)
{
    try
    {
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            try {
                if (file.is_directory() && file.path().filename() != "Scenes")
                    std::filesystem::remove_all(file.path());
                else if (file.path().extension() != ".exe")
                    std::filesystem::remove(file.path());
            }
            catch (const std::exception& e) {
                if (file.path().filename() != "Scenes")
                    ConsoleLogger::ErrorLog("Build Log - Error deleting file: " + file.path().string() + " while cleaning up. Error: " + e.what());
                continue;
            }
        }
    }
    catch (const std::exception& e) {
        ConsoleLogger::ErrorLog("Build Log - Error during build cleanup: " + (std::string)e.what());
        return;
    }
}

//bool ProjectManager::ExtractZip(const char* zip_path, const char* dest_folder)
//{
//    mz_zip_archive zip_archive;
//    mz_bool status;
//
//    memset(&zip_archive, 0, sizeof(zip_archive));
//    status = mz_zip_reader_init_file(&zip_archive, zip_path, 0);
//    if (!status) {
//        printf("Failed to open ZIP file\n");
//        return false;
//    }
//
//    int num_files = mz_zip_reader_get_num_files(&zip_archive);
//    for (int i = 0; i < num_files; ++i) {
//        mz_zip_archive_file_stat file_stat;
//        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
//            printf("Failed to get file info\n");
//            mz_zip_reader_end(&zip_archive);
//            return false;
//        }
//
//        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
//            // Directory entry - create folder if needed
//            // You might want to handle creating directories here
//        }
//        else {
//            mz_uint uncompressed_size = static_cast<mz_uint>(file_stat.m_uncomp_size);
//            void* file_data = malloc(uncompressed_size);
//
//            if (!file_data) {
//                printf("Failed to allocate memory\n");
//                mz_zip_reader_end(&zip_archive);
//                return false;
//            }
//
//            if (!mz_zip_reader_extract_to_mem(&zip_archive, i, file_data, uncompressed_size, 0)) {
//                printf("Failed to extract file\n");
//                free(file_data);
//                mz_zip_reader_end(&zip_archive);
//                return false;
//            }
//
//            char file_path[256]; // Adjust size as needed
//            sprintf_s(file_path, sizeof(file_path), "%s/%s", dest_folder, file_stat.m_filename);
//
//            FILE* file;
//            if (fopen_s(&file, file_path, "wb") != 0 || !file) {
//                printf("Failed to create file %s\n", file_path);
//                free(file_data);
//                mz_zip_reader_end(&zip_archive);
//                return false;
//            }
//
//            fwrite(file_data, 1, uncompressed_size, file);
//            fclose(file);
//            free(file_data);
//        }
//    }
//
//    mz_zip_reader_end(&zip_archive);
//    return true;
//}

void ProjectManager::BuildToWindows(ProjectData projectData) // Todo: Maybe make a .json format file that contains information like scenes and which scene should be first opened
{
    // Todo: Change path of models and include models in build
    SaveProject();

    std::filesystem::path buildPath = projectData.path / "Builds" / "Windows";
    if (!std::filesystem::exists(buildPath))
        std::filesystem::create_directories(buildPath);

    std::filesystem::remove_all(buildPath); // Removes previous build files to prevent crash.

    try
    {
        std::filesystem::copy(std::filesystem::path(__FILE__).parent_path() / "BuildPresets", buildPath, std::filesystem::copy_options::recursive);
        //std::filesystem::copy_file(std::filesystem::path(__FILE__).parent_path() / "BuildPresets.zip", buildPath / "BuildPresets.zip");

        //ExtractZip((buildPath / "BuildPresets.zip").string().c_str(), buildPath.string().c_str());

        std::filesystem::create_directory(buildPath / "Scenes");
        std::filesystem::copy(projectData.path / "Assets" / "Scenes", buildPath / "Scenes"); // Todo: Instead, copy all of the .scene files from project and put in here
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        ConsoleLogger::ErrorLog("Build Log - Error copying BuildPresets directory and/or files, terminating build. Error: " + (std::string)e.what(), false);
        return;
    }

    if (!std::filesystem::exists(projectData.path / "api"))
    {
        std::filesystem::create_directories(projectData.path / "api");
        Utilities::HideFile(projectData.path / "api");
    }
    std::filesystem::remove_all(projectData.path / "api");

    CopyApiFiles(std::filesystem::path(__FILE__).parent_path(), projectData.path / "api");
    CopyApiFiles(projectData.path / "api", buildPath / "Source"); // Todo: Copy all files

    if (!BuildScripts(projectData.path / "Assets" / "Scripts", buildPath / "Source"))
        return;

    GenerateExposedVariablesFunctions(buildPath / "Source");

    // Saves original path, sets new current path, and then run cmake and mingw32-make
    std::filesystem::path originalPath = std::filesystem::current_path();
    _chdir(buildPath.string().c_str());

    ConsoleLogger::InfoLog("Build Log - Running cmake", false);

    system("cmake -G \"MinGW Makefiles\" .");

    ConsoleLogger::InfoLog("Build Log - Running mingw32-make", false);

    system(("mingw32-make -j" + std::to_string(static_cast<int>(std::round(Utilities::GetNumberOfCores() * 1))) + " PLATFORM=PLATFORM_DESKTOP").c_str()); // Todo: Make the number of cores configurable, and default at 75%

    // Resets the current path
    _chdir(originalPath.string().c_str());

    // Renames the .exe to the project name
    //std::filesystem::rename(buildPath / "Game.exe", buildPath / projectData.name + ".exe.); ------------------------ Fix this

    // Cleanup
    ConsoleLogger::InfoLog("Build Log - Cleaning up", false);

    CleanupBuildFolder(buildPath);

    ConsoleLogger::InfoLog("Build Log - Build complete", false);

    Utilities::OpenPathInExplorer(buildPath);

    // Todo: Open file explorer to build location if not already at lcoation in file explorere
}

void ProjectManager::SaveProject()
{
    ConsoleLogger::InfoLog("Saving project");
    for (Scene& scene : *SceneManager::GetScenes())
    {
        SceneManager::SaveScene(&scene);
    }
    ConsoleLogger::InfoLog("Project saved");
}

ProjectData ProjectManager::LoadProject(std::filesystem::path path)
{
    ProjectData projectData = LoadProjectData(path);
    //projectData.name = path.stem().string();
    projectData.path = path.string();
    return projectData;
}

void ProjectManager::SaveProjectData(ProjectData projectData) // Todo: Encode File
{
    json projectDataJson;

    projectDataJson["name"] = projectData.name;
    projectDataJson["is3D"] = projectData.is3D;
    
    std::filesystem::path path = (projectData.path / "ProjectSettings.cry");

    std::string formattedPath = path.string();
    std::size_t found = formattedPath.find('\\'); // Todo: Check if I need to format the path
    while (found != std::string::npos) {
        formattedPath.replace(found, 1, "\\\\");
        found = formattedPath.find('\\', found + 2);
    }
    formattedPath.erase(std::remove_if(formattedPath.begin(), formattedPath.end(), [](char c) {
        return !std::isprint(static_cast<unsigned char>(c));
        }), formattedPath.end());

    std::ofstream file(formattedPath);
    file << std::setw(4) << projectDataJson << std::endl;
    file.close();

    if (file.fail() && file.bad())
    {
        if (file.eof())
        {
            ConsoleLogger::ErrorLog("End of file reached while project data at the path '" + path.string() + "'");
        }
        else if (file.fail())
        {
            ConsoleLogger::ErrorLog("The project data file at the path \"" + path.string() + "\" failed to open. The file can't be found or you have invalid permissions.");
        }
        else if (file.bad())
        {
            char errorMessage[256];\
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The project data failed to save. Error: " + std::string(errorMessage));
        }
        else if (file.is_open())
        {
            ConsoleLogger::ErrorLog("The project data failed to save because the save file is open in another program");
        }
        else
        {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The project data failed to save. Error: " + std::string(errorMessage));
        }
        return;
    }
    ConsoleLogger::InfoLog("The project data has been saved");
}

ProjectData ProjectManager::LoadProjectData(std::filesystem::path projectPath) // Todo: Decode File
{
    if (!std::filesystem::exists(projectPath / "ProjectSettings.cry"))
    {
        ConsoleLogger::WarningLog("The project's data is missing, creating a new one.");
        // Todo: Create new Project Data. Check stuff like where Models or Sprites folder exists, etc to determine whether its a 2D or 3D game.
        throw std::runtime_error("Project data is missing at the path: " + (projectPath / "ProjectSettings.cry").string());
    }

    std::string filePathString = (projectPath / "ProjectSettings.cry").string();
    filePathString.erase(std::remove_if(filePathString.begin(), filePathString.end(),
        [](char c) { return !std::isprint(c); }), filePathString.end());

    std::ifstream file(filePathString);
    if (!file.is_open())
    {
        ConsoleLogger::WarningLog("Can not open the project data file.");
        // Todo: Handle this
        throw std::runtime_error("Failed to open the project data at the path: " + (projectPath / "ProjectSettings.cry").string());
    }
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    json projectDataJson = json::parse(fileStream.str());

    ProjectData projectData;
    projectData.name = std::string(projectDataJson["name"]);
    projectData.is3D = bool(projectDataJson["is3D"]);

    ConsoleLogger::InfoLog("Project data has been loaded");

    return projectData;
}

void ProjectManager::GenerateExposedVariablesFunctions(std::filesystem::path path)
{
    // I need to loop through each component of each gameobject of each scene. Then store the component name, ID, and values in an array.
    // Then I need to loop through the scripts (inside the build folder, not assets folder) and look for headers. Then when I find a header, check if that header is the name of one of my components
    // Stored in the array. If it is, then loop thruogh values of it in the array and create the functio nand switch

    std::unordered_map<std::filesystem::path, std::unordered_map<int, nlohmann::json>> components;

    // Todo: If a component in one scene as the same ID as a component in another scene, it will cause issues.
    for (Scene& scene : *SceneManager::GetScenes()) // Todo: Only iterate the scenes that are set to be built
    {
        for (GameObject* gameObject : scene.GetGameObjects())
        {
            for (Component* component : gameObject->GetComponents())
            {
                ScriptComponent* scriptComponent = dynamic_cast<ScriptComponent*>(component);
                if (!scriptComponent)
                    continue;
                components[scriptComponent->GetHeaderPath()][scriptComponent->id] = scriptComponent->exposedVariables;
            }
        }
    }

    for (const auto& component : components)
    {
        if (!std::filesystem::exists(path / component.first.filename()))
        {
            // Todo: Do something if the file does not exist
            ConsoleLogger::ErrorLog("Failed find " + (path / component.first.filename()).string()); // Todo: Remove this warning
            continue;
        }
        // Todo: This assume the .cpp is in teh same location as the header. Consider getting the .cpp path from the Component and store it in the components map
        if (!std::filesystem::exists(path / (component.first.stem().string() + ".cpp")))
        {
            // Todo: Do something if the file does not exist
            ConsoleLogger::ErrorLog("Failed find " + (path / (component.first.stem().string() + ".cpp")).string()); // Todo: Remove this warning
            continue;
        }

        // Todo: This will not work if a function has "{" before class declaration, such as in variables or in a comment

        // Create function in header
        std::ifstream headerFile(path / component.first.filename());
        if (!headerFile.is_open())
        {
            // Todo: Do something if file failed to open
            continue;
        }

        
        bool foundPublic = false;
        std::string line;
        while (std::getline(headerFile, line))
        {
            if (line.find("public:") != std::string::npos)
            {
                foundPublic = true;
                break;
            }
        }

        headerFile.clear();
        headerFile.seekg(0, std::ios::beg);

        line = "";
        bool placedFunction = false;
        std::ofstream tempHeader(path / (component.first.stem().string() + ".temp"));
        while (std::getline(headerFile, line))
        {
            if (foundPublic)
            {
                if (!placedFunction && line.find("public:") != std::string::npos) // Todo: Should probably remove whitespaces incase someone wrote it like this, "public :"
                {
                    tempHeader << line << std::endl;
                    tempHeader << "void SetExposedVariables() override;\n";
                    placedFunction = true;
                }
                else
                    tempHeader << line << std::endl;
            }
            else
            {
                if (!placedFunction && line.find("{") != std::string::npos)
                {
                    tempHeader << line << std::endl;
                    tempHeader << "public:\nvoid SetExposedVariables() override;\nprivate:\n";
                    placedFunction = true;
                }
                else
                    tempHeader << line << std::endl;
            }
        }

        headerFile.close();
        tempHeader.close();

        std::filesystem::remove(path / component.first.filename());
        std::filesystem::rename(path / (component.first.stem().string() + ".temp"), path / component.first.filename());

        if (!placedFunction)
        {
            // Todo: Failed to find a place to put the SetExposedVariables() function
            ConsoleLogger::ErrorLog("Failed to find a place to put SetExposedVariables() in " + (path / component.first.filename()).string()); // Todo: Remove this warning
            continue;
        }

        // Create function in CPP
        std::ifstream cppFile(path / (component.first.stem().string() + ".cpp"));
        if (!cppFile.is_open())
        {
            // Todo: Do something if file failed to open
            continue;
        }

        line = "";
        placedFunction = false;
        bool lastLineWasInclude = false;
        std::ofstream tempCpp(path / (component.first.stem().string() + ".temp"));
        while (std::getline(cppFile, line))
        {
            if (!placedFunction)
            {
                if (lastLineWasInclude && line.find("#include") == std::string::npos)
                {
                    tempCpp << ("void " + component.first.stem().string() + "::SetExposedVariables() {\n");

                    tempCpp << "switch (id) {\n";

                    for (const auto& data : component.second)
                    {
                        tempCpp << "case " + std::to_string(data.first) + std::string(":\n");
                        for (auto variables = data.second[1].begin(); variables != data.second[1].end(); ++variables)
                        {
                            if ((*variables)[0] == "float")
                                tempCpp << (*variables)[1].get<std::string>() + " = " + (*variables)[2].dump() + "f;\n";
                            else
                                tempCpp << (*variables)[1].get<std::string>() + " = " + (*variables)[2].dump() + ";\n";
                        }
                        tempCpp << "break;\n";
                    }

                    tempCpp << "}\n}\n";
                    placedFunction = true;
                }

                tempCpp << line << std::endl;

                if (!placedFunction && line.find("#include") != std::string::npos)
                    lastLineWasInclude = true;
                else
                    lastLineWasInclude = false;
            }
            else
                tempCpp << line << std::endl;
        }

        cppFile.close();
        tempCpp.close();

        std::filesystem::remove(path / (component.first.stem().string() + ".cpp"));
        std::filesystem::rename(path / (component.first.stem().string() + ".temp"), path / (component.first.stem().string() + ".cpp"));

        if (!placedFunction)
        {
            // Todo: Failed to find a place to put the SetExposedVariables() function
            ConsoleLogger::ErrorLog("Failed to find a place to put SetExposedVariables() in " + (path / (component.first.stem().string() + ".cpp")).string()); // Todo: Remove this warning
            continue;
        }
        // Iterate component.first[1]
    }

    //for (const auto& file : std::filesystem::recursive_directory_iterator(path))
    //{
    //    if (!std::filesystem::is_directory(file))
    //    {
    //        if (file.path().extension() == ".h")
    //        {

    //        }
    //        else if (file.path().extension() == ".cpp")
    //        {

    //        }
    //    }
    //}
}