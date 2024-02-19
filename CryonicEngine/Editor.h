#pragma once

#include "GameObject.h"
#include "RaylibWrapper.h"

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
	void RenderProperties();
	bool RenderHierarchyNode(GameObject* gameObject, bool normalColor);
	void RenderHierarchy();
	void RenderTopbar();
	void RenderComponentsWin();
	void RenderScriptCreateWin();
	void RenderCameraView();
	void RenderConsole();
	void Render();

	void UpdateViewport();
	void SetupViewport();

	void Cleanup();

	static RaylibWrapper::Camera camera;
	static RaylibWrapper::Camera2D camera2D;
};