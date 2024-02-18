#pragma once

#include "Component.h"

class SpriteRenderer : public Component
{
public:
	SpriteRenderer(GameObject* obj) : Component(obj)
	{
		runInEditor = true;
		name = "SpriteRenderer";
		iconUnicode = "\xef\x87\x85";
	}
	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

	Texture2D& GetTexture();
	void SetTexture(Texture2D texture);
	std::filesystem::path GetTexturePath() const;
	void SetTexturePath(std::filesystem::path path);

private:
	Texture2D texture;
	bool textureSet = false;
	bool color;
	std::filesystem::path texturePath;
};