#pragma once

#include "Component.h"
#include <filesystem>

class Lighting : public Component
{
public:
	Lighting(GameObject* obj, int id) : Component(obj, id)
	{
		name = "Lighting";
		iconUnicode = "\xef\x83\xab";
		runInEditor = true;
	}
	Lighting* Clone() override
	{
		return new Lighting(gameObject, -1);
	}
	//void Start() override;
	void Update(float deltaTime) override;
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;
};