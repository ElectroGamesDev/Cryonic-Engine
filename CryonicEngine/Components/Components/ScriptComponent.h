#pragma once

#include "Component.h"
#include <filesystem>

class ScriptComponent : public Component
{
public:
	ScriptComponent(GameObject* obj) : Component(obj)
	{
		name = "ScriptComponent";
	}
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;
	void SetHeaderPath(std::filesystem::path path);
	void SetCppPath(std::filesystem::path path);
	std::filesystem::path GetHeaderPath();
	std::filesystem::path GetCppPath();
	void SetName(std::string name);
	std::string GetName();

private:
	std::filesystem::path _headerPath;
	std::filesystem::path _CppPath;
	std::string _name;
}; 