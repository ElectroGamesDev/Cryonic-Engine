#pragma once

#include <unordered_map> 
#include <iostream>
#include "raylib.h"

class IconManager
{
public:
	static void Cleanup();
	static void Init();

	static std::unordered_map<std::string, Texture2D*> imageTextures;
};