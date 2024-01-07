#include "CameraComponent.h"
#include "../CryonicCore.h"
#include "../IconManager.h"
#include "../Editor.h"

CameraComponent CameraComponent::main(nullptr);

void CameraComponent::Start()
{
    main = *this;
}

void CameraComponent::Update(float deltaTime)
{
    camera.position = gameObject->transform.GetPosition();
    camera.target = Vector3Add(gameObject->transform.GetPosition(), Vector3RotateByQuaternion({0,0,1}, gameObject->transform.GetRotation()));

    // Move to DrawGizmo() or EditorUpdate()
    Draw3DBillboard(Editor::camera, *IconManager::imageTextures["CameraGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 });
}