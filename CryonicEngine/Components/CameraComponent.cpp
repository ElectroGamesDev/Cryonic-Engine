#include "CameraComponent.h"
#include "../CryonicCore.h"
#if defined(EDITOR)
#include "../ProjectManager.h"
//#include "../IconManager.h"
//#include "../Editor.h"
#endif
#include "../ShadowManager.h"

CameraComponent* CameraComponent::main = nullptr;

CameraComponent::CameraComponent(GameObject* obj, int id) : Component(obj, id)
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

void CameraComponent::Update()
{
	if (main == nullptr)
	{
		main = this;
		//ShadowManager::SetCamera(raylibCamera);
	}

	Vector3 pos = gameObject->transform.GetPosition();
	raylibCamera.SetPosition(pos.x, pos.y, pos.z);

	Vector3 target = gameObject->transform.GetPosition() + RotateVector3ByQuaternion({ 0,0,1 }, gameObject->transform.GetRotation());
	raylibCamera.SetTarget(target.x, target.y, target.z);
    //camera.target = Vector3Add(gameObject->transform.GetPosition(), Vector3RotateByQuaternion({0,0,1}, gameObject->transform.GetRotation()));
}

void CameraComponent::Destroy() // TOdo: This doesn't run in the edtitor
{
	if (main == this)
		main = nullptr;
}

#if defined(EDITOR)
void CameraComponent::EditorUpdate()
{
    if (!setMain)
    {
        setMain = true;
        main = this;
		//ShadowManager::SetCamera(raylibCamera);
    }
    // Draw3DBillboard(Editor::camera, *IconManager::imageTextures["CameraGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 }); // Todo: Re-add this
}
#endif

Vector2 CameraComponent::GetWorldToScreen(Vector3 position)
{
	std::array<float, 2> pos = raylibCamera.GetWorldToScreen(position.x, position.y, position.z);
	return {pos[0], pos[1]};
}