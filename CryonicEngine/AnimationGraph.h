#pragma once

#include <any>
#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
#include "Animation.h"

class AnimationGraph
{
public:
	AnimationGraph(std::string path)
	{
		std::ifstream file(std::filesystem::path(RaylibWrapper::GetWorkingDirectory()) / "Resources" / "Assets" / path); // Todo: Should I just use Filesystem GetWorkingPath? GetWorkingPath isn't even the best solution here
	
		if (!file.is_open())
		{
			ConsoleLogger::ErrorLog("Animation Graph failed to load. Path: " + path);
			return;
		}

		nlohmann::json jsonData;
		file >> jsonData;

		file.close();

		for (const auto& node : jsonData["nodes"])
		{
			Animation animation;
			animation.name = node["name"].get<std::string>();
			animation.loop = node["loop"].get<bool>();
			animation.speed = node["speed"].get<float>();
			if (node.contains("sprites") && !node["sprites"].is_null() && !node["sprites"].empty())
			{
				for (const auto& sprite : node["sprites"])
					animation.sprites.push_back(sprite);
			}
			animations.push_back(animation);
		}
	}

	void SetActiveAnimation(std::string animation)
	{

	}
	void SetActiveAnimation(Animation* animation)
	{

	}

	Animation* GetActiveAnimation()
	{

	}

	std::vector<Animation>& GetAnimations()
	{
		return animations;
	}

	// Todo: Currently for each AnimationPlayer, its creating a new copy of the animations data. It should instead use references if its already been created.

private:
	std::vector<Animation> animations;
};