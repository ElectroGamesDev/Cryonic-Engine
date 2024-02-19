#include "Lighting.h"
#include "../CryonicCore.h"
#include "../RaylibLightWrapper.h"
#if defined(EDITOR)
//#include "../Editor.h"
//#include "../IconManager.h"
#endif

bool setup;
Vector3 lastPosition = {0,0,0};
RaylibLight raylibLight;

void Lighting::Update(float deltaTime)
{
	if (!setup)
	{
		setup = true;
		Vector3 pos = gameObject->transform.GetPosition();
		raylibLight.Create(0, pos.x, pos.y, pos.z, 0, 0, 0, 255, 255, 255, 255, LitStandard);
	}

	if (gameObject->transform.GetPosition() != lastPosition)
	{
		Vector3 pos = gameObject->transform.GetPosition();
		raylibLight.SetPosition(pos.x, pos.y, pos.z);
	}
	raylibLight.Update(); // Todo: Check if I need this here and not in the if statement above
	lastPosition = gameObject->transform.GetPosition(); // Todo: See if I can move this into the if statement above
}

#if defined(EDITOR)
void Lighting::EditorUpdate()
{
	//Draw3DBillboard(Editor::camera, *IconManager::imageTextures["LightGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 }); // Todo: Re-add this
}
#endif