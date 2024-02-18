#pragma once

#include "Component.h"
#include <filesystem>

class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* obj) : Component(obj)
	{
		name = "CameraComponent";
		iconUnicode = "\xef\x80\xb0";
		runInEditor = true;

		//if (ProjectManager::projectData.is3D) // Todo: Re-add this
			camera.projection = CAMERA_PERSPECTIVE;
		//else
		//	camera.projection = CAMERA_ORTHOGRAPHIC;
		camera.fovy = 45;
		camera.up.y = 1;
		camera.position.y = 0;
		camera.position.z = 0;
		camera.position.x = 0;
		camera.target = { 0.0f, 0.0f, 0.0f };

		// Todo: Near Plane
		// Todo: Far Planer
	}
	void Start() override;
	void Update(float deltaTime) override;
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;
	Camera camera = { 0 };

	static CameraComponent* main;
	bool setMain = false;
};