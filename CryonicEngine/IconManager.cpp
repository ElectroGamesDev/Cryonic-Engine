#include "IconManager.h"
#include <filesystem>

std::unordered_map<std::string, Texture2D*> IconManager::imageTextures;

void IconManager::Cleanup()
{
    for (auto& image : imageTextures)
    {
        UnloadTexture(*image.second);
        //delete image.second;
    }
    imageTextures.clear();
}

void IconManager::Init()
{
    for (const std::filesystem::path& path : std::filesystem::directory_iterator(std::filesystem::path(__FILE__).parent_path() / "resources" / "images"))
    {
        imageTextures[path.stem().string()] = new Texture2D(LoadTexture(path.string().c_str()));
    }
}
