#include "SpriteRenderer.h"
#include "rlgl.h"
#include "../ShaderManager.h"

Texture2D& SpriteRenderer::GetTexture()
{
    return texture;
}

void SpriteRenderer::SetTexture(Texture2D texture)
{
    this->texture = texture;
    this->textureSet = true;
}

std::filesystem::path SpriteRenderer::GetTexturePath() const
{
    return texturePath;
}

void SpriteRenderer::SetTexturePath(std::filesystem::path path)
{
    this->texturePath = path;
}

void SpriteRenderer::Update(float deltaTime)
{
    if ((std::filesystem::exists(texturePath)))
    {
        DrawTexturePro(texture,
            {
            0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)
        },
            {
            gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y,
                texture.width* gameObject->transform.GetScale().x,
                texture.height* gameObject->transform.GetScale().y
        },
                {
                texture.width * gameObject->transform.GetScale().x / 2,
                texture.height * gameObject->transform.GetScale().y / 2
            },
                gameObject->transform.GetRotationEuler().y,
                    WHITE);
    }
    else if (texturePath == "Square")
    {
        DrawRectanglePro(
            {
                gameObject->transform.GetPosition().x,
                gameObject->transform.GetPosition().y,
                5 * gameObject->transform.GetScale().x,
                5 * gameObject->transform.GetScale().y
            },
            {
                5 * gameObject->transform.GetScale().x / 2,
                5 * gameObject->transform.GetScale().y / 2
            },
                gameObject->transform.GetRotation().y,
                WHITE);

    }
    else
    {
        // Invalid texture path
    }
}

void SpriteRenderer::Destroy()
{
    if (textureSet)
        UnloadTexture(texture);
}