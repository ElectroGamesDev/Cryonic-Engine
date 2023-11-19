#pragma once

#include "Component.h"
#include "ScriptComponent.h"
#include <filesystem>
#include <functional>

class ScriptComponent : public Component
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;
	void SetPath(std::filesystem::path path);
	std::filesystem::path GetPath();

private:
	std::filesystem::path _path;
}; 