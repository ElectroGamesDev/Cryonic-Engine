#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <iostream>
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include "imgui_impl_raylib.h"

Camera camera = { 0 };

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI); // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    InitWindow(GetScreenWidth(), GetScreenHeight(), ("Game Name Here"));
    ToggleFullscreen();
    SetWindowMinSize(100, 100);
    SetTargetFPS(144); // Todo: Set target FPS to monitor refresh rate and handle editor being moved across monitors or just take the higher refresh rate
	
	SceneManager::LoadScene(std::filesystem::current_path() / "Scenes" / "Default.scene");
	SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
	
	camera.fovy = 45;
    camera.up.y = 1;
    camera.position.y = 3;
    camera.position.z = -25;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = { 0.0f, 0.0f, 0.0f };

    while (!WindowShouldClose())
    {
        BeginDrawing();
		
		ClearBackground(SKYBLUE);
		
		BeginMode3D(camera);
		
		for (GameObject& gameObject : SceneManager::GetActiveScene()->GetGameObjects())
		{
			if (!gameObject.IsActive()) continue;

			rlPushMatrix();

			// build up the transform
			Matrix transform = MatrixTranslate(gameObject.transform.GetPosition().x, gameObject.transform.GetPosition().y, gameObject.transform.GetPosition().z);

			transform = MatrixMultiply(QuaternionToMatrix(gameObject.transform.GetRotation()), transform);

			transform = MatrixMultiply(MatrixScale(gameObject.transform.GetScale().x, gameObject.transform.GetScale().y, gameObject.transform.GetScale().z), transform);

			// apply the transform
			rlMultMatrixf(MatrixToFloat(transform));

			// draw model
			if ((std::filesystem::exists(gameObject.GetModelPath())))
				DrawModel(gameObject.GetModel(), Vector3Zero(), 1, WHITE);
			else if (gameObject.GetModelPath() == "Cube")
				DrawCube(Vector3Zero(), 1, 1, 1, LIGHTGRAY);
			else if (gameObject.GetModelPath() == "Plane")
				DrawPlane(Vector3Zero(), Vector2{1,1}, LIGHTGRAY);
			else if (gameObject.GetModelPath() == "Sphere")
				DrawSphere(Vector3Zero(), 1, LIGHTGRAY);
			else
			{
				// Invalid model path or empty object
			}

			rlPopMatrix();
		}
		
		EndMode3D();
		
        EndDrawing();
    }

    //Cleanup();
    CloseWindow();
	return 0;
}