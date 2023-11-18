#include "FontManager.h"
#include <vector>
#include <iostream>
#include <unordered_map> 
#include <utility>
#include "ConsoleLogger.h"
#include "imgui_impl_raylib.h"

static ImFont* defaultFont;
static std::vector<std::pair<std::string, int>> unloadedFonts;
static std::unordered_map<std::string, std::unordered_map<int, ImFont*>> fonts;
static bool updateFonts = false;

void FontManager::InitFontManager()
{
	defaultFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/fonts/Familiar-Pro-Bold.ttf", 16);
	Imgui_ImplRaylib_BuildFontAtlas();
}

ImFont* FontManager::CreateFont(std::string font, int size)
{
	//fonts[font][size] = io.Fonts->AddFontFromFileTTF(("resources/fonts/" + font +".ttf").c_str(), size);
	//Imgui_ImplRaylib_BuildFontAtlas();
	unloadedFonts.push_back(std::make_pair(font, size));
	ConsoleLogger::InfoLog(font + " font created at size " + std::to_string(size));
	//return fonts[font][size];
	updateFonts = true;
	return defaultFont; // Returns the default font since the new font can't be used until the next frame
}

void FontManager::CreateFonts(std::string font, std::vector<int> sizes)
{
	for (auto& size : sizes) CreateFont(font, size);
}

ImFont* FontManager::GetFont(std::string font, int size, bool checkIfExists)
{
	//for (auto& pair : fonts)
	//{
	//	if (pair.first != font) continue;
	//	for (auto& fontSize: pair.second)
	//	{
	//		if (fontSize.first != size) continue;
	//		return fontSize.second;
	//	}
	//}

	if (!checkIfExists)
		return fonts[font][size];

	auto fontIter = fonts.find(font);
	if (fontIter != fonts.end()) {
		auto& sizeMap = fontIter->second;
		auto sizeIter = sizeMap.find(size);

		if (sizeIter != sizeMap.end())
			return sizeIter->second;
	}

	return CreateFont(font, size);
}

void FontManager::UpdateFonts()
{
	if (!updateFonts) return;
	updateFonts = false;
	ImGuiIO& io = ImGui::GetIO();

	for (auto& font : unloadedFonts)
	{
		fonts[font.first][font.second] = io.Fonts->AddFontFromFileTTF(("resources/fonts/" + font.first + ".ttf").c_str(), font.second);
	}
	unloadedFonts.clear();

	Imgui_ImplRaylib_BuildFontAtlas();
}