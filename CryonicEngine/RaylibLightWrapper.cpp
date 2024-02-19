#include "RaylibLightWrapper.h"
#include "raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "ShaderManager.h"
#include "RaylibShaderWrapper.h"

Light light;
Shader lightShader;

void RaylibLight::Create(int type, float posX, float posY, float posZ, float targetX, float targetY, float targetZ, unsigned char colorR, unsigned char colorG, unsigned char colorB, unsigned char colorA, Shaders shader)
{
	lightShader = RaylibShader::shaders[shader].shader;
	light = CreateLight(type, { posX, posY, posZ }, { targetX, targetY, targetZ }, { colorR, colorG, colorB, colorA }, RaylibShader::shaders[shader].shader);
}

void RaylibLight::SetPosition(float x, float y, float z)
{
	light.position = { x, y, z };
}

void RaylibLight::Update()
{
	UpdateLightValues(lightShader, light);
}
