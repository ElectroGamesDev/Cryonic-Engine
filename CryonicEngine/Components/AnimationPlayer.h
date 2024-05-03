#pragma once

#include "Component.h"
#include "../AnimationGraph.h"

class AnimationPlayer : public Component
{
public:
	AnimationPlayer(GameObject* obj, int id) : Component(obj, id)
	{
		name = "AnimationPlayer";
		iconUnicode = "\xef\x9c\x8c";

#if defined(EDITOR)
        std::string variables = R"(
        [
            0,
            [
                [
                    "AnimationGraph",
                    "animationGraph",
                    "nullptr",
                    "Animation Graph",
                    {
                        "Extensions": [".animgraph"]
                    }
                ]
            ]
        ]
    )";
        exposedVariables = nlohmann::json::parse(variables);
#endif
	}

	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

    AnimationGraph* animationGraph = nullptr;

private:
};