#pragma once

#include "ProjectManager.h"
#include "GameObject.h"

class Editor
{
public:
	RenderTexture2D* CreateModelPreview(std::filesystem::path modelPath, int textureSize);

	void InitFonts();
	void InitStyle();
	void InitMisc();
	void InitScenes();
	void InitImages();
	void Init(ProjectData _projectData);

	void RenderViewport();
	void RenderFileExplorer();
	void RenderProperties();
	bool RenderHierarchyNode(GameObject& gameObject, bool normalColor);
	void RenderHierarchy();
	void RenderTopbar();
	void RenderComponentsWin();
	void RenderScriptCreateWin();
	void RenderCameraView();
	void Render();

	void UpdateViewport();
	void SetupViewport();

	void Cleanup();

	static Camera camera;

private:
	ProjectData projectData;
};