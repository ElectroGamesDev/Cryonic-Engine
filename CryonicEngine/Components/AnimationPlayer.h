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

    // Todo: Currently for each AnimationPlayer, its creating a new copy of the animations data. It should instead use references if its already been created.

	void Start() override;
	void Update(float deltaTime) override;
    void Destroy() override;

    void SetAnimationGraph(AnimationGraph* animationGraph);
    AnimationGraph* GetAnimationGraph();

private:
    AnimationGraph* animationGraph = nullptr;
    bool ownsGraph = false;
};