#include "Lighting.h"
#include "../CryonicCore.h"
#include "../RaylibLightWrapper.h"
#include "../RaylibWrapper.h"
#if defined(EDITOR)
#include "../Editor.h"
//#include "../IconManager.h"
#else
#include "CameraComponent.h"
#endif

bool setup;
Vector3 lastPosition = {0,0,0};
std::deque<Lighting*> Lighting::lights;
int Lighting::nextId;
//RaylibLight raylibLight;
//Lighting* Lighting::main;
//bool Lighting::setMain = false;

// Using Awake and Destroy instead of Enable and Disable because it could have performance improvements, especially if games use flickering lights by enabling/disabling the component
void Lighting::Awake()
{
	//main = this;
    
    lightId = nextId;
    nextId++;
    shadowManager.Init(lightId);

    lights.push_back(this);
}

void Lighting::Destroy()
{
    auto it = std::find(lights.begin(), lights.end(), this);

    if (it != lights.end())
        lights.erase(it);
}

void Lighting::Enable()
{
    // Enabling the light in the shader // Todo: Make sure this actually works
    RaylibWrapper::Vector4 lightColorNormalized = RaylibWrapper::ColorNormalize({ color.r, color.g, color.b, 255 });
    RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.lightColLoc, &lightColorNormalized, RaylibWrapper::SHADER_UNIFORM_VEC4);
}

void Lighting::Disable()
{
    // Disabling the light in the shader // Todo: Make sure this actually works
    RaylibWrapper::Vector4 lightColorNormalized = RaylibWrapper::ColorNormalize({ color.r, color.g, color.b, 0 });
    RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.lightColLoc, &lightColorNormalized, RaylibWrapper::SHADER_UNIFORM_VEC4);
}

void Lighting::RenderLight(int index)
{
	//if (!setup)
	//{
	//	setup = true;
	//	Vector3 pos = gameObject->transform.GetPosition();
	//	raylibLight.Create(0, pos.x, pos.y, pos.z, 0, 0, 0, 255, 255, 255, 255, ShaderManager::LitStandard);
	//}

	//if (gameObject->transform.GetPosition() != lastPosition)
	//{
	//	Vector3 pos = gameObject->transform.GetPosition();
	//	raylibLight.SetPosition(pos.x, pos.y, pos.z);
	//}
	//raylibLight.Update(); // Todo: Check if I need this here and not in the if statement above
	//lastPosition = gameObject->transform.GetPosition(); // Todo: See if I can move this into the if statement above

    //if (CameraComponent::main != nullptr)
    //{
    //    Vector3 mainCameraPos = CameraComponent::main->gameObject->transform.GetPosition();
    //    shadowManager.camera.position = { mainCameraPos.x + Lighting::main->gameObject->transform.GetPosition().x,
    //        mainCameraPos.y + Lighting::main->gameObject->transform.GetPosition().y,
    //        mainCameraPos.z + Lighting::main->gameObject->transform.GetPosition().z };
    //}

    if (index > 15) // The index cant be higher than 15
        return;

#if defined(EDITOR)
    RaylibWrapper::SetShaderValue(ShadowManager::shader, ShadowManager::shader.locs[RaylibWrapper::SHADER_LOC_VECTOR_VIEW], &Editor::camera.position, RaylibWrapper::SHADER_UNIFORM_VEC3);
#else
    if (!CameraComponent::main)
        return;

    RaylibWrapper::SetShaderValue(ShadowManager::shader, ShadowManager::shader.locs[RaylibWrapper::SHADER_LOC_VECTOR_VIEW], &CameraComponent::main, RaylibWrapper::SHADER_UNIFORM_VEC3);
#endif

    if (lastRotation != gameObject->transform.GetRotation() && (shadowManager.lightType == 0 || shadowManager.lightType == 1)) // Rotation doesn't affect point lights
    {
        Vector3 rotation = gameObject->transform.GetRotationEuler() * DEG2RAD;
        RaylibWrapper::Matrix rotationMatrix = RaylibWrapper::MatrixRotateXYZ({ rotation.x, rotation.y, rotation.z });
        shadowManager.lightDir = RaylibWrapper::Vector3Normalize(RaylibWrapper::Vector3Transform({0, 0, -1}, rotationMatrix));
        lastRotation = gameObject->transform.GetRotation();
    }

    if (lastPosition != gameObject->transform.GetPosition())
    {
        lastPosition = gameObject->transform.GetPosition();
        shadowManager.camera.position = RaylibWrapper::Vector3Add({ lastPosition.x, lastPosition.y, lastPosition.z }, RaylibWrapper::Vector3Scale(shadowManager.lightDir, -15.0f)); // Todo: Remove the scale for point light
        shadowManager.lightPos = { lastPosition.x, lastPosition.y, lastPosition.z };
        RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.lightPosLoc, &shadowManager.lightPos, RaylibWrapper::SHADER_UNIFORM_VEC3);
    }

    RaylibWrapper::SetShaderValue(ShadowManager::shader, shadowManager.lightDirLoc, &shadowManager.lightDir, RaylibWrapper::SHADER_UNIFORM_VEC3);
    RaylibWrapper::SetShaderValue(ShadowManager::shader, shadowManager.lightPosLoc, &shadowManager.lightPos, RaylibWrapper::SHADER_UNIFORM_VEC3);

    RaylibWrapper::Matrix lightView;
    RaylibWrapper::Matrix lightProj;
    RaylibWrapper::BeginTextureMode(shadowManager.shadowMapTexture);
    RaylibWrapper::ClearBackground({ 255, 255, 255, 255 });
    RaylibWrapper::BeginMode3D(shadowManager.camera);
    lightView = RaylibWrapper::rlGetMatrixModelview();
    lightProj = RaylibWrapper::rlGetMatrixProjection();

    for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
    {
        if (!gameObject->IsActive()) continue;

        for (Component* component : gameObject->GetComponents())
        {
            if (component->IsActive())
                component->Render(true);
        }
    }

    RaylibWrapper::EndMode3D();
    RaylibWrapper::EndTextureMode();

    RaylibWrapper::SetShaderValueMatrix(ShadowManager::shader, shadowManager.lightVPLoc, RaylibWrapper::MatrixMultiply(lightView, lightProj));

    RaylibWrapper::rlActiveTextureSlot(lightId); // cant be more than 15
    RaylibWrapper::rlEnableTexture(shadowManager.shadowMapTexture.depth.id);

    RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.shadowMapLoc, &lightId, RaylibWrapper::SHADER_UNIFORM_INT);
}

#if defined(EDITOR)
void Lighting::EditorUpdate()
{
    bool isNowActive = active && gameObject->globalActive && gameObject->active;

    if (isNowActive != wasLastActive)
    {
        if (isNowActive)
            Enable();
        else
            Disable();

        wasLastActive = isNowActive;
    }


    if (color.r != exposedVariables[1][0][2][0].get<int>() ||
        color.g != exposedVariables[1][0][2][1].get<int>() ||
        color.b != exposedVariables[1][0][2][2].get<int>() ||
        color.a != exposedVariables[1][0][2][3].get<int>())
    {
        color.r = exposedVariables[1][0][2][0].get<int>();
        color.g = exposedVariables[1][0][2][1].get<int>();
        color.b = exposedVariables[1][0][2][2].get<int>();
        color.a = exposedVariables[1][0][2][3].get<int>();

        unsigned char alpha = 255;
        if (!isNowActive)
            alpha = 0;

        RaylibWrapper::Vector4 lightColorNormalized = RaylibWrapper::ColorNormalize({ color.r, color.g, color.b, alpha });
        RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.lightColLoc, &lightColorNormalized, RaylibWrapper::SHADER_UNIFORM_VEC4);
    }


    std::string typeString = "Point";
    if (type == Spot)
        typeString = "Spot";
    else if (type == Directional)
        typeString = "Directional";

    std::string currentTypeString = exposedVariables[1][1][2].get<std::string>();

    if (typeString != currentTypeString)
    {
        if (currentTypeString == "Point")
            type = Point;
        else if (currentTypeString == "Spot")
            type = Spot;
        else
            type = Directional;

        int value = static_cast<int>(type);
        RaylibWrapper::SetShaderValue(shadowManager.shader, shadowManager.lightTypeLoc, &value, RaylibWrapper::SHADER_UNIFORM_INT);
    }


	//Draw3DBillboard(Editor::camera, *IconManager::imageTextures["LightGizmoIcon"], gameObject->transform.GetPosition(), 2.0f, { 255, 255, 255, 150 }); // Todo: Re-add this
	//if (!setMain)
	//{
	//	setMain = true;
	//	main = this;
	//}
}
#endif