#pragma once

#include <unordered_map> 
#include <iostream>
#include "RaylibWrapper.h"

class IconManager
{
public:
	static void Cleanup();
	static void Init();

	static std::unordered_map<std::string, RaylibWrapper::Texture2D*> imageTextures;
};