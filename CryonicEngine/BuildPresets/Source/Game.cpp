#include "Game.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "Components/Component.h"
#include "Components/CameraComponent.h"
#include "Components/SpriteRenderer.h"
#include "AudioClip.h"
#include "ShaderManager.h"
#include "RaylibModelWrapper.h"
#include "RaylibWrapper.h"
#ifdef WINDOWS
#include <windows.h>
#elif WEB
#include <emscripten/emscripten.h>
#include <unordered_set>
#include "InputSystem.h"
#endif
#include "imgui_internal.h"
#include "FontManager.h"
#include <vector>

#ifdef IS3D
#include "../Jolt/Jolt.h"
#include "../Jolt/Core/Factory.h"
#include "../Jolt/Core/TempAllocator.h"
#include "../Jolt/Core/JobSystemThreadPool.h"
#include "Component/Rigidbody3D.h"
JPH_SUPPRESS_WARNINGS
#else
#include "CollisionListener2D.h"
#include "Physics2DDebugDraw.h"

b2World* world = nullptr;
CollisionListener2D collisionListener;
#endif

std::filesystem::path exeParent;

// Todo: Get this from project settings
// These are global so the MainLoop() can access them
float timeStep = 1.0f / 60.0f;
int32 velocityIterations = 8;
int32 positionIterations = 3;
float timeSinceLastUpdate = 0.0f;

void MainLoop();

int main(void)
{
#ifdef WINDOWS
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
#endif

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

	// Physics setup. Must go before scene loading
#ifdef IS3D
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();
	TempAllocatorImpl temp_allocator(10 * 1024 * 1024);
	JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	const uint cMaxBodies = 65536;
	const uint cNumBodyMutexes = 0;
	const uint cMaxBodyPairs = 65536;
	const uint cMaxContactConstraints = 10240;

	BPLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImplObjectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter;

	PhysicsSystem physicsSystem;
	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);

	//MyBodyActivationListener bodyActivationListener;
	//physicsSystem.SetBodyActivationListener(&body_activationListener);

	MyContactListener contactListener;
	physicsSystem.SetContactListener(&contactListener);

	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	Rigidbody3D::bodyInterface = physicsSystem.GetBodyInterface();
#else
	b2Vec2 gravity(0.0f, -9.8f);
	world = new b2World(gravity);
	world->SetContactListener(&collisionListener);
	//Physics2DDebugDraw debugDraw;
	//debugDraw.SetFlags(b2Draw::e_shapeBit);
	//world->SetDebugDraw(&debugDraw);
#endif

	// Shaders must be initiated before scenes/gameobjects
	ShaderManager::Init();

	// Todo: This assumes the default scene path and name
	if (exeParent.empty())
		SceneManager::LoadScene("Resources/Assets/Scenes/Default.scene");
	else
		SceneManager::LoadScene(exeParent / "Resources" / "Assets" / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());

#ifdef WEB
	emscripten_set_main_loop(MainLoop, 60, 1);
#else
	while (!RaylibWrapper::WindowShouldClose())
		MainLoop();
#endif

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
#ifdef IS3D
	UnregisterTypes();
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
#else
	delete world;
#endif
	return 0;
}

void MainLoop()
{
	timeSinceLastUpdate += RaylibWrapper::GetFrameTime();
	while (timeSinceLastUpdate >= timeStep)
	{
#ifdef IS3D
		physicsSystem.Update(timeStep, cCollisionSteps, &temp_allocator, &job_system);
#else
		world->Step(timeStep, velocityIterations, positionIterations);
		collisionListener.ContinueContact(); // Todo: Should this go after the loop?
#endif
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

	// This is used because mouse inputs don't work on web if the input happens between BeginDrawing() and EndDrawing(). Edit: This has been commented out because it appears to work without it, and the Raylib wiki may be out of date
//#ifdef WEB
//	Mouse::buttonsPressed.clear();
//	Mouse::buttonsReleased.clear();
//	Mouse::buttonsDown.clear();
//
//	for (int button = 0; button <= 6; button++)
//	{
//		MouseButton mouseButton = static_cast<MouseButton>(button);
//
//		if (RaylibWrapper::IsMouseButtonDown(static_cast<RaylibWrapper::MouseButton>(button)))
//			Mouse::buttonsDown.insert(mouseButton);
//
//		if (RaylibWrapper::IsMouseButtonPressed(static_cast<RaylibWrapper::MouseButton>(button)))
//			Mouse::buttonsPressed.insert(mouseButton);
//
//		if (RaylibWrapper::IsMouseButtonReleased(static_cast<RaylibWrapper::MouseButton>(button)))
//			Mouse::buttonsReleased.insert(mouseButton);
//	}
//#endif

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

	SpriteRenderer::Render();

	ImGui::End();

#ifdef IS2D
	//world->DebugDraw();
#endif

	RaylibWrapper::EndMode3D();

	// Render GUI
	ImGui::Render();
	RaylibWrapper::ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

	RaylibWrapper::EndDrawing();
}