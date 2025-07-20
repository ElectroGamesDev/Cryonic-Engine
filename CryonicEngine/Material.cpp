#include "Material.h"
#include "ShaderManager.h"
#include "ShadowManager.h"

std::unordered_map<std::filesystem::path, Material*> Material::materials;
RaylibWrapper::Material Material::defaultMaterial;
RaylibWrapper::Texture2D Material::whiteTexture;
#if defined (EDITOR)
RaylibWrapper::Material Material::previewMaterial;
#endif
int Material::nextId = 2; // 0 is used for default, 1 is used for embed

void Material::LoadDefaultMaterial()
{
    //std::pair<unsigned int, int*> shader = ShaderManager::GetShader(ShaderManager::LitStandard);
    //defaultMaterial.shader.id = shader.first;
    //defaultMaterial.shader.locs = shader.second;
    defaultMaterial.shader.id = ShadowManager::shader.id;
    defaultMaterial.shader.locs = ShadowManager::shader.locs;

    defaultMaterial.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];
    defaultMaterial.maps[RaylibWrapper::MATERIAL_MAP_ALBEDO] = { Material::whiteTexture , { 220, 220, 220, 255 }, 1 };
    defaultMaterial.maps[RaylibWrapper::MATERIAL_MAP_NORMAL] = { Material::whiteTexture , { 128, 128, 255 }, 1 }; // { 128, 128, 255 } is "flat" for normal map
    defaultMaterial.maps[RaylibWrapper::MATERIAL_MAP_ROUGHNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0.5 };
    defaultMaterial.maps[RaylibWrapper::MATERIAL_MAP_METALNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0 };
    defaultMaterial.maps[RaylibWrapper::MATERIAL_MAP_EMISSION] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0 };

    defaultMaterial.params[0] = static_cast<float>(0);
}

void Material::UnloadDefaultMaterial()
{
    RaylibWrapper::UnloadMaterial(defaultMaterial);
}

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

#if defined (EDITOR)
void Material::LoadPreviewMaterial()
{
    previewMaterial.shader.id = ShadowManager::shader.id;
    previewMaterial.shader.locs = ShadowManager::shader.locs;

    previewMaterial.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];
}

void Material::UnloadPreviewMaterial()
{
    RaylibWrapper::UnloadMaterial(previewMaterial);
}
#endif

Material* Material::GetMaterial(std::string path)
{
    if (auto it = materials.find(path); it != materials.end())
        return it->second;
    else
        return new Material(path); // Material's constructor handles everything. Todo: This material isn't deleted if it fails in the constructor
}

RaylibWrapper::Material* Material::GetRaylibMaterial()
{
    return &raylibMaterial;
}