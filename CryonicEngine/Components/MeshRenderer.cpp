#include "MeshRenderer.h"
#include "../RaylibWrapper.h"
#if defined (EDITOR)
#include "../ProjectManager.h"
#else
#include "../Game.h"
#endif
//#include "rlgl.h"
//#include "../ShaderManager.h"

void MeshRenderer::Awake()
{
#if defined (EDITOR)
    SetMaterial(Material::GetMaterial(exposedVariables[1][0][2]));
#endif
}

RaylibModel& MeshRenderer::GetModel()
{
    return raylibModel;
}

void MeshRenderer::SetModel(ModelType model, std::filesystem::path path, ShaderManager::Shaders shader)
{
    //this->raylibModel = model;
#if defined (EDITOR)
    this->modelSet = raylibModel.Create(model, path, shader, ProjectManager::projectData.path / "Assets");
#else
    if (exeParent.empty())
        this->modelSet = raylibModel.Create(model, path, shader, "Resources/Assets");
    else
        this->modelSet = raylibModel.Create(model, path, shader, std::filesystem::path(exeParent) / "Resources" / "Assets");
#endif

    SetMaterial(material);

    // Todo: Set material. Using path or material var if its set
}

std::filesystem::path MeshRenderer::GetModelPath() const
{
    return modelPath;
}

void MeshRenderer::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

void MeshRenderer::SetMaterial(Material* mat)
{
    // Todo: The material can be used by multiple meshes. Track number of meshes using the material and if it only equals 1 (This mesh), then unload it
    material = mat;

    if (modelSet)
    {
        if (material == nullptr || material->GetPath() == "Default")
        {
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_ALBEDO, Material::whiteTexture, { 220, 220, 220, 255 }, 1);
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_NORMAL, Material::whiteTexture, { 128, 128, 255 }, 1); // { 128, 128, 255 } is "flat" for normal maps
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_ROUGHNESS, Material::whiteTexture, { 255, 255, 255, 255 }, 0.5f);
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_METALNESS, Material::whiteTexture, { 255, 255, 255, 255 }, 0);
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_EMISSION, Material::whiteTexture, { 255, 255, 255, 255 }, 0);
        }
        else
        {
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_ALBEDO, *material->GetAlbedoSprite()->GetTexture(), { material->GetAlbedoColor().r, material->GetAlbedoColor().g, material->GetAlbedoColor().b, material->GetAlbedoColor().a }, 1);
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_NORMAL, *material->GetNormalSprite()->GetTexture(), { 128, 128, 255 }, 1); // { 128, 128, 255 } is "flat" for normal maps
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_ROUGHNESS, *material->GetRoughnessSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetRoughness());
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_METALNESS, *material->GetMetallicSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetMetallic());
            raylibModel.SetMaterial(0, RaylibWrapper::MATERIAL_MAP_EMISSION, *material->GetEmissionSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetEmission());
        }
    }
}

Material* MeshRenderer::GetMaterial()
{
    return material;
}

void MeshRenderer::Render(bool renderShadows)
{
    if (!modelSet || (renderShadows && !castShadows))
        return;

    Vector3 position = gameObject->transform.GetPosition();
    Quaternion rotation = gameObject->transform.GetRotation();
    Vector3 scale = gameObject->transform.GetScale();
    raylibModel.DrawModelWrapper(position.x, position.y, position.z, scale.x, scale.y, scale.z, rotation.x, rotation.y, rotation.z, rotation.w, 255, 255, 255, 255);
}

#if defined(EDITOR)
void MeshRenderer::EditorUpdate()
{
    // Set material
    if ((!material && !(exposedVariables[1][0][2] == "Default") && defaultMaterial) || (material && material->GetPath() != exposedVariables[1][0][2])) // Todo: This will run continuously if the material is deleted and the file is deleted/moved.
    {
        if (exposedVariables[1][0][2] == "Default")
            defaultMaterial = true;
        else
            SetMaterial(Material::GetMaterial(exposedVariables[1][0][2]));
    }

    castShadows = exposedVariables[1][1][2].get<bool>();
}
#endif

void MeshRenderer::Destroy()
{
    // Todo: Currently Destroy() does work in the editor, but it shouldn't. Will need to move this.
    if (modelSet)
        raylibModel.DeleteInstance();
}