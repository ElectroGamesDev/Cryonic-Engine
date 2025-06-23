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
    // If a custom model was previously used then cleanup the embeded materials
    if (this->modelSet && !raylibModel.IsPrimitive())
    {
        for (RaylibWrapper::Material& mat : embededMaterials)
            delete[] mat.maps;

        embededMaterials.clear();
    }

    //this->raylibModel = model;
#if defined (EDITOR)
    this->modelSet = raylibModel.Create(model, path, shader, ProjectManager::projectData.path / "Assets");
#else
    if (exeParent.empty())
        this->modelSet = raylibModel.Create(model, path, shader, "Resources/Assets");
    else
        this->modelSet = raylibModel.Create(model, path, shader, std::filesystem::path(exeParent) / "Resources" / "Assets");
#endif

    // Set embeded materials
    if (model == Custom)
    {
        for (int i = 0; i < RaylibWrapper::MAX_MATERIAL_MAPS; i++)
            embededMaterials.push_back(raylibModel.GetMaterial(i));
    }

    SetMaterial(nullptr);
}

std::filesystem::path MeshRenderer::GetModelPath() const
{
    return modelPath;
}

void MeshRenderer::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

void MeshRenderer::SetMaterial(Material* mat) // Todo: MEMORY LEAK! We shouldn't be creating new materials here! Materials should already be created.
{
    // Todo: The material can be used by multiple meshes. Track number of meshes using the material and if it only equals 1 (This mesh), then unload it
    material = mat;

    if (modelSet)
    {
        // I need to use SetMaterials() instead of SetMaterialMap() so it removes the other materials on the model and so it doesn't affect other models with the old material
        if (material == nullptr || material->GetPath() == "Default")
        {
            // Set to the embeded materials
            if (embededMaterials.size() > 0)
            {
                // Todo: If a normal map isn't set, its not using the flat values and if the roughness map isn't set, it's most likely using 0 for the value
                std::vector<RaylibWrapper::Material*> embededMats;

                for (RaylibWrapper::Material& embededMat : embededMaterials)
                    embededMats.push_back(&embededMat);

                raylibModel.SetMaterials(embededMats);
            }
            // Set to a default material
            else
            {
                //RaylibWrapper::Material newMat;

                //std::pair<unsigned int, int*> shader = ShaderManager::GetShader(ShaderManager::LitStandard); // Todo: Change this to get the shader in the material
                //newMat.shader.id = shader.first;
                //newMat.shader.locs = shader.second;

                //newMat.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];
                //newMat.maps[RaylibWrapper::MATERIAL_MAP_ALBEDO] = { Material::whiteTexture , { 220, 220, 220, 255 }, 1 };
                //newMat.maps[RaylibWrapper::MATERIAL_MAP_NORMAL] = { Material::whiteTexture , { 128, 128, 255 }, 1 }; // { 128, 128, 255 } is "flat" for normal map
                //newMat.maps[RaylibWrapper::MATERIAL_MAP_ROUGHNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0.5 };
                //newMat.maps[RaylibWrapper::MATERIAL_MAP_METALNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0 };
                //newMat.maps[RaylibWrapper::MATERIAL_MAP_EMISSION] = { Material::whiteTexture , { 255, 255, 255, 255 }, 0 };

                //raylibModel.SetMaterialMap(0, RaylibWrapper::MATERIAL_MAP_ALBEDO, Material::whiteTexture, { 220, 220, 220, 255 }, 1);
                //raylibModel.SetMaterialMap(0, RaylibWrapper::MATERIAL_MAP_NORMAL, Material::whiteTexture, { 128, 128, 255 }, 1); // { 128, 128, 255 } is "flat" for normal maps
                //raylibModel.SetMaterialMap(0, RaylibWrapper::MATERIAL_MAP_ROUGHNESS, Material::whiteTexture, { 255, 255, 255, 255 }, 0.5f);
                //raylibModel.SetMaterialMap(0, RaylibWrapper::MATERIAL_MAP_METALNESS, Material::whiteTexture, { 255, 255, 255, 255 }, 0);
                //raylibModel.SetMaterialMap(0, RaylibWrapper::MATERIAL_MAP_EMISSION, Material::whiteTexture, { 255, 255, 255, 255 }, 0);

                raylibModel.SetMaterials({ &Material::defaultMaterial });
            }
        }
        else
        {
            //RaylibWrapper::Material newMat;

            //std::pair<unsigned int, int*> shader = ShaderManager::GetShader(ShaderManager::LitStandard); // Todo: Change this to get the shader in the material
            //newMat.shader.id = shader.first;
            //newMat.shader.locs = shader.second;

            //newMat.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];

            ////if (material->GetAlbedoSprite() != nullptr)
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_ALBEDO] = { *material->GetAlbedoSprite()->GetTexture(), { material->GetAlbedoColor().r, material->GetAlbedoColor().g, material->GetAlbedoColor().b, material->GetAlbedoColor().a }, 1 };
            ////else
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_ALBEDO] = { Material::whiteTexture , { material->GetAlbedoColor().r, material->GetAlbedoColor().g, material->GetAlbedoColor().b, material->GetAlbedoColor().a }, 1 };

            ////if (material->GetNormalSprite() != nullptr)
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_NORMAL] = { *material->GetNormalSprite()->GetTexture(), { 255, 255, 255 }, 1 };
            ////else
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_NORMAL] = { Material::whiteTexture , { 128, 128, 255 }, 1 }; // { 128, 128, 255 } is "flat" for normal maps

            ////if (material->GetRoughnessSprite() != nullptr)
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_ROUGHNESS] = { *material->GetRoughnessSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetRoughness() };
            ////else
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_ROUGHNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, material->GetRoughness() };

            ////if (material->GetMetallicSprite() != nullptr)
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_METALNESS] = { *material->GetMetallicSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetMetallic() };
            ////else
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_METALNESS] = { Material::whiteTexture , { 255, 255, 255, 255 }, material->GetMetallic() };

            ////if (material->GetEmissionSprite() != nullptr)
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_EMISSION] = { *material->GetEmissionSprite()->GetTexture(), { 255, 255, 255, 255 }, material->GetEmission() };
            ////else
            ////    newMat.maps[RaylibWrapper::MATERIAL_MAP_EMISSION] = { Material::whiteTexture , { 255, 255, 255, 255 }, material->GetEmission() };
            
            // Todo: Consider supporting multiple materials. Do this by checking if the mesh uses multiple materials.
            raylibModel.SetMaterials({ material->GetRaylibMaterial()});
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

// TODO: IN AN UPDATE FUNCTION (make sure editor runs it too), CHEDCK IF MATERIAL updated FLAG HAS BEEN CHECKED, IF IT HAS, THEN UPDATE THE MATERIAL. Although then when would it flip back?
// Maybe I should use callbacks

#if defined(EDITOR)
void MeshRenderer::EditorUpdate()
{
    if (exposedVariables[1][0][2] == "nullptr")
        exposedVariables[1][0][2] = "Default";

    // Set material
    if ((!material && !(exposedVariables[1][0][2] == "Default" && defaultMaterial)) || (material && material->GetPath() != exposedVariables[1][0][2])) // Todo: This will run continuously if the material is deleted and the file is deleted/moved.
    {
        ConsoleLogger::WarningLog(gameObject->GetName());
        if (exposedVariables[1][0][2] == "Default")
        {
            ConsoleLogger::WarningLog("123321");
            defaultMaterial = true;
            SetMaterial(nullptr);
        }
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