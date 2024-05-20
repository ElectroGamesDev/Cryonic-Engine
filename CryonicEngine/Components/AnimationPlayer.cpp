#include "AnimationPlayer.h"

void AnimationPlayer::Start()
{
}


void AnimationPlayer::Update(float deltaTime)
{
}

void AnimationPlayer::Destroy()
{
	if (animationGraph != nullptr && ownsGraph)
		delete animationGraph;
}

void AnimationPlayer::SetAnimationGraph(AnimationGraph* animationGraph)
{
	if (this->animationGraph != nullptr && ownsGraph)
		delete this->animationGraph;
	ownsGraph = false;
	this->animationGraph = animationGraph;

	activeAnimationState = animationGraph->GetStartAnimationState();
}

AnimationGraph* AnimationPlayer::GetAnimationGraph()
{
	return animationGraph;
}

void AnimationPlayer::SetActiveAnimation(std::string animation)
{
	if (animationGraph == nullptr)
		return;

	for (AnimationGraph::AnimationState& animationState : *animationGraph->GetAnimationStates())
	{
		if (animationState.animation.GetId() < 0) // Checks if the animation ID is less than 0 (Start or Any node)
			return;

		if (animationState.animation.GetName() == animation)
		{
			activeAnimationState = &animationState;
			break;
		}
	}
}

void AnimationPlayer::SetActiveAnimation(Animation* animation)
{
	if (animationGraph == nullptr || animation->GetId() < 0) // Checks if the animationGraph is null, or if the animation ID is less than 0 (Start or Any node)
		return;

	for (AnimationGraph::AnimationState& animationState : *animationGraph->GetAnimationStates())
	{
		if (animationState.animation.GetId() == animation->GetId())
		{
			activeAnimationState = &animationState;
			break;
		}
	}
}

Animation* AnimationPlayer::GetActiveAnimation()
{
	return &activeAnimationState->animation;
}
