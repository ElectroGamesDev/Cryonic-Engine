#include "Image.h"
#include "../RaylibWrapper.h"
#include "CameraComponent.h"
#if defined (EDITOR)
#else
#include "../imgui.h"
#endif

void Image::Awake()
{
#if defined(EDITOR)
    sprite = new Sprite(exposedVariables[1][0][2].get<std::string>()); // Todo: Handle if the path no longer exists
#endif
    //SetSprite(sprite);
}

void Image::RenderGui()
{
    Vector2 position = CameraComponent::main->GetWorldToScreen(gameObject->transform.GetPosition());

#if defined(EDITOR)
    // Divding positions by Raylib window size then multiply it by Viewport window size
    position.x = position.x / RaylibWrapper::GetScreenWidth() * ImGui::GetWindowSize().x;
    position.y = position.y / RaylibWrapper::GetScreenHeight() * ImGui::GetWindowSize().y;
#endif

    if (sprite->GetTexture())
    {
        RaylibWrapper::Texture2D* texture = sprite->GetTexture();

        position.x -= texture->width * gameObject->transform.GetScale().x / 2;
        position.y -= texture->height * gameObject->transform.GetScale().y / 2;

        ImGui::SetCursorPos({ position.x, position.y });

        //RaylibWrapper::Texture2D raylibTexture = { texture->id, texture->width, texture->height, texture->mipmaps, texture->format };
        //RaylibWrapper::rlImGuiImageSizeTintV(&raylibTexture, { texture->width * gameObject->transform.GetScale().x, texture->height * gameObject->transform.GetScale().y }, { (float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255 });
        RaylibWrapper::rlImGuiImageSizeV(texture, { texture->width * gameObject->transform.GetScale().x, texture->height * gameObject->transform.GetScale().y });
    }
    else if (sprite->GetPath() == "Square")
    {
        Vector3 position = gameObject->transform.GetPosition();
        Vector3 scale = gameObject->transform.GetScale();
        RaylibWrapper::DrawRectangleProFlipped({ position.x, position.y, scale.x * 3, scale.y * 3 },
            {
                scale.x * 3 / 2,
                scale.y * 3 / 2
            },
            gameObject->transform.GetRotationEuler().y,
            { color.r, color.g, color.b, color.a });
    }
    else if (sprite->GetPath() == "Circle")
    {
        Vector3 position = gameObject->transform.GetPosition();
        RaylibWrapper::DrawCircleSectorFlipped({ position.x, position.y }, gameObject->transform.GetScale().x * 1.5f, 0, 360, 36, { color.r, color.g, color.b, color.a });
    }
}

#if defined(EDITOR)
void Image::EditorUpdate()
{
    if (!setup)
    {
        Awake();
        setup = true;
    }

    color.r = exposedVariables[1][1][2][0].get<int>();
    color.g = exposedVariables[1][1][2][1].get<int>();
    color.b = exposedVariables[1][1][2][2].get<int>();
    color.a = exposedVariables[1][1][2][3].get<int>();

    if (sprite->GetPath() != exposedVariables[1][0][2])
    {
        SetSprite(new Sprite(exposedVariables[1][0][2].get<std::string>()));
        // Todo: Should it unload the old sprite?
    }
}
#endif

void Image::SetSprite(Sprite* sprite)
{
    this->sprite = sprite;
}

void Image::SetColor(Color color)
{
    this->color = color;
}

Color Image::GetColor() const
{
    return color;
}
