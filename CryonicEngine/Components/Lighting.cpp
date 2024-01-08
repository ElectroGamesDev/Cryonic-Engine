#include "Lighting.h"
#include "raylib.h"
#include "rlights.h"
#include "../ShaderManager.h"
#include "../CryonicCore.h"
#include "../Editor.h"
#include "../IconManager.h"

bool setup;
Light light;
Vector3 lastPosition = {0,0,0};

void Lighting::Update(float deltaTime)
{
	if (!setup)
	{
		setup = true;
		light = CreateLight(LIGHT_POINT, gameObject->transform.GetPosition(), Vector3Zero(), BLUE, ShaderManager::shaders[ShaderManager::LitStandard]);
	}

	if (!Vector3Equals(gameObject->transform.GetPosition(), lastPosition))
	{
		light.position = gameObject->transform.GetPosition();
	}
	UpdateLightValues(ShaderManager::shaders[ShaderManager::LitStandard], light); // Todo: Check if I need this herer and not in the if statement above
	lastPosition = gameObject->transform.GetPosition(); // Todo: See if I can move this into the if statement above
}

void Lighting::EditorUpdate()
{
	Draw3DBillboard(Editor::camera, *IconManager::imageTextures["LightGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 });
}