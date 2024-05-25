#include "MeshRenderer.h"
#include "../RaylibWrapper.h"
#if defined (EDITOR)
#include "../ProjectManager.h"
#endif
//#include "rlgl.h"
//#include "../ShaderManager.h"

RaylibModel& MeshRenderer::GetModel()
{
    return raylibModel;
}

void MeshRenderer::SetModel(ModelType model, std::filesystem::path path, Shaders shader)
{
    //this->raylibModel = model;
#if defined (EDITOR)
    this->modelSet = raylibModel.Create(model, path, shader, ProjectManager::projectData.path / "Assets");
#else
    this->modelSet = raylibModel.Create(model, path, shader, std::filesystem::path(RaylibWrapper::GetWorkingDirectory()) / "Resources" / "Assets");
#endif
}

std::filesystem::path MeshRenderer::GetModelPath() const
{
    return modelPath;
}

void MeshRenderer::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

void MeshRenderer::Update()
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
    // Todo: Currently Destroy() does work in the editor, but it shouldn't. Will need to move this.
    if (modelSet)
        raylibModel.DeleteInstance();
}