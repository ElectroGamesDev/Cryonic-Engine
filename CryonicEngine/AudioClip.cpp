#include "AudioClip.h"

bool AudioClip::ShouldLoadInMemory() const
{
	return loadInMemory;
}

std::string AudioClip::GetPath() const
{
	return path;
}