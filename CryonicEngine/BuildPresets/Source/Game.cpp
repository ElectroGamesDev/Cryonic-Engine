#include "Game.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "Components/Component.h"
#include "Components/CameraComponent.h"
#include "ShaderManager.h"
#include "RaylibModelWrapper.h"
#include "RaylibWrapper.h"
#include "CollisionListener2D.h"
#include "Physics2DDebugDraw.h"

#ifdef IS3D
#include "Jolt/Jolt.h"
JPH_SUPPRESS_WARNINGS
#endif

b2World* world = nullptr;

int main(void)
{
	RaylibWrapper::SetConfigFlags(0);
	RaylibWrapper::InitWindow(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight(), (NAME));
	//RaylibWrapper::ToggleFullscreen();
	//RaylibWrapper::ToggleBorderlessWindowed();
	RaylibWrapper::SetWindowMinSize(100, 100);
	RaylibWrapper::SetTargetFPS(60);
	
	RaylibWrapper::InitAudioDevice();

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
	SceneManager::LoadScene(std::filesystem::current_path() / "Resources" / "Assets" / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
		
	for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
	{
		if (!gameObject->IsActive()) continue;
		for (Component* component : gameObject->GetComponents())
		{
			if (!component->IsActive()) continue;
			component->Start();
		}
	}

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

			for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
			{
				if (!gameObject->IsActive()) continue;
				for (Component* component : gameObject->GetComponents())
				{
					if (!component->IsActive()) continue;
					component->FixedUpdate(timeStep);
				}
			}
		}

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
		
		for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject->IsActive()) continue;
			for (Component* component : gameObject->GetComponents())
			{
				if (!component->IsActive()) continue;
				component->Update(RaylibWrapper::GetFrameTime());
			}
		}

		//world->DebugDraw();
		
		RaylibWrapper::EndMode3D();
		
		RaylibWrapper::EndDrawing();
    }

	// Todo: There may be other scenes loaded. Make sure to also unload them.

	SceneManager::UnloadScene(SceneManager::GetActiveScene());

	RaylibWrapper::CloseAudioDevice();

	ShaderManager::Cleanup();
    RaylibWrapper::CloseWindow();
	delete world;
	return 0;
}