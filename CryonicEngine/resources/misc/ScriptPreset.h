#pragma once

#include "Component.h"

class ScriptPreset : public Component
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	//void Destroy() override;
};