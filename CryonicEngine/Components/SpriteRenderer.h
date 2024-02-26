#pragma once

#include "Component.h"
#include "../CryonicCore.h"


class SpriteRenderer : public Component
{
public:
	SpriteRenderer(GameObject* obj) : Component(obj)
	{
		runInEditor = true;
		name = "SpriteRenderer";
		iconUnicode = "\xef\x87\x85";

#if defined(EDITOR)
		std::string variables = R"(
        [
            0,
            [
                [
                    "Color",
                    "tint",
                    [ 255, 255, 255, 255 ],
                    "Tint"
                ]
            ]
        ]
    )";
		exposedVariables = nlohmann::json::parse(variables);
#endif
	}
	// Hide everything from API
	void Start() override {};
	void Update(float deltaTime) override;
#if defined(EDITOR)
	void EditorUpdate() override;
#endif
	void Destroy() override;

	Texture2D& GetTexture();
	void SetTexture(Texture2D texture);
	std::filesystem::path GetTexturePath() const;
	void SetTexturePath(std::filesystem::path path);

private:
	Texture2D texture;
	bool textureSet = false;
	Expose Color tint = {255, 255, 255, 255};
	std::filesystem::path texturePath;
};