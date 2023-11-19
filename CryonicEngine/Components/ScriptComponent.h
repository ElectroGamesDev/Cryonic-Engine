#pragma once

#include "Component.h"
#include "ScriptComponent.h"
#include <filesystem>
#include <functional>

class ScriptComponent : public Component
{
public:
	//Component* Instance = nullptr;

	//Component*(*InstantiateScript)();
	//void (*DestroyScript)(ScriptComponent);

	//template<typename T>
	//void Bind()
	//{
	//	InstantiateScript = []() { return = static_cast<Component*>(new T()); };
	//	DestroyScript = [](ScriptComponent* scriptComponent) { delete scriptComponent->Instance; scriptComponent->Instance = nullptr; };
	//}
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;
	void SetPath(std::filesystem::path path);
	std::filesystem::path GetPath();

private:
	std::filesystem::path _path;
}; 