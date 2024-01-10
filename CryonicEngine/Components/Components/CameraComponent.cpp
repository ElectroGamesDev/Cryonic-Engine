#include "CameraComponent.h"
#include "../CryonicCore.h"
#if defined(EDITOR)
#include "../IconManager.h"
#include "../Editor.h"
#endif

CameraComponent* CameraComponent::main = nullptr;

void CameraComponent::Start()
{
    main = this;
}

void CameraComponent::Update(float deltaTime)
{
    camera.position = gameObject->transform.GetPosition();
    camera.target = Vector3Add(gameObject->transform.GetPosition(), Vector3RotateByQuaternion({0,0,1}, gameObject->transform.GetRotation()));
}

#if defined(EDITOR)
void CameraComponent::EditorUpdate()
{
    if (!setMain)
    {
        setMain = false;
        main = this;
    }
    Draw3DBillboard(Editor::camera, *IconManager::imageTextures["CameraGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 });
}
#endif