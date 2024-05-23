#pragma once

#include "GameObject.h"
#include "RaylibWrapper.h"
#include <json.hpp>
#include <variant>

struct DataFile
{
	std::string path;
	nlohmann::json json;
};

class Editor
{
public:
	RaylibWrapper::RenderTexture2D* CreateModelPreview(std::filesystem::path modelPath, int textureSize);

	void InitFonts();
	void InitStyle();
	void InitMisc();
	void InitScenes();
	void InitImages();
	void Init();

	void RenderViewport();
	void RenderFileExplorerTreeNode(std::filesystem::path path, bool openOnDefault);
	void RenderFileExplorer();
	int RenderColorPicker(std::string name, ImVec2 position, ImVec4& selectedColor, ImVec4& previousColor); // Does this really need to be in here?
	void RenderProperties();
	bool RenderHierarchyNode(GameObject* gameObject, bool normalColor);
	void RenderHierarchy();
	void RenderTopbar();
	void RenderComponentsWin();
	void RenderScriptCreateWin();
	void RenderCameraView();
	void RenderConsole();
	void RenderAnimationGraph();
	void RenderProjectSettings();
	void Render();

	void UpdateViewport();
	void SetupViewport();

	void Cleanup();

	static RaylibWrapper::Camera camera;
	static RaylibWrapper::Camera2D camera2D;

	static std::variant<std::monostate, GameObject*, DataFile> objectInProperties;
};