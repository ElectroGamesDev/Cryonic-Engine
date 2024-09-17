#pragma once

#include "json.hpp"
//#include "GUI/Element.h"
#include "GameObject.h"

namespace CanvasEditor
{
	bool RenderHierarchyNode(nlohmann::json* gameObject, bool normalColor, bool& childDoubleClicked);
	void RenderHierarchy();
	void Render();

	extern bool windowOpen;
	extern nlohmann::json canvasData;
}