#include "Material.h"

std::unordered_map<std::filesystem::path, Material*> Material::materials;
RaylibWrapper::Texture2D Material::whiteTexture;

void Material::LoadWhiteTexture()
{
    RaylibWrapper::Image image = RaylibWrapper::GenImageColor(1, 1, { 255, 255, 255, 255 });
    whiteTexture = RaylibWrapper::LoadTextureFromImage({ image.data, image.width, image.height, image.mipmaps, image.format });
    RaylibWrapper::UnloadImage(image);
}

void Material::UnloadWhiteTexture()
{
    RaylibWrapper::UnloadTexture(whiteTexture);
}

Material* Material::GetMaterial(std::string path)
{
    if (auto it = materials.find(path); it != materials.end())
        return it->second;
    else
        return new Material(path); // Material's constructor handles everything. Todo: This material isn't deleted if it fails in the constructor
}