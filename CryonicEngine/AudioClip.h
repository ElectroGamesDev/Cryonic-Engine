#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
// This is used by AudioPlayer. It's in here since AudioPlayer.h couldn't include it without it using the wrong include path for game builds.
namespace Raylib
{
#include "raylib.h"
}

class AudioClip
{
public:
	AudioClip(std::string path)
	{
		std::ifstream file(std::filesystem::path(RaylibWrapper::GetWorkingDirectory()) / "Resources" / "Assets" / std::string(path + ".data")); // Todo: Should I just use Filesystem GetWorkingPath? GetWorkingPath isn't even the best solution here

		if (!file.is_open())
		{
			ConsoleLogger::ErrorLog("Audio Clip failed to load. Path: " + path);
			return;
		}

		nlohmann::json jsonData;
		file >> jsonData;

		file.close();

		loadInMemory = jsonData["loadInMemory"].get<bool>();

		this->path = path;
	}

	/**
	 * @brief Returns whether an audio clip should be loaded into memory or if it should be streamed.
	 *
	 * @return [bool] True if the audio clip should be loaded into memory, false if it should is streamed.
	 */
	bool ShouldLoadInMemory() const; // This is useless to scripting API, but is useful for AudioPlayer.

	/**
	 * @brief Returns the relative path to the audio clip file.
	 *
	 * @return [std::string] The relative path to the audio clip file.
	 */
	std::string GetPath() const;

private:
	std::string path = "";
	bool loadInMemory = false; // True = load the audio clip into the disk, False = stream the audio clip
};