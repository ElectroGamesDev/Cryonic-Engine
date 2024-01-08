#pragma once

#include "Component.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* obj) : Component(obj)
	{
		runInEditor = true;
		name = "MeshRenderer";
	}
	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

	Model& GetModel();
	void SetModel(Model model);
	std::filesystem::path GetModelPath() const;
	void SetModelPath(std::filesystem::path path);
	BoundingBox GetBounds() const;
	void SetBounds(BoundingBox bounds);

private:
	bool setShader = false;
	Model model;
	bool modelSet = false;
	std::filesystem::path modelPath;
	BoundingBox bounds;
};