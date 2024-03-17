#include "Skybox.h"
#include "../RaylibDrawWrapper.h"
#include "../RaylibWrapper.h"
#include "../ProjectManager.h"

Texture2D& Skybox::GetTexture()
{
    return texture;
}

void Skybox::SetTexture(Texture2D texture)
{
    this->texture = texture;
    this->textureSet = true;
}

std::filesystem::path Skybox::GetTexturePath() const
{
    return texturePath;
}

void Skybox::SetTexturePath(std::filesystem::path path)
{
    this->texturePath = path;
}

void Skybox::Update(float deltaTime)
{
    // Todo: Don't render if its 2D
}

#if defined(EDITOR)
void Skybox::EditorUpdate()
{
    color.r = exposedVariables[1][0][2][0].get<int>();
    color.g = exposedVariables[1][0][2][1].get<int>();
    color.b = exposedVariables[1][0][2][2].get<int>();
    color.a = exposedVariables[1][0][2][3].get<int>();
}
#endif

void Skybox::Destroy()
{
    if (textureSet)
        RaylibWrapper::UnloadTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format });
}