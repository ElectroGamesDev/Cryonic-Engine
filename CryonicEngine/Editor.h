#pragma once

#include "ProjectManager.h"

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
	void RenderHierarchy();
	void RenderTopbar();
	void RenderComponentsWin();
	void Render();

	void UpdateViewport();
	void SetupViewport();

	void Cleanup();
private:
	Camera camera = { 0 };
	ProjectData projectData;
};