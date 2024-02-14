#pragma once

#include "Component.h"

class SpriteRenderer : public Component
{
public:
	SpriteRenderer(GameObject* obj) : Component(obj)
	{
		runInEditor = true;
		name = "SpriteRenderer";
	}
	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

	Texture2D& GetTexture();
	void SetTexture(Texture2D texture);
	std::filesystem::path GetTexturePath() const;
	void SetTexturePath(std::filesystem::path path);

private:
	Expose Texture2D texture;
	bool textureSet = false;
	std::filesystem::path texturePath;
};