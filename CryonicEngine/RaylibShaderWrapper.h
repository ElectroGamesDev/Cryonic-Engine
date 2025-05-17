#pragma once
#include "ShaderManager.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

class RaylibShader
{
public:
	void Load(const char* vertexPath, const char* fragmentPath);
	void Update(float cameraPosX, float cameraPosY, float cameraPosZ);
	void Unload();
	std::pair<unsigned int, int*> GetShader(); // id, locs

	Shader shader;
	static std::unordered_map<ShaderManager::Shaders, RaylibShader> shaders;
};