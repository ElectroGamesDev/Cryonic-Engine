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
		nlohmann::json jsonArray = nlohmann::json::array();
		jsonArray.push_back(0);

		nlohmann::json variablesArray = nlohmann::json::array();
		variablesArray.push_back("Color");
		variablesArray.push_back("tint");
		nlohmann::json colorArray = nlohmann::json::array();
		colorArray.push_back("255");
		colorArray.push_back("255");
		colorArray.push_back("255");
		colorArray.push_back("255");
		variablesArray.push_back(colorArray);
		variablesArray.push_back("Tint");

		jsonArray.push_back(variablesArray);

		exposedVariables = jsonArray;
#endif
	}
	// Hide everything from API
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
	Expose Color color = {255, 255, 255, 255};
	std::filesystem::path texturePath;
};