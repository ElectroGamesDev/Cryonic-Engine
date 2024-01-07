#include "CameraComponent.h"
#include "raylib.h"

void CameraComponent::Start()
{
    main = *this;
}

void CameraComponent::Update(float deltaTime)
{
    camera.position = gameObject->transform.GetPosition();
    camera.target = Vector3Add(gameObject->transform.GetPosition(), Vector3RotateByQuaternion({0,0,1}, gameObject->transform.GetRotation()));
}