#pragma once

#include <unordered_map>
#include <string>

class ShaderManager
{
public:
	enum Shaders
	{
		None,
		LitStandard
	};

	static void Init();
	static void Cleanup();
	static void UpdateShaders(float cameraPosX, float cameraPosY, float cameraPosZ);
};