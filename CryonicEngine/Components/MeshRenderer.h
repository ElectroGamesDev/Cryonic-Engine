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
	}
	/// Hide everything in this in the API
	void Start() override {};
	void Update(float deltaTime) override;
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
};