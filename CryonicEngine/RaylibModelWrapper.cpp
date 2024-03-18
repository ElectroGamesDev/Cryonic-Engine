#include "RaylibModelWrapper.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "RaylibShaderWrapper.h"
#include <unordered_map>
#include "ConsoleLogger.h"

static std::unordered_map<std::filesystem::path, std::pair<Model, int>> models;
static std::unordered_map<ModelType, std::pair<Model, int>> primitiveModels;

bool RaylibModel::Create(ModelType type, std::filesystem::path path, Shaders shader, std::filesystem::path projectPath)
{
    switch (type)
    {
    case Custom:
        if (!std::filesystem::exists(projectPath / "Assets" / path))
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
            models[path] = std::make_pair(LoadModel((projectPath / "Assets" / path).string().c_str()), 1);
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
            primitiveModels[Sphere] = std::make_pair(LoadModelFromMesh(GenMeshSphere(1, 1, 1)), 1);
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
            primitiveModels[Cylinder] = std::make_pair(LoadModelFromMesh(GenMeshCylinder(1, 1, 1)), 1);
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
            primitiveModels[Cone] = std::make_pair(LoadModelFromMesh(GenMeshCone(1, 1, 1)), 1);
            model = &primitiveModels[Cone];
        }
        primitiveModel = true;
        break;
    default:
        return false;
    }

    // Todo: I may need to do this when drawing since multiple game objects use the same model
    modelShader = shader;
    for (size_t i = 0; i < model->first.materialCount; ++i)
        model->first.materials[i].shader = RaylibShader::shaders[modelShader].shader;
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
    rlPushMatrix();

    // build up the transform
    Matrix transform = MatrixTranslate(posX, posY, posZ);

    transform = MatrixMultiply(QuaternionToMatrix({rotationX, rotationY, rotationZ, rotationW}), transform);

    transform = MatrixMultiply(MatrixScale(sizeX, sizeY, sizeZ), transform);

    // apply the transform
    rlMultMatrixf(MatrixToFloat(transform));

    //BeginShaderMode(ShaderManager::shaders[_shader]); // Todo: I think I can remove this since I'm setting the shader in Create()

    // Draw model
    DrawModel(model->first, Vector3Zero(), 1, { colorR, colorG, colorB, colorA });

    //EndShaderMode();

    rlPopMatrix();
}
