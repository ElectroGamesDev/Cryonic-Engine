#include "CameraComponent.h"
#include "../CryonicCore.h"
#if defined(EDITOR)
#include "../ProjectManager.h"
//#include "../IconManager.h"
//#include "../Editor.h"
#endif

CameraComponent* CameraComponent::main = nullptr;

CameraComponent::CameraComponent(GameObject* obj) : Component(obj)
{
	name = "CameraComponent";
	iconUnicode = "\xef\x80\xb0";
	runInEditor = true;

#ifdef EDITOR
	if (ProjectManager::projectData.is3D)
		raylibCamera.SetProjection(0);
	else
		raylibCamera.SetProjection(1);
#elif IS3D
	raylibCamera.SetProjection(0);
#else
	raylibCamera.SetProjection(1);
#endif

	raylibCamera.SetFOVY(45); // Todo: Make this configurable on the camera, and add a dirty flag to update it when it changes
	raylibCamera.SetUpY(1);

	Vector3 pos = gameObject->transform.GetPosition();
	raylibCamera.SetPosition(pos.x, pos.y, pos.z);

	Vector3 target = gameObject->transform.GetPosition() + RotateVector3ByQuaternion({ 0,0,1 }, gameObject->transform.GetRotation());
	raylibCamera.SetTarget(target.x, target.y, target.z);

	// Todo: Near Plane
	// Todo: Far Planer
}

void CameraComponent::Start()
{
    main = this;
}

void CameraComponent::Update(float deltaTime)
{
	Vector3 pos = gameObject->transform.GetPosition();
	raylibCamera.SetPosition(pos.x, pos.y, pos.z);

	Vector3 target = gameObject->transform.GetPosition() + RotateVector3ByQuaternion({ 0,0,1 }, gameObject->transform.GetRotation());
	raylibCamera.SetTarget(target.x, target.y, target.z);
    //camera.target = Vector3Add(gameObject->transform.GetPosition(), Vector3RotateByQuaternion({0,0,1}, gameObject->transform.GetRotation()));
}

#if defined(EDITOR)
void CameraComponent::EditorUpdate()
{
    if (!setMain)
    {
        setMain = false;
        main = this;
    }
    // Draw3DBillboard(Editor::camera, *IconManager::imageTextures["CameraGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 }); // Todo: Re-add this
}
#endif