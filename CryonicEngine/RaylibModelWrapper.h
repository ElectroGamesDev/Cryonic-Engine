#pragma once
#include "ShaderManager.h"
#include <filesystem>
#include "RaylibWrapper.h"

class Model;

enum ModelType
{
	Custom,
	Cube,
	Sphere,
	Plane,
	Cylinder,
	Cone
};

class RaylibModel
{
public:
	bool Create(ModelType type, std::filesystem::path path, ShaderManager::Shaders shader, std::filesystem::path projectPath);
	void Unload();
	void DeleteInstance();
	void DrawModelWrapper(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ, float rotationX, float rotationY, float rotationZ, float rotationW, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA);
	void SetMaterial(int materialIndex, int mapIndex, RaylibWrapper::Texture2D texture, RaylibWrapper::Color color, float intesity);
	void SetShaderValue(int materialIndex, int locIndex, const void* value, int uniformType);
	int GetShaderLocation(int materialIndex, std::string uniformName);
	void SetShader(int materialIndex, ShaderManager::Shaders shader);
	static void SetShadowShader(unsigned int id, int* locs);
	static void SetMaterialPreviewShader(unsigned int id, int* locs);

private:
	std::pair<Model, int>* model;
	bool primitiveModel = false;
	ShaderManager::Shaders modelShader;
	static std::pair<unsigned int, int*> shadowShader;
	static std::pair<unsigned int, int*> materialPreviewShadowShader;
};
