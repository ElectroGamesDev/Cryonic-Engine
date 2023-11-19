#pragma once

#include "Component.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject& obj) : Component(obj)
	{
		runInEditor = true;
	}
	void Start() override {};
	void Update(float deltaTime) override;
	void Destroy() override;

	Model GetModel() const;
	void SetModel(Model model);
	std::filesystem::path GetModelPath() const;
	void SetModelPath(std::filesystem::path path);
	BoundingBox GetBounds() const;
	void SetBounds(BoundingBox bounds);

private:
	Model model;
	std::filesystem::path modelPath;
	BoundingBox bounds;
};