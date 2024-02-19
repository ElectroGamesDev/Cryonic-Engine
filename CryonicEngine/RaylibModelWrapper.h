#pragma once
#include "ShaderManager.h"
#include <filesystem>

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
	bool Create(ModelType type, std::filesystem::path path, Shaders shader);
	void Unload();
	void DrawModelWrapper(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ, float rotationX, float rotationY, float rotationZ, float rotationW, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA);
};