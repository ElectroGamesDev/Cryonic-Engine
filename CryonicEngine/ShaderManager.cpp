#include "ShaderManager.h"
#include "Components/CameraComponent.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)  // I had to remove PLATFORM_DESKTOP from predefines to stop crash so this code is useless and GLSL_VERSION is always 100 right now.
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

std::unordered_map<ShaderManager::Shaders, Shader> ShaderManager::shaders;

void ShaderManager::Cleanup()
{
    for (auto it = shaders.begin(); it != shaders.end(); ++it)
    {
        UnloadShader(it->second);
    }
    shaders.clear();
}

void ShaderManager::Init()
{
    //shaders[LitStandard] = LoadShader(("resources/shaders/glsl" + std::to_string(GLSL_VERSION) + "/lighting.vs").c_str(), ("resources/shaders/glsl" + std::to_string(GLSL_VERSION) + "/lighting.fs").c_str());
    shaders[LitStandard] = LoadShader("resources/shaders/glsl330/lighting.vs", "resources/shaders/glsl330/lighting.fs");

    // Can put below in a for loop
    shaders[LitStandard].locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shaders[LitStandard], "viewPos");
    int ambientLoc = GetShaderLocation(shaders[LitStandard], "ambient");
    float ambientValues[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(shaders[LitStandard], ambientLoc, ambientValues, SHADER_UNIFORM_VEC4);

}

void ShaderManager::UpdateShaders()
{
    if (CameraComponent::main == nullptr)
        return;

    for (auto it = shaders.begin(); it != shaders.end(); ++it)
    {
        float cameraPos[3] = {
            CameraComponent::main->gameObject->transform.GetPosition().x,
            CameraComponent::main->gameObject->transform.GetPosition().y,
            CameraComponent::main->gameObject->transform.GetPosition().z
        };
        SetShaderValue(it->second, it->second.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    }
}