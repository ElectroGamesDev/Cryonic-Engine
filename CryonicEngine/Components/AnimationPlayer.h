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

    void SetActiveAnimation(std::string animation);

    void SetActiveAnimation(Animation* animation); // Todo: VisualStudio hints/autocomplete makes this the default SetActiveAnimation. The string paramter function should be the default

    Animation* GetActiveAnimation();

private:
    AnimationGraph* animationGraph = nullptr;
    AnimationGraph::AnimationState* activeAnimationState = nullptr;
    bool ownsGraph = false; // Currently when animations graphs are set as an exposed variables, they are created with a pointer. If its created by AnimationPlayer, this will be true so it knows to destroy it.
};