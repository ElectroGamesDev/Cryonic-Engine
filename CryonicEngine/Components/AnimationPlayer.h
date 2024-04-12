#pragma once

#include "Component.h"

class AnimationPlayer : public Component
{
public:
	AnimationPlayer(GameObject* obj, int id) : Component(obj, id)
	{
		name = "AnimationPlayer";
		iconUnicode = "\xef\x9c\x8c";
	}

	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

private:
};