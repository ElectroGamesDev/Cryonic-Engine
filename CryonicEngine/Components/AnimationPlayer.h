#pragma once

#include "Component.h"
#include "../AnimationGraph.h"
#include "SpriteRenderer.h"

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
	void Update() override;
    void Destroy() override;

    void Pause();
    void Unpause();
    bool IsPaused();

    void SetAnimationGraph(AnimationGraph* animationGraph);
    AnimationGraph* GetAnimationGraph();

    void SetActiveAnimation(std::string animation);

    void SetActiveAnimation(Animation* animation); // Todo: VisualStudio hints/autocomplete makes this the default SetActiveAnimation. The string paramter function should be the default

    Animation* GetActiveAnimation();

    bool paused = false;

private:
    SpriteRenderer* spriteRenderer = nullptr;
    AnimationGraph* animationGraph = nullptr;
    AnimationGraph::AnimationState* activeAnimationState = nullptr;
    bool ownsGraph = false; // Currently when animations graphs are set as an exposed variables, they are created with a pointer. If its created by AnimationPlayer, this will be true so it knows to destroy it.
    bool previouslyExisted = true; // Used to send a warning message if the SpriteRenderer was previously existed but now removed.
    float timeElapsed = 0.0f; // The time elapsed sinnce the animation started.
    int previousSprite = -1; // The previous animation sprite used. Used to determine whether the sprite needs to be changed
};