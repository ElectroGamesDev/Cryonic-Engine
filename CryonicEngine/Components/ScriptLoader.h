#pragma once

#include "Component.h"
#include "ScriptComponent.h"
#include <filesystem>

void SetupScriptComponent(GameObject* gameObject, int id, bool active, std::string scriptName);

bool BuildScripts(std::filesystem::path projectPath, std::filesystem::path buildPath);