#pragma once

#ifdef IS2D
#include "box2d/box2d.h"
#endif
#include <filesystem>

extern std::filesystem::path exeParent;

#ifdef IS2D
extern b2World* world;
#endif