#include "FileWatcher.h"
#include "ProjectManager.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>
#include "Sprite.h"

namespace FileWatcher
{
    static std::unordered_map<std::string, std::filesystem::path> deletedFiles; // file name, old path

    void Init()
    {
        efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();
        UpdateListener* listener = new UpdateListener();
        efsw::WatchID watchID3 = fileWatcher->addWatch(ProjectManager::projectData.path.string() + "\\Assets", listener, true, {{efsw::Option::WinBufferSize, 1024 * 1024}});
        fileWatcher->watch();
    }

    void UpdateListener::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
    {
        // Add and Delete is used for file moving.
        switch (action) {
        case efsw::Actions::Add:
        {
            auto it = deletedFiles.find(filename);
            if (it != deletedFiles.end())
            {
                FileMoved(it->second, dir + "\\" + filename);
                deletedFiles.erase(it);
            }
            break;
        }
        case efsw::Actions::Delete:
            deletedFiles[filename] = std::filesystem::path(dir + "\\" + filename);
            std::thread(CheckIfFileMoved, std::filesystem::path(dir + "\\" + filename)).detach();
            break;
        case efsw::Actions::Modified: // Gets called when a file is modifiued, or something gets changed in a folder
            FileModified(dir + "\\" + filename);
            break;
        case efsw::Actions::Moved: // Gets called when something is renamed
            FileMoved(oldFilename, filename);
            break;
        default:
            break;
        }
    }

    void CheckIfFileMoved(std::filesystem::path path)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto it = deletedFiles.find(path.filename().string());
        if (it != deletedFiles.end())
        {
            // File was not added, and therefore assumed to not have been moved. Handle deleted files here
            std::cout << path.string() + " Has been deleted" << std::endl;
            deletedFiles.erase(it);
        }
    }

    void FileMoved(std::filesystem::path oldPath, std::filesystem::path newPath)
    {
        // Update paths of everything using the old path
        std::cout << oldPath.string() + " Has been moved to " + newPath.string() << std::endl;

        // When moving a folder, it only runs Add and Delete on folder, not files within

        if (std::filesystem::exists(oldPath.string() + ".data"))
            std::filesystem::rename(oldPath.string() + ".data", (newPath.parent_path() / (newPath.stem().string() + newPath.extension().string() + ".data")));
    }
    void FileModified(std::filesystem::path filePath) // Todo: Since we have sleeps, after the sleep, we should check if there are any newer modifications. Applications like Gimp when saving/overwriting, it modifies the file 3 times. Having this would prevent it from being loaded and unloaded 3 times.
    {
        if (std::filesystem::is_directory(filePath) || filePath.extension() == ".data") // file types like .animgraph shouldn't be added here?
            return;

        // Todo: Add meshes
        // Todo: Add fonts

        if (filePath.extension() == ".png" || filePath.extension() == ".jpg" || filePath.extension() == ".jpeg") // Update sprites
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Its possible that the file is still being written and unable to be read. I assume this is because programs like gimp modify the files 3 times, or it just takes a bit to be readable.

            std::string relativePath = std::filesystem::relative(filePath, ProjectManager::projectData.path / "Assets").string();
            auto it = Sprite::textures.find(relativePath);
            if (it == Sprite::textures.end())
                return;

            ConsoleLogger::ErrorLog("TeSt");
            // For some reason it fails to load the texture. In discord, JeffM resonding to someone else said it shouldn't be called within another thread, but this was happening before too. Also otherS said its not thread safe
            RaylibWrapper::UnloadTexture(*it->second.first); // This doesn't seem to be actually unloading the texture since I can still use it???
            //delete it->second.first;
            //it->second.first = new RaylibWrapper::Texture2D(RaylibWrapper::LoadTexture(filePath.string().c_str()));
        }
    }
};