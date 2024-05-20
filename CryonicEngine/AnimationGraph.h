#pragma once

#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
#include "Animation.h"
#include <variant>

class AnimationGraph
{
public:
	enum ConditionType
	{
		less,
		Equal,
		Greater,
	};
	struct Condition
	{
		std::string parameter;
		ConditionType conditionType;
		std::variant<bool, int, float> value;
	};
	struct Transition
	{
		Animation* to = nullptr;
		std::vector<Condition> conditions;
	};
	struct AnimationState
	{
		Animation animation;
		std::vector<Transition> transitions;
	};

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
			if (node["id"].get<int>() == -5) // Todo: Also check for Any nodes
				continue;

			AnimationState animationState;
			animationState.animation.id = node["id"].get<int>();
			animationState.animation.name = node["name"].get<std::string>();
			animationState.animation.loop = node["loop"].get<bool>();
			animationState.animation.speed = node["speed"].get<float>();
			if (node.contains("sprites") && !node["sprites"].is_null() && !node["sprites"].empty())
			{
				for (const auto& sprite : node["sprites"])
					animationState.animation.sprites.push_back(sprite.get<std::string>());
			}

			// Todo: Set transitions here

			animationStates.push_back(animationState);

			// Checks to see if its the start animation. Todo: This needs to be redone when transitions are added
			if (startAnimationState == nullptr)
			{
				for (const auto& link : jsonData["links"])
				{
					if ((link[0] == -5 && link[1] == animationState.animation.id) || (link[0] == animationState.animation.id || link[1] == -5)) // Checks if its the start animation
					{
						startAnimationState = &animationStates.back();
						break;
					}
				}
			}
		}
	}

	Animation* GetStartAnimation()
	{
		return &startAnimationState->animation;
	}

	// Hide in API
	AnimationState* GetStartAnimationState()
	{
		return startAnimationState;
	}

	std::vector<Animation*> GetAnimations()
	{
		std::vector<Animation*> animations;
		for (AnimationState& animationState : animationStates)
			animations.push_back(&animationState.animation);
		return animations;
	}

	// Hide in API
	std::vector<AnimationState>* GetAnimationStates()
	{
		return &animationStates;
	}

private:
	std::vector<AnimationState> animationStates;
	AnimationState* startAnimationState = nullptr;
};