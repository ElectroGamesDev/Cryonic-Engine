#include "AnimationPlayer.h"

void AnimationPlayer::Start()
{
	spriteRenderer = gameObject->GetComponent<SpriteRenderer>();

	// Todo: Move to Awake()/Constructor but after exposed variables are set. Having in Start could cause issues if user's change the animationGraph via a script
	if (animationGraph != nullptr)
	{
		SetAnimationGraph(animationGraph);
		ownsGraph = true;
	}
}


void AnimationPlayer::Update(float deltaTime)
{
	if (paused || animationGraph == nullptr || activeAnimationState == nullptr)
		return;

	if (spriteRenderer == nullptr)
	{
		spriteRenderer = gameObject->GetComponent<SpriteRenderer>();

		// Checks to see if the spriteRenderer is still nullptr after attempting to set it, then sends a warning if it previously existed but no longer does
		// If no spriteRenderer exists, its adding unnecessary overhead as its trying to find a new spriteRenderer each frame
		if (spriteRenderer == nullptr)
		{
			if (previouslyExisted)
			{
				previouslyExisted = false;
				ConsoleLogger::WarningLog("Animation Player - Game object \"" + gameObject->GetName() + "\" does not have a Sprite Renderer. Add one or remove/disable the Animation Player.", false);
			}
			return;
		}
	}

	previouslyExisted = true;
	if (spriteRenderer->IsActive())
	{
		if (timeElapsed >= activeAnimationState->animation.GetSpeed())
		{
			if (activeAnimationState->animation.IsLooped())
			{
				timeElapsed = 0;
				// Set animation back to the first one.
			}
			else
			{
				// Todo: Transition to another animation if there is another, or wait until a transition is available, or until looped is enabled, or until the active animation is changed
				return;
			}
		}

		// Using a variable so its easier to debug SetTexture if there is a syntax issue
		std::filesystem::path path = "";
#if !defined(EDITOR)
		path = std::filesystem::path(RaylibWrapper::GetWorkingDirectory()) / "Resources" / "Assets";
#endif
		int index = static_cast<int>(timeElapsed / (activeAnimationState->animation.GetSpeed() / activeAnimationState->animation.GetSprites()->size()));
		if (index != previousSprite)
		{
			spriteRenderer->SetTexture(path / (*activeAnimationState->animation.GetSprites())[index]);
			previousSprite = index;
		}
		timeElapsed += deltaTime;
	}
}

void AnimationPlayer::Destroy()
{
	if (animationGraph != nullptr && ownsGraph)
		delete animationGraph;
}

void AnimationPlayer::Play()
{
	Unpause();
}

void AnimationPlayer::Stop()
{
	Pause();
}

void AnimationPlayer::Pause()
{
	paused = true;
}

void AnimationPlayer::Unpause()
{
	paused = false;
}

bool AnimationPlayer::IsPaused()
{
	return paused;
}

void AnimationPlayer::SetAnimationGraph(AnimationGraph* animationGraph)
{
	if (this->animationGraph != nullptr && ownsGraph)
		delete this->animationGraph;
	ownsGraph = false;
	this->animationGraph = animationGraph;

	activeAnimationState = animationGraph->GetStartAnimationState();
	timeElapsed = 0.0f;
	previousSprite = -1;
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

	timeElapsed = 0.0f;
	previousSprite = -1;
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

	timeElapsed = 0.0f;
	previousSprite = -1;
}

Animation* AnimationPlayer::GetActiveAnimation()
{
	return &activeAnimationState->animation;
}
