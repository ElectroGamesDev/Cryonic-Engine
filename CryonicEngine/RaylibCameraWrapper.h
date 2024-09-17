#pragma once

#include <array>

class RaylibCamera
{
public:
	void SetFOVY(int y);
	void SetUpY(int amount);
	void SetPosition(int x, int y, int z);
	void SetPositionX(int x);
	void SetPositionY(int y);
	void SetPositionZ(int z);
	void SetTarget(float x, float y, float z);
	// 0 = Perspective, 1 = Orthographic
	void SetProjection(int projection);
	void BeginMode3D();
	std::array<float, 2> GetWorldToScreen(float x, float y, float z);
};