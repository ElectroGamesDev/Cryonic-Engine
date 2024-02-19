#pragma once

#include <unordered_map>
#include <string>

enum Shaders
{
	LitStandard
};

class ShaderManager
{
public:
	static void Init();
	static void Cleanup();
	static void UpdateShaders(float cameraPosX, float cameraPosY, float cameraPosZ);
};