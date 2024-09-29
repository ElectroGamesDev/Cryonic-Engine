#pragma once

#include <efsw/efsw.hpp>
#include <filesystem>

namespace FileWatcher
{
    class UpdateListener : public efsw::FileWatchListener
    {
        void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;
    };

    void Init();
    void FileMoved(std::filesystem::path oldPath, std::filesystem::path newPath);
    void FileModified(std::filesystem::path filePath);
    void CheckIfFileMoved(std::filesystem::path path);
};