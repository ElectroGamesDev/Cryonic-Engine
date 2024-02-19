//#include "raylib.h"
//#include "raymath.h"
//#include "rlgl.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
//#include "imgui_impl_raylib.h"
#include "Components/Component.h"
#include "Components/CameraComponent.h"
#include "ShaderManager.h"
#include "RaylibModelWrapper.h"
#include "RaylibWrapper.h"

int main(void)
{
	RaylibWrapper::SetConfigFlags(RaylibWrapper::FLAG_WINDOW_RESIZABLE | RaylibWrapper::FLAG_WINDOW_HIGHDPI | RaylibWrapper::FLAG_MSAA_4X_HINT | RaylibWrapper::FLAG_VSYNC_HINT); // Todo: Make FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT optional
	RaylibWrapper::InitWindow(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight(), ("Game Name Here"));
	RaylibWrapper::ToggleFullscreen();
	RaylibWrapper::SetWindowMinSize(100, 100);
	RaylibWrapper::SetTargetFPS(144); // Todo: Option to set this to montitor refresh rate
	
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

    while (!RaylibWrapper::WindowShouldClose())
    {
		if (CameraComponent::main != nullptr)
            ShaderManager::UpdateShaders(CameraComponent::main->gameObject->transform.GetPosition().x, CameraComponent::main->gameObject->transform.GetPosition().y, CameraComponent::main->gameObject->transform.GetPosition().z);
		
		RaylibWrapper::BeginDrawing();
		
		RaylibWrapper::ClearBackground({ 135, 206, 235, 255 });
		
		CameraComponent::main->raylibCamera.BeginMode3D();
		
		for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject->IsActive()) continue;
			for (Component* component : gameObject->GetComponents())
			{
				if (!component->IsActive()) continue;
				component->Update(RaylibWrapper::GetFrameTime());
			}
		}
		
		RaylibWrapper::EndMode3D();
		
		RaylibWrapper::EndDrawing();
    }

	// Todo: Run component disabled function

    //Cleanup();
	ShaderManager::Cleanup();
    RaylibWrapper::CloseWindow();
	return 0;
}