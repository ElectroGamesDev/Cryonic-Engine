#pragma once

#include "Component.h"
#include "../RaylibCameraWrapper.h"
#include <filesystem>

class CameraComponent : public Component
{
public:
	CameraComponent(GameObject* obj, int id);
	CameraComponent* Clone() override
	{
		return new CameraComponent(gameObject, -1);
	}
	void Start() override;
	void Update() override;
	void Destroy() override;
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;

	RaylibCamera raylibCamera;;
	static CameraComponent* main;
	bool setMain = false;
};