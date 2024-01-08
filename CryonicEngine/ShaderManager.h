#pragma once

#include <unordered_map>
#include <string>
#include "raylib.h"

class ShaderManager
{
public:
	static void Init();
	static void Cleanup();
	static void UpdateShaders();

	enum Shaders
	{
		LitStandard
	};

	static std::unordered_map<Shaders, Shader> shaders;
};