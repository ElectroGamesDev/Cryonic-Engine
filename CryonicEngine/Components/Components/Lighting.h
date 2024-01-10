#pragma once

#include "Component.h"
#include <filesystem>

class Lighting : public Component
{
public:
	Lighting(GameObject* obj) : Component(obj)
	{
		name = "Lighting";
		runInEditor = true;
	}
	//void Start() override;
	void Update(float deltaTime) override;
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;
};