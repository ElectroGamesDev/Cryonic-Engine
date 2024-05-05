#pragma once

#include <string>
#include <vector>

class Animation
{
public:
	int GetId()
	{
		return id;
	}

	std::string GetName()
	{
		return name;
	}

	void SetLooped(bool loop)
	{
		this->loop = loop;
	}

	bool IsLooped()
	{
		return loop;
	}

	void SetSpeed(float speed)
	{
		this->speed = speed;
	}

	float GetSpeed()
	{
		return speed;
	}

	std::vector<std::string>& GetSprites()
	{
		return sprites;
	}

	// Hide in API
	int id;
	std::string name;
	bool loop;
	float speed;
	std::vector<std::string> sprites;
};