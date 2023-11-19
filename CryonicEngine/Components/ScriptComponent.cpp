#include "ScriptComponent.h"

void ScriptComponent::Start()
{

}

void ScriptComponent::Update(float deltaTime)
{

}

void ScriptComponent::Destroy()
{

}

void ScriptComponent::SetPath(std::filesystem::path path)
{
	_path = path;
}

std::filesystem::path ScriptComponent::GetPath()
{
	return _path;
}