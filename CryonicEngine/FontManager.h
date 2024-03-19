#pragma once

#if defined(EDITOR)
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
//#include "rlImGui.h"
#include "imgui.h"

namespace FontManager
{
    ImFont* LoadFont(std::string font, int size, bool iconFont = false);
    ImFont* GetFont(std::string font, int size, bool checkIfExists = true);
    void LoadFonts(std::string font, std::vector<int> sizes);
    void UpdateFonts();
    void InitFontManager();
};
#endif