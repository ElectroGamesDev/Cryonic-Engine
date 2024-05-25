#pragma once

#include "Component.h"
#include "../CryonicCore.h"


class Skybox : public Component
{
public:
	Skybox(GameObject* obj, int id) : Component(obj, id)
	{
		runInEditor = true;
		name = "Skybox";
		iconUnicode = "\xef\x87\xbe";

#if defined(EDITOR)
		std::string variables = R"(
        [
            0,
            [
                [
                    "Color",
                    "color",
                    [ 255, 255, 255, 255 ],
                    "color"
                ]
            ]
        ]
    )";
		exposedVariables = nlohmann::json::parse(variables);
#endif
	}
	Skybox* Clone() override
	{
		return new Skybox(gameObject, -1);
	}
	// Hide everything from API
	void Start() override {};
	void Update() override;
#if defined(EDITOR)
	void EditorUpdate() override;
#endif
	void Destroy() override;

	Texture2D& GetTexture();
	void SetTexture(Texture2D texture);
	std::filesystem::path GetTexturePath() const;
	void SetTexturePath(std::filesystem::path path);

	Color color = { 255, 255, 255, 255 };

private:
	Texture2D texture;
	bool textureSet = false;
	std::filesystem::path texturePath;
};