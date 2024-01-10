#include "ScriptComponent.h"
#include "ScriptLoader.h"

void ScriptComponent::Start()
{
	// Todo: Move this to Awake()
	SetupScriptComponent(this);
	// Todo: Remove Component
}

void ScriptComponent::Update(float deltaTime)
{
}

void ScriptComponent::Destroy()
{

}

void ScriptComponent::SetHeaderPath(std::filesystem::path path)
{
	_headerPath = path;
}

void ScriptComponent::SetCppPath(std::filesystem::path path)
{
	_CppPath = path;
}

std::filesystem::path ScriptComponent::GetHeaderPath()
{
	return _headerPath;
}

std::filesystem::path ScriptComponent::GetCppPath()
{
	return _CppPath;
}

void ScriptComponent::SetName(std::string name)
{
	_name = name;
}

std::string ScriptComponent::GetName()
{
	return _name;
}