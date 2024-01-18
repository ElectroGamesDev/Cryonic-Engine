#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "imgui_impl_raylib.h"
#include "Components/Component.h"
#include "Components/CameraComponent.h"
#include "ShaderManager.h"

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI); // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    InitWindow(GetScreenWidth(), GetScreenHeight(), ("Game Name Here"));
    ToggleFullscreen();
    SetWindowMinSize(100, 100);
    //SetTargetFPS(144);
	
	SceneManager::LoadScene(std::filesystem::current_path() / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
	
	ShaderManager::Init();
	
	for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
	{
		if (!gameObject->IsActive()) continue;
		for (Component* component : gameObject->GetComponents())
		{
			if (!component->IsActive()) continue;
			component->Start();
		}
	}

    while (!WindowShouldClose())
    {
		ShaderManager::UpdateShaders();
		
        BeginDrawing();
		
		ClearBackground(SKYBLUE);
		
		BeginMode3D(CameraComponent::main->camera);
		
		for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject->IsActive()) continue;
			for (Component* component : gameObject->GetComponents())
			{
				if (!component->IsActive()) continue;
				component->Update(GetFrameTime());
			}
		}
		
		EndMode3D();
		
        EndDrawing();
    }

    //Cleanup();
	ShaderManager::Cleanup();
    CloseWindow();
	return 0;
}