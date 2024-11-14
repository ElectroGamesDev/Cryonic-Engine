#pragma once
#include <imgui_internal.h>
#include <string>

namespace AssetManager
{
	void RenderWindow();
	void CheckForUpdates();
	bool InstallAsset(std::string url);
	bool UpdateAsset(std::string url);
	bool FetchAssets();
	void Init(ImGuiWindowClass* windowClass);
	void Cleanup();

	extern bool open;
}