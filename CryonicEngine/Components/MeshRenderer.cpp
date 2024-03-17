#include "MeshRenderer.h"
//#include "rlgl.h"
//#include "../ShaderManager.h"

RaylibModel& MeshRenderer::GetModel()
{
    return raylibModel;
}

void MeshRenderer::SetModel(ModelType model, std::filesystem::path path, Shaders shader)
{
    //this->raylibModel = model;
    this->modelSet = raylibModel.Create(model, path, shader);
}

std::filesystem::path MeshRenderer::GetModelPath() const
{
    return modelPath;
}

void MeshRenderer::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

void MeshRenderer::Update(float deltaTime)
{
    if (!modelSet)
        return;
    Vector3 position = gameObject->transform.GetPosition();
    Quaternion rotation = gameObject->transform.GetRotation();
    Vector3 scale = gameObject->transform.GetScale();
    raylibModel.DrawModelWrapper(position.x, position.y, position.z, scale.x, scale.y, scale.z, rotation.x, rotation.y, rotation.z, rotation.w, 255, 255, 255, 255);
}

void MeshRenderer::Destroy()
{
    if (modelSet)
        raylibModel.Unload();
}