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
			AnimationState animationState;
			animationState.animation.id = node["id"].get<int>();
			animationState.animation.name = node["name"].get<std::string>();
			animationState.animation.loop = node["loop"].get<bool>();
			animationState.animation.speed = node["speed"].get<float>();
			if (node.contains("sprites") && !node["sprites"].is_null() && !node["sprites"].empty())
			{
				for (const auto& sprite : node["sprites"])
					animationState.animation.sprites.push_back(sprite);
			}

			// Todo: Set transitions here

			animationStates.push_back(animationState);
		}
	}

	void SetActiveAnimation(std::string animation)
	{
		if (animation == "Start" || animation == "Any")
			return;

		for (AnimationState& animationState : animationStates)
		{
			if (animationState.animation.GetName() == animation)
			{
				activeAnimationState = &animationState;
				break;
			}
		}
	}

	void SetActiveAnimation(Animation* animation)
	{
		for (AnimationState& animationState : animationStates)
		{
			if (animationState.animation.GetId() == animation->GetId())
			{
				activeAnimationState = &animationState;
				break;
			}
		}
	}

	Animation* GetActiveAnimation()
	{
		return &activeAnimationState->animation;
	}

	std::vector<Animation*> GetAnimations()
	{
		std::vector<Animation*> animations;
		for (AnimationState& animationState : animationStates)
			animations.push_back(&animationState.animation);
		return animations;
	}

private:
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

	std::vector<AnimationState> animationStates;
	AnimationState* activeAnimationState = nullptr;
};