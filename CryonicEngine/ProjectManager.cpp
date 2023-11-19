#include "ProjectManager.h"
#include "ConsoleLogger.h"
#include <cstdlib>
#include <direct.h>
#include <filesystem>
#include <fstream>
#include "Scenes/SceneManager.h"
#include "Utilities.h"

void ProjectManager::CopyApiFiles(std::filesystem::path source, std::filesystem::path destination)
{
    std::vector<std::string> filesToCopy = { "CryonicAPI", "Scenes", "ConsoleLogger", "FontManager", "GameObject", "Components" };
     
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
                std::filesystem::copy(file.path(), destination / file.path().filename());
            }
            else
                std::filesystem::copy(file.path(), destination);
        }
    }
}

int ProjectManager::CreateProject(ProjectData projectData) // Todo: Add try-catch
{
    std::filesystem::path path = projectData.path / projectData.name;
    ConsoleLogger::InfoLog("Project \"" + projectData.name + "\" is being created at the path \"" + path.string() + "\"");

    std::filesystem::create_directory(path);
    if (!std::filesystem::exists(path)) return 1;

    std::filesystem::create_directory(path / "api");
    std::filesystem::create_directory(path / "Settings");
    std::filesystem::create_directory(path / "Plugins");
    std::filesystem::create_directory(path / "Assets");
    std::filesystem::create_directory(path / "Assets" / "Scripts");
    std::filesystem::create_directory(path / "Assets" / "Scenes");

    Utilities::HideFile(path / "api");

    CopyApiFiles(std::filesystem::path(__FILE__).parent_path(), path / "api");

    switch (projectData.templateData._template)
    {
    case Platformer25D:
    case Blank3D:
        std::filesystem::create_directory(path / "Assets" / "Models");
        break;
    case Blank2D:
        std::filesystem::create_directory(path / "Assets" / "Sprites");
        break;
    default:
        break;
    }

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

void ProjectManager::BuildToWindows(ProjectData projectData) // Maybe make a .json format file that contains information like scenes and which scene should be first opened
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

    CopyApiFiles(projectData.path / "api", buildPath / "Source");

    // Saves original path, sets new current path, and then run cmake and mingw32-make
    std::filesystem::path originalPath = std::filesystem::current_path();
    _chdir(buildPath.string().c_str());

    ConsoleLogger::InfoLog("Build Log - Running cmake", false);

    system("cmake -G \"MinGW Makefiles\" .");

    ConsoleLogger::InfoLog("Build Log - Running mingw32-make", false);

    system("mingw32-make PLATFORM=PLATFORM_DESKTOP");

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
    ProjectData projectData;
    projectData.name = path.stem().string();
    projectData.path = path.string();
    return projectData;
}