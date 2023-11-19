#include "MeshRenderer.h"
#include "rlgl.h"

Model MeshRenderer::GetModel() const
{
    return model;
}

void MeshRenderer::SetModel(Model model)
{
    this->model = model;
    this->bounds = GetMeshBoundingBox(model.meshes[0]); // Todo: Set the correct scale depending on size variable.
}

std::filesystem::path MeshRenderer::GetModelPath() const
{
    return modelPath;
}

void MeshRenderer::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

BoundingBox MeshRenderer::GetBounds() const
{
    return bounds;
}

void MeshRenderer::SetBounds(BoundingBox bounds)
{
    this->bounds = bounds;
}

void MeshRenderer::Update(float deltaTime)
{
    rlPushMatrix();

    // build up the transform
    Matrix transform = MatrixTranslate(gameObject.transform.GetPosition().x, gameObject.transform.GetPosition().y, gameObject.transform.GetPosition().z);

    transform = MatrixMultiply(QuaternionToMatrix(gameObject.transform.GetRotation()), transform);

    transform = MatrixMultiply(MatrixScale(gameObject.transform.GetScale().x, gameObject.transform.GetScale().y, gameObject.transform.GetScale().z), transform);

    // apply the transform
    rlMultMatrixf(MatrixToFloat(transform));

    // draw model
    if ((std::filesystem::exists(GetModelPath())))
        DrawModel(GetModel(), Vector3Zero(), 1, WHITE);
    else if (GetModelPath() == "Cube")
        DrawCube(Vector3Zero(), 1, 1, 1, LIGHTGRAY);
    else if (GetModelPath() == "Plane")
        DrawPlane(Vector3Zero(), Vector2{ 1,1 }, LIGHTGRAY);
    else if (GetModelPath() == "Sphere")
        DrawSphere(Vector3Zero(), 1, LIGHTGRAY);
    else
    {
        // Invalid model path
    }

    rlPopMatrix();
}

void MeshRenderer::Destroy()
{
    UnloadModel(GetModel());
}