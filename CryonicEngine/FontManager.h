#pragma once

#if defined(EDITOR)
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
//#include "rlImGui.h"
#include "imgui.h"

class FontManager
{
public:
    static ImFont* CreateFont(std::string font, int size, bool iconFont = false);
    static ImFont* GetFont(std::string font, int size, bool checkIfExists = true);
    static void CreateFonts(std::string font, std::vector<int> sizes);
    static void UpdateFonts();
    static void InitFontManager();
};
#endif