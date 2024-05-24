#pragma once

#include "../../api/CryonicAPI.h"

class ScriptPreset : public Component
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};