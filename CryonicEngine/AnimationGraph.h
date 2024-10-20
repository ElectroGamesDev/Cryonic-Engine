#pragma once

#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
#include "Animation.h"
#include <variant>
#ifndef EDITOR
#include "Game.h"
#endif

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
		for (char& c : path) // Reformat the path for unix
		{
			if (c == '\\')
				c = '/';
		}

		std::ifstream file;
#ifndef EDITOR
		if (exeParent.empty())
			file.open("Resources/Assets/" + path);
		else
			file.open(std::filesystem::path(exeParent) / "Resources" / "Assets" / path);
#endif
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
					animationState.animation.sprites.push_back(new Sprite(sprite.get<std::string>()));
			}

			// Todo: Set transitions here

			animationStates.push_back(animationState);
		}

		// Finds the start animation and sets it to startAnimationState. This must be done here so the startAnimationState pointer doesn't invalidate when animationStates resizes
		int startAnimationId = 0;
		for (const auto& link : jsonData["links"])
		{
			// Checks if its the start animation
			// Abs() is probably not needed here since the input id should always be positive, unlike the output id
			if (link[0] == -5)
			{
				startAnimationId = std::abs(link[1].get<int>());
				break;
			}
			else if (link[1] == -5)
			{
				startAnimationId = std::abs(link[0].get<int>());
				break;
			}
		}
		for (AnimationState& animationState : animationStates)
		{
			if (startAnimationState != nullptr)
				break;

			if (animationState.animation.id == startAnimationId)
				startAnimationState = &animationState;
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