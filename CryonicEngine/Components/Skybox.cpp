#include "Skybox.h"
#include "../RaylibDrawWrapper.h"
#include "../RaylibWrapper.h"
#include "../ProjectManager.h"

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
    // Todo: Don't render if its 2D
}

#if defined(EDITOR)
void SpriteRenderer::EditorUpdate()
{
    color.r = exposedVariables[1][0][2][0].get<int>();
    color.g = exposedVariables[1][0][2][1].get<int>();
    color.b = exposedVariables[1][0][2][2].get<int>();
    color.a = exposedVariables[1][0][2][3].get<int>();
}
#endif

void SpriteRenderer::Destroy()
{
    if (textureSet)
        RaylibWrapper::UnloadTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format });
}