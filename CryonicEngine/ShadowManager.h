#pragma once

#include "RaylibWrapper.h"
#include "GameObject.h"
#include "Components/Component.h"
#include "Scenes/SceneManager.h"
#include "RaylibCameraWrapper.h"

class ShadowManager
{
public:
    void Init(int id, int width = 1024, int height = 1024);
    ~ShadowManager();
    static void UnloadShader();
    RaylibWrapper::Matrix RenderShadowPass();
    void SetCamera();

    RaylibWrapper::RenderTexture2D shadowMapTexture;
    static RaylibWrapper::Shader shader;
    RaylibWrapper::Camera camera;
    RaylibWrapper::Vector3 lightDir;
    int lightDirLoc;
    int lightColLoc;
    int lightVPLoc;
    int shadowMapLoc;
private:
    Vector2 size;

    RaylibWrapper::RenderTexture2D LoadShadowMapRenderTexture();
    RaylibWrapper::RenderTexture2D shadowmap;
};
