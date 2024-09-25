#include "AudioClip.h"

bool AudioClip::LoadedInMemory() const
{
	return loadInMemory;
}

std::string AudioClip::GetPath() const
{
	return path;
}

Raylib::Sound* AudioClip::GetRaylibSound()
{
	return sound;
}
