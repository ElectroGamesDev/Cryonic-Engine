#pragma once
#include "ShaderManager.h"
#include <filesystem>

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
	bool Create(ModelType type, std::filesystem::path path, Shaders shader, std::filesystem::path projectPath);
	void Unload();
	void DeleteInstance();
	void DrawModelWrapper(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ, float rotationX, float rotationY, float rotationZ, float rotationW, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA);

private:
	std::pair<Model, int>* model;
	bool primitiveModel = false;
	Shaders modelShader;
};