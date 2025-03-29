#pragma once

#include "Component.h"
#include "../RaylibModelWrapper.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj, int id) : Component(obj, id)
	{
		runInEditor = true;
		name = "MeshRenderer";
		iconUnicode = "\xef\x86\xb2";

#if defined(EDITOR)
		std::string variables = R"(
        [
            0,
            [
                [
                    "bool",
                    "castShadows",
                    true,
                    "Cast Shadows"
                ]
            ]
        ]
    )";
		exposedVariables = nlohmann::json::parse(variables);
#endif
	}
	MeshRenderer* Clone() override
	{
		return new MeshRenderer(gameObject, -1);
	}
	/// Hide everything in this in the API
	void Start() override {};
	void Render(bool renderShadows) override;
#if defined(EDITOR)
	void EditorUpdate() override;
#endif
	void Destroy() override;

	RaylibModel& GetModel();
	void SetModel(ModelType model, std::filesystem::path path, Shaders shader);
	std::filesystem::path GetModelPath() const;
	void SetModelPath(std::filesystem::path path);

private:
	bool setShader = false;
	RaylibModel raylibModel;
	bool modelSet = false;
	std::filesystem::path modelPath;

	bool castShadows;
};