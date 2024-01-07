#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "imgui_impl_raylib.h"
#include "Components/Component.h"

Camera camera = { 0 };

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI); // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    InitWindow(GetScreenWidth(), GetScreenHeight(), ("Game Name Here"));
    ToggleFullscreen();
    SetWindowMinSize(100, 100);
    //SetTargetFPS(144);
	
	SceneManager::LoadScene(std::filesystem::current_path() / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
	
	camera.fovy = 45;
    camera.up.y = 1;
    camera.position.y = 3;
    camera.position.z = -25;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = { 0.0f, 0.0f, 0.0f };
	
	for (GameObject& gameObject : SceneManager::GetActiveScene()->GetGameObjects())
	{
		if (!gameObject.IsActive()) continue;
		for (Component* component : gameObject.GetComponents())
		{
			if (!component->IsActive()) continue;
			component->Start();
		}
	}

    while (!WindowShouldClose())
    {
        BeginDrawing();
		
		ClearBackground(SKYBLUE);
		
		BeginMode3D(camera);
		
		for (GameObject& gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject.IsActive()) continue;
			for (Component* component : gameObject.GetComponents())
			{
				if (!component->IsActive()) continue;
				component->Update(GetFrameTime());
			}
		}
		
		EndMode3D();
		
        EndDrawing();
    }

    //Cleanup();
    CloseWindow();
	return 0;
}