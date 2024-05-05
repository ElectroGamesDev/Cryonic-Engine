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
}

AnimationGraph* AnimationPlayer::GetAnimationGraph()
{
	return animationGraph;
}
