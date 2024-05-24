#pragma once

#include "../../api/CryonicAPI.h"

class ScriptPreset : public Component
{
public:
	ScriptPreset(GameObject* obj, int id) : Component(obj, id) {};
	void Start() override;
	void Update(float deltaTime) override;
};