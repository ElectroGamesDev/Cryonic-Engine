#pragma once

#include "Component.h"
#include "../CryonicCore.h"
#include <unordered_map>


class SpriteRenderer : public Component
{
public:
	SpriteRenderer(GameObject* obj, int id) : Component(obj, id)
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
                ],
				[
                    "int",
                    "renderOrder",
                    0,
                    "Render Order"
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

	//Texture2D& GetTexture();
	void SetTexture(std::filesystem::path path);
	std::filesystem::path GetTexturePath() const;

	Color tint = { 255, 255, 255, 255 };
	int renderOrder = 0;

	static std::unordered_map<std::filesystem::path, std::pair<Texture2D, int>> textures;

private:
	std::pair<Texture2D, int>* texture = nullptr;
	std::filesystem::path texturePath;
};