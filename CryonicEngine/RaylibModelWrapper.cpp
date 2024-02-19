#include "RaylibModelWrapper.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "RaylibShaderWrapper.h"

Model model;
Shaders modelShader;

bool RaylibModel::Create(ModelType type, std::filesystem::path path, Shaders shader)
{
    switch (type)
    {
    case Custom:
        if (!std::filesystem::exists(path))
        {
            // Todo: Send user error
            return false;
        }
        model = LoadModel(path.string().c_str());
        break;
    case Cube:
        model = LoadModelFromMesh(GenMeshCube(1, 1, 1));
        break;
    case Sphere:
        LoadModelFromMesh(GenMeshSphere(1, 1, 1));
        break;
    case Plane:
        LoadModelFromMesh(GenMeshPlane(1, 1, 1, 1));
        break;
    case Cylinder:
        LoadModelFromMesh(GenMeshCylinder(1, 1, 1));
        break;
    case Cone:
        LoadModelFromMesh(GenMeshCone(1, 1, 1));
        break;
    default:
        return false;
    }

    modelShader = shader;
    for (size_t i = 0; i < model.materialCount; ++i)
        model.materials[i].shader = RaylibShader::shaders[modelShader].shader;
    return true;
}

void RaylibModel::Unload()
{
    UnloadModel(model);
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
    DrawModel(model, Vector3Zero(), 1, { colorR, colorG, colorB, colorA });

    //EndShaderMode();

    rlPopMatrix();
}
