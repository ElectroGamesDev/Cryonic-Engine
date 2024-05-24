#include "SpriteRenderer.h"
#include "../RaylibDrawWrapper.h"
#include "../RaylibWrapper.h"

std::unordered_map<std::filesystem::path, std::pair<Texture2D, int>> SpriteRenderer::textures;

//Texture2D& SpriteRenderer::GetTexture()
//{
//    return texture.first;
//}

void SpriteRenderer::SetTexture(std::filesystem::path path)
{
    if (texture != nullptr)
    {
        if (path == texturePath)
            return;
        Destroy();
    }

    if (path == "Square" || path == "Circle")
    {
        texturePath = path;
        return;
    }

    // Todo: Check to make sure the texture file exists, if it doesn't then replace it with some kind of NULL txture

    if (auto it = textures.find(path); it != textures.end())
    {
        texture = &(it->second);
        texture->second++;
    }
    else
    {
        RaylibWrapper::Texture2D tempTexture = RaylibWrapper::LoadTexture(path.string().c_str());
        textures[path] = std::make_pair(Texture2D{ tempTexture.id, tempTexture.width, tempTexture.height, tempTexture.mipmaps, tempTexture.format }, 1);
        texture = &textures[path];
    }

    texturePath = path;
}

std::filesystem::path SpriteRenderer::GetTexturePath() const
{
    return texturePath;
}

void SpriteRenderer::Update(float deltaTime)
{
    // Todo: I should create an enum to store whether if its a sqaure, circle, etc, if its a shape. It will be faster than comparing strings in an if-else
    if (texture != nullptr)
    {
        int xFlip = flipX ? -1 : 1;
        int yFlip = flipY ? 1 : -1; // The Y is already flipped

        RaylibWrapper::DrawTextureProFlipped({ texture->first.id, texture->first.width, texture->first.height, texture->first.mipmaps, texture->first.format },
            { 0, 0, static_cast<float>(texture->first.width) * xFlip, static_cast<float>(texture->first.height) * yFlip },
            { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, texture->first.width * gameObject->transform.GetScale().x / 10, texture->first.height* gameObject->transform.GetScale().y / 10 },
            { texture->first.width * gameObject->transform.GetScale().x / 10 / 2, texture->first.height * gameObject->transform.GetScale().y / 10 / 2 },
            gameObject->transform.GetRotationEuler().y,
            { tint.r, tint.g, tint.b, tint.a });
    }
    else if (texturePath == "Square")
    {
        Vector3 position = gameObject->transform.GetPosition();
        Vector3 scale = gameObject->transform.GetScale();
        //DrawRectangleWrapper(position.x, position.y, scale.x, scale.y, gameObject->transform.GetRotationEuler().y, tint.r, tint.g, tint.b, tint.a);
        RaylibWrapper::DrawRectangleProFlipped({ position.x, position.y, scale.x * 3, scale.y * 3 },
            {
                scale.x * 3 / 2,
                scale.y * 3 / 2
            },
            gameObject->transform.GetRotationEuler().y,
            { tint.r, tint.g, tint.b, tint.a });
    }
    else if (texturePath == "Circle")
    {
        Vector3 position = gameObject->transform.GetPosition();
        //DrawCircleWrapper(position.x, position.y, gameObject->transform.GetScale().x, tint.r, tint.g, tint.b, tint.a);
        RaylibWrapper::DrawCircleSectorFlipped({ position.x, position.y }, gameObject->transform.GetScale().x * 1.5f, 0, 360, 36, { tint.r, tint.g, tint.b, tint.a });
    }
    else
    {
        // Todo: Invalid texture path
    }
}

#if defined(EDITOR)
void SpriteRenderer::EditorUpdate()
{
    tint.r = exposedVariables[1][0][2][0].get<int>();
    tint.g = exposedVariables[1][0][2][1].get<int>();
    tint.b = exposedVariables[1][0][2][2].get<int>();
    tint.a = exposedVariables[1][0][2][3].get<int>();

    renderOrder = exposedVariables[1][1][2].get<int>();

    flipX = exposedVariables[1][2][2].get<bool>();
    flipY = exposedVariables[1][3][2].get<bool>();
}
#endif

void SpriteRenderer::Destroy()
{
    if (texture == nullptr)
        return;

    texture->second--;
    if (texture->second <= 0)
    {
        RaylibWrapper::UnloadTexture({ texture->first.id, texture->first.width, texture->first.height, texture->first.mipmaps, texture->first.format });
        auto it = textures.begin();
        while (it != textures.end())
        {
            if (&it->second == texture)
            {
                it = textures.erase(it);
                break;
            }
            else
                ++it;
        }
    }
    texture = nullptr;
}