#include "SpriteRenderer.h"
#include "../RaylibDrawWrapper.h"

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
        // Todo: Re-add this
        //DrawTexturePro(texture,
        //    {
        //    0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)
        //},
        //    {
        //    gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y,
        //        texture.width* gameObject->transform.GetScale().x,
        //        texture.height* gameObject->transform.GetScale().y
        //},
        //        {
        //        texture.width * gameObject->transform.GetScale().x / 2,
        //        texture.height * gameObject->transform.GetScale().y / 2
        //    },
        //        gameObject->transform.GetRotationEuler().y,
        //            WHITE);
    }
    else if (texturePath == "Square")
    {
        Vector3 position = gameObject->transform.GetPosition();
        Vector3 scale = gameObject->transform.GetScale();
        DrawRectangleWrapper(position.x, position.y, scale.x, scale.y, gameObject->transform.GetRotation().y, 255, 255, 255, 255);

    }
    else if (texturePath == "Circle")
    {
        Vector3 position = gameObject->transform.GetPosition();
        DrawCircleWrapper(position.x, position.y, gameObject->transform.GetScale().x, 255, 255, 255, 255);
    }
    else
    {
        // Invalid texture path
    }
}

void SpriteRenderer::Destroy()
{
    //if (textureSet)
    //    UnloadTexture(texture);
}