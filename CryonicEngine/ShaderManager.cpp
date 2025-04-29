#include "ShaderManager.h"
//#include "Components/CameraComponent.h"
#include <filesystem>
#include "RaylibShaderWrapper.h"
#include "RaylibWrapper.h"
#ifndef EDITOR
#include "Game.h"
#endif

#if defined(PLATFORM_DESKTOP)  // I had to remove PLATFORM_DESKTOP from predefines to stop crash so this code is useless and GLSL_VERSION is always 100 right now.
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

void ShaderManager::Cleanup()
{
    for (auto it = RaylibShader::shaders.begin(); it != RaylibShader::shaders.end(); ++it)
    {
        it->second.Unload();
    }
    RaylibShader::shaders.clear();
}

void ShaderManager::Init()
{
    //shaders[ShaderManager::LitStandard] = LoadShader(("resources/shaders/glsl" + std::to_string(GLSL_VERSION) + "/lighting.vs").c_str(), ("resources/shaders/glsl" + std::to_string(GLSL_VERSION) + "/lighting.fs").c_str());

    // Todo: Replace glsl330 to GLSL_VERSION. I will need to define the platform though
#if defined (EDITOR)
    // Todo: This won't work for PC's other than mine
    RaylibShader::shaders[ShaderManager::LitStandard].Load((std::filesystem::path(__FILE__).parent_path() / "Resources/shaders/glsl330/lighting.vs").string().c_str(), (std::filesystem::path(__FILE__).parent_path() / "resources/shaders/glsl330/lighting.fs").string().c_str());
#else
    if (exeParent.empty())
    {
        RaylibShader::shaders[ShaderManager::LitStandard].Load("Resources/shaders/glsl330/lighting.vs", "Resources/shaders/glsl330/lighting.fs");
    }
    else
    {
        RaylibShader::shaders[ShaderManager::LitStandard].Load((std::filesystem::path(exeParent) / "Resources/shaders/glsl330/lighting.vs").string().c_str(), (std::filesystem::path(exeParent) / "Resources/shaders/glsl330/lighting.fs").string().c_str());
    }
#endif
    //std::string currentDirectory = GetWorkingDirectory();
    //std::string relativePath = "resources/shaders/glsl330/lighting.vs";

    //// Combine the current directory and relative path
    //std::string fullPath = currentDirectory + "/" + relativePath;

    //// Output the full path
    //ConsoleLogger::InfoLog("Full path: " + fullPath);
}

void ShaderManager::UpdateShaders(float cameraPosX, float cameraPosY, float cameraPosZ)
{
    for (auto it = RaylibShader::shaders.begin(); it != RaylibShader::shaders.end(); ++it)
        it->second.Update(cameraPosX, cameraPosY, cameraPosZ);
}