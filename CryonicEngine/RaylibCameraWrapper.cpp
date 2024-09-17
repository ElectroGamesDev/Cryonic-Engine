#include "RaylibCameraWrapper.h"
#include "raylib.h"
#include "RaylibWrapper.h"

Camera camera = { 0 }; // Todo: Store this as RaylibWrapper Camera, or be able to pass Raylib Camera in BeginMode3D

void RaylibCamera::SetFOVY(int fov)
{
	camera.fovy = fov;
}

void RaylibCamera::SetUpY(int amount)
{
	camera.up.y = amount;
}

void RaylibCamera::SetPosition(int x, int y, int z)
{
	camera.position.x = x;
	camera.position.y = y;
	camera.position.z = z;
}

void RaylibCamera::SetPositionX(int x)
{
	camera.position.x = x;
}

void RaylibCamera::SetPositionY(int y)
{
	camera.position.y = y;
}

void RaylibCamera::SetPositionZ(int z)
{
	camera.position.z = z;
}

void RaylibCamera::SetTarget(float x, float y, float z)
{
	camera.target.x = x;
	camera.target.y = y;
	camera.target.z = z;
}

void RaylibCamera::SetProjection(int projection)
{
	camera.projection = projection;
}

void RaylibCamera::BeginMode3D()
{
	RaylibWrapper::BeginMode3D({ { camera.position.x, camera.position.y, camera.position.z }, { camera.target.x, camera.target.y, camera.target.z }, { camera.up.x, camera.up.y, camera.up.z }, camera.fovy, camera.projection });
}

std::array<float, 2> RaylibCamera::GetWorldToScreen(float x, float y, float z)
{
	Vector2 pos = ::GetWorldToScreen({x, y, z}, camera);
	return {pos.x, pos.y};
}