#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
#ifndef EDITOR
#include "Game.h"
#endif
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
		std::ifstream file;
#ifndef EDITOR
		file.open(std::filesystem::path(exeParent) / "Resources" / "Assets" / std::string(path + ".data"));
#endif
		if (!file.is_open())
		{
			ConsoleLogger::ErrorLog("Audio Clip failed to load. Path: " + path);
			return;
		}

		nlohmann::json jsonData;
		file >> jsonData;

		file.close();

		loadInMemory = jsonData["public"]["loadInMemory"].get<bool>();

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