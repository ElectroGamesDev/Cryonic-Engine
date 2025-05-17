#include "RaylibModelWrapper.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "RaylibShaderWrapper.h"
#include <unordered_map>
#include "ConsoleLogger.h"

static std::unordered_map<std::filesystem::path, std::pair<Model, int>> models;
static std::unordered_map<ModelType, std::pair<Model, int>> primitiveModels;
std::pair<unsigned int, int*> RaylibModel::shadowShader;
std::pair<unsigned int, int*> RaylibModel::materialPreviewShadowShader;

bool RaylibModel::Create(ModelType type, std::filesystem::path path, ShaderManager::Shaders shader, std::filesystem::path projectPath)
{
    // Todo: Don't create new meshes for primitives
    switch (type)
    {
    case Custom:
        if (!std::filesystem::exists(projectPath / path))
        {
            // Todo: Send user error
            return false;
        }

        if (auto it = models.find(path); it != models.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            models[path] = std::make_pair(LoadModel((projectPath/ path).string().c_str()), 1);
            model = &models[path];
        }
        break;
    case Cube:
        if (auto it = primitiveModels.find(Cube); it != primitiveModels.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            primitiveModels[Cube] = std::make_pair(LoadModelFromMesh(GenMeshCube(1, 1, 1)), 1);
            model = &primitiveModels[Cube];
        }
        primitiveModel = true;
        break;
    case Sphere:
        if (auto it = primitiveModels.find(Sphere); it != primitiveModels.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            primitiveModels[Sphere] = std::make_pair(LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32)), 1);
            model = &primitiveModels[Sphere];
        }
        primitiveModel = true;
        break;
    case Plane:
        if (auto it = primitiveModels.find(Plane); it != primitiveModels.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            primitiveModels[Plane] = std::make_pair(LoadModelFromMesh(GenMeshPlane(1, 1, 1, 1)), 1);
            model = &primitiveModels[Plane];
        }
        primitiveModel = true;
        break;
    case Cylinder:
        if (auto it = primitiveModels.find(Cylinder); it != primitiveModels.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            primitiveModels[Cylinder] = std::make_pair(LoadModelFromMesh(GenMeshCylinder(0.5f, 2, 32)), 1);
            model = &primitiveModels[Cylinder];
        }
        primitiveModel = true;
        break;
    case Cone:
        if (auto it = primitiveModels.find(Cone); it != primitiveModels.end())
        {
            model = &(it->second);
            model->second++;
        }
        else
        {
            primitiveModels[Cone] = std::make_pair(LoadModelFromMesh(GenMeshCone(0.5f, 1, 32)), 1);
            model = &primitiveModels[Cone];
        }
        primitiveModel = true;
        break;
    default:
        return false;
    }

    modelShader = shader;

    if (path == "MaterialPreview" && projectPath == "MaterialPreview")
    {
        for (size_t i = 0; i < model->first.materialCount; ++i)
            model->first.materials[i].shader = { materialPreviewShadowShader.first, materialPreviewShadowShader.second };
    }
    else if (shader != ShaderManager::None)
    {
        for (size_t i = 0; i < model->first.materialCount; ++i)
            //model->first.materials[i].shader = RaylibShader::shaders[modelShader].shader;
            model->first.materials[i].shader = { shadowShader.first, shadowShader.second };
    }

    return true;
}

void RaylibModel::Unload()
{
    if (model == nullptr)
        return;
    UnloadModel(model->first);
    if (primitiveModel)
    {
        auto it = primitiveModels.begin();
        while (it != primitiveModels.end())
        {
            if (&it->second == model)
            {
                it = primitiveModels.erase(it);
                break;
            }
            else
                ++it;
        }
    }
    else
    {
        auto it = models.begin();
        while (it != models.end())
        {
            if (&it->second == model)
            {
                it = models.erase(it);
                break;
            }
            else 
                ++it;
        }
    }
    model = nullptr;
}

void RaylibModel::DeleteInstance()
{
    model->second--;
    if (model->second <= 0)
        Unload();
}

void RaylibModel::DrawModelWrapper(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ, float rotationX, float rotationY, float rotationZ, float rotationW, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA)
{
    if (model == nullptr || model->first.meshCount < 1)
    {
        ConsoleLogger::ErrorLog("Error drawing model");
        return;

    }

    rlPushMatrix();

    // build up the transform
    Matrix transform = MatrixTranslate(posX, posY, posZ);

    transform = MatrixMultiply(QuaternionToMatrix({rotationX, rotationY, rotationZ, rotationW}), transform);

    transform = MatrixMultiply(MatrixScale(sizeX, sizeY, sizeZ), transform);

    // apply the transform
    rlMultMatrixf(MatrixToFloat(transform));

    //BeginShaderMode(ShaderManager::shaders[_shader]); // Todo: I think I can remove this since I'm setting the shader in Create()

    //if (renderShadow)
    //    BeginShaderMode({ shadowShader.first, shadowShader.second });

    // Draw model
    DrawModel(model->first, Vector3Zero(), 1, { colorR, colorG, colorB, colorA });

    //if (renderShadow)
    //    EndShaderMode();

    //EndShaderMode();

    rlPopMatrix();
}

void RaylibModel::SetShadowShader(unsigned int id, int* locs)
{
    shadowShader = { id, locs };
}

void RaylibModel::SetMaterialPreviewShader(unsigned int id, int* locs)
{
    materialPreviewShadowShader = { id, locs };
}

void RaylibModel::SetShaderValue(int materialIndex, int locIndex, const void* value, int uniformType)
{
    if (!model)
        return;

    ::SetShaderValue(model->first.materials[materialIndex].shader, locIndex, value, uniformType);
}

int RaylibModel::GetShaderLocation(int materialIndex, std::string uniformName)
{
    if (!model)
        return -1;

    return ::GetShaderLocation(model->first.materials[materialIndex].shader, uniformName.c_str());
}

void RaylibModel::SetShader(int materialIndex, ShaderManager::Shaders shader)
{
    modelShader = shader;
    if (shader == ShaderManager::LitStandard)
        model->first.materials[materialIndex].shader = { shadowShader.first, shadowShader.second };
    else if (shader == ShaderManager::None)
        model->first.materials[materialIndex].shader = {};
}

bool RaylibModel::IsPrimitive()
{
    return primitiveModel;
}

void RaylibModel::SetMaterialMap(int materialIndex, int mapIndex, RaylibWrapper::Texture2D texture, RaylibWrapper::Color color, float intensity)
{
    if (!model)
        return;

    model->first.materials[materialIndex].maps[mapIndex] = {
        { texture.id, texture.width, texture.height, texture.mipmaps, texture.format },
        { color.r, color.g, color.b, 255 },
        intensity
    };
}

void RaylibModel::SetMaterials(std::vector<RaylibWrapper::Material*> mats) // Todo: There is a memory leak here
{
    if (!model)
        return;

    // Materials should never be modified or unloaded in here

    // Deallocate the materials' maps
    //if (model->first.materials)
    //{
    //    for (int i = 0; i < model->first.materialCount; ++i)
    //    {
    //        if (model->first.materials[i].maps)
    //            delete[] model->first.materials[i].maps;
    //    }
    //}

    //// If the material count is not the same, then resize the array
    //if (model->first.materialCount != mats.size())
    //{
    //    // Todo: Should old materials be unloaded?

    //    delete[] model->first.materials; // Deallocate the materials array
    //    model->first.materials = new Material[mats.size()]; // Allocate the materials array with the new size
    //    model->first.materialCount = mats.size();
    //}

    //delete[] model->first.materials; // Todo: Should I delete the old materials array? When I was testing this, when I modified this array, all of my models (all cubes) changed materials too. They may be
    // sharing this array. Although then there will be a memory leak for objects with a materials array not in use. Should I also delete the materials.maps array?

    model->first.materialCount = mats.size();
    model->first.materials = new Material[mats.size()];
    model->first.materials->maps = new MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];

    for (size_t i = 0; i < mats.size(); ++i)
    {
        RaylibWrapper::Material& mat = *mats[i];
        Material& material = model->first.materials[i];

        // Set shader
        material.shader.id = mat.shader.id;
        material.shader.locs = mat.shader.locs;

        // Set params
        material.params[0] = mat.params[0];
        material.params[1] = mat.params[1];
        material.params[2] = mat.params[2];
        material.params[3] = mat.params[3];

        // Set maps
        for (int j = 0; j < RaylibWrapper::MAX_MATERIAL_MAPS; ++j)
            SetMaterialMap(i, j, mat.maps[j].texture, mat.maps[j].color, mat.maps[j].value);
    }
}

RaylibWrapper::Material RaylibModel::GetMaterial(int index)
{
    if (!model)
        return {};

    Material* material = &model->first.materials[index];
    RaylibWrapper::Material mat;

    // Set maps
    mat.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];

    for (int i = 0; i < RaylibWrapper::MAX_MATERIAL_MAPS; ++i)
    {
        RaylibWrapper::MaterialMap map;
        MaterialMap& raylibMap = material->maps[i];

        map.texture = { raylibMap.texture.id, raylibMap.texture.width, raylibMap.texture.height, raylibMap.texture.mipmaps, raylibMap.texture.format };
        map.color = { raylibMap.color.r, raylibMap.color.g, raylibMap.color.b, raylibMap.color.a };
        map.value = raylibMap.value;

        mat.maps[i] = map;
    }

    // Set shader
    mat.shader.id = material->shader.id;
    mat.shader.locs = material->shader.locs;

    // Set params
    mat.params[0] = material->params[0];
    mat.params[1] = material->params[1];
    mat.params[2] = material->params[2];
    mat.params[3] = material->params[3];

    return mat;
}