#include "Game.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "Components/Component.h"
#include "Components/CameraComponent.h"
#include "AudioClip.h"
#include "ShaderManager.h"
#include "RaylibModelWrapper.h"
#include "RaylibWrapper.h"
#include "CollisionListener2D.h"
#include "Physics2DDebugDraw.h"
#include <windows.h>
#include "imgui_internal.h"
#include "FontManager.h"
#include <vector>

#ifdef IS3D
#include "Jolt/Jolt.h"
JPH_SUPPRESS_WARNINGS
#endif

b2World* world = nullptr;
std::filesystem::path exeParent;

int main(void)
{
	// Sets executable path to a variable
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		char exePath[MAX_PATH];
		GetModuleFileName(hModule, exePath, (sizeof(exePath)));
		std::string temp = exePath;
		exeParent = temp;
		exeParent = exeParent.parent_path();
	}
	else
	{
		ConsoleLogger::WarningLog("Failed to get module handle. There may be game-breaking issues.", false);
		exeParent = std::filesystem::current_path();
	}

	// Creates the window
	RaylibWrapper::SetConfigFlags(0);
	RaylibWrapper::InitWindow(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight(), (NAME));
	//RaylibWrapper::ToggleFullscreen();
	//RaylibWrapper::ToggleBorderlessWindowed();
	//if (RaylibWrapper::GetScreenWidth() == RaylibWrapper::GetMonitorWidth(RaylibWrapper::GetCurrentMonitor()) && RaylibWrapper::GetScreenHeight() == RaylibWrapper::GetMonitorHeight(RaylibWrapper::GetCurrentMonitor())) RaylibWrapper::MaximizeWindow();
	RaylibWrapper::SetWindowMinSize(100, 100);
	RaylibWrapper::SetTargetFPS(60);
	
	RaylibWrapper::InitAudioDevice();

	// ImGui Setup
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enabled Multi-Viewports
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui::StyleColorsDark();
	RaylibWrapper::ImGui_ImplRaylib_Init();

	FontManager::InitFontManager();

	// Must go before scene loading
	CollisionListener2D collisionListener;
	b2Vec2 gravity(0.0f, -9.8f);
	world = new b2World(gravity);
	world->SetContactListener(&collisionListener);
	//Physics2DDebugDraw debugDraw;
	//debugDraw.SetFlags(b2Draw::e_shapeBit);
	//world->SetDebugDraw(&debugDraw);

	// Shaders must be initiated before scenes/gameobjects
	ShaderManager::Init();

	// Todo: This assumes the default scene path and name
	SceneManager::LoadScene(exeParent / "Resources" / "Assets" / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());

	float timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	float timeSinceLastUpdate = 0.0f;

    while (!RaylibWrapper::WindowShouldClose())
    {
		timeSinceLastUpdate += RaylibWrapper::GetFrameTime();
		while (timeSinceLastUpdate >= timeStep)
		{
			world->Step(timeStep, velocityIterations, positionIterations);
			collisionListener.ContinueContact();
			timeSinceLastUpdate -= timeStep;

			fixedDeltaTime = timeStep;

			for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
			{
				if (!gameObject->IsActive() || !gameObject->IsGlobalActive())
					continue;
				for (Component* component : gameObject->GetComponents())
				{
					if (!component->IsActive())
						continue;
					component->FixedUpdate();
					fixedDeltaTime = timeStep; // Setting this here and before the loop incase if a component changes the fixed delta time
				}
			}
		}

		// GUI
		FontManager::UpdateFonts();

		RaylibWrapper::ImGui_ImplRaylib_ProcessEvents();
		RaylibWrapper::ImGui_ImplRaylib_NewFrame();
		ImGui::NewFrame();

		if (CameraComponent::main != nullptr)
            ShaderManager::UpdateShaders(CameraComponent::main->gameObject->transform.GetPosition().x, CameraComponent::main->gameObject->transform.GetPosition().y, CameraComponent::main->gameObject->transform.GetPosition().z);
		
		RaylibWrapper::BeginDrawing();
		
#ifdef IS3D
		RaylibWrapper::ClearBackground({ 135, 206, 235, 255 });
#else
		RaylibWrapper::ClearBackground({ 128, 128, 128, 255 });
#endif
		
		CameraComponent::main->raylibCamera.BeginMode3D();
		
		// Update CollisionSystem

		deltaTime = RaylibWrapper::GetFrameTime();
		float tempDelaTime = deltaTime;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight()));
		ImGui::Begin("##Game", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);

		// Call components Update()
		// Not sure if I should use a range-based loop since a gameobject or component can be deleted within the loop
		for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject->IsActive()) continue;
			for (Component* component : gameObject->GetComponents())
			{
				if (!component->IsActive()) continue;
				component->Update();
				deltaTime = tempDelaTime; // Setting this here and before the loop incase if a component changes the delta time

				if (gameObject && component) // Component/Gameobject may get deleted in the Update(). I could also check if the gameobject and component is still active here although most likely useless overhead
					component->RenderGui();
			}
		}

		ImGui::End();

		//world->DebugDraw();
		
		RaylibWrapper::EndMode3D();

		// Render GUI
		ImGui::Render();
		RaylibWrapper::ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
		
		RaylibWrapper::EndDrawing();
    }

	// Todo: There may be other scenes loaded. Make sure to also unload them.

	SceneManager::UnloadScene(SceneManager::GetActiveScene());

	// Cleanup sounds (AudioPlayer cleans up streamed sounds)
	for (auto& pair : AudioClip::sounds)
		Raylib::UnloadSound(pair.second);

	RaylibWrapper::CloseAudioDevice();
	RaylibWrapper::ImGui_ImplRaylib_Shutdown();
	ImGui::DestroyContext();

	ShaderManager::Cleanup();
    RaylibWrapper::CloseWindow();
	delete world;
	return 0;
}