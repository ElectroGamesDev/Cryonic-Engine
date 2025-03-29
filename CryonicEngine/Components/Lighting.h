#pragma once

#include "Component.h"
#include <filesystem>
#include <deque>
#include "../ShadowManager.h"

class Lighting : public Component
{
public:
	Lighting(GameObject* obj, int id) : Component(obj, id)
	{
		name = "Lighting";
		iconUnicode = "\xef\x83\xab";
		runInEditor = true;
		Awake();

		std::string variables = R"(
        [
            0,
            [
                [
                    "Color",
                    "color",
                    [ 255, 255, 255, 255 ],
                    "Color"
                ],
                [
                    "Type",
                    "type",
                    "Point",
                    "Type",
                    [
                        "Point",
                        "Spot",
						"Directional"
                    ]
                ]
            ]
        ]
    )";
		exposedVariables = nlohmann::json::parse(variables);
	}

	enum Type
	{
		Point,
		Spot,
		Directional
	};

	Lighting* Clone() override
	{
		return new Lighting(gameObject, -1);
	}
	//void Start() override;
	void Awake() override;
	void Destroy() override;
	void Enable() override;
	void Disable() override;
	void RenderLight(int index);
#ifdef EDITOR
	void EditorUpdate() override;
#endif
	//void Destroy() override;

	//static Lighting* main; // Todo: Remove this
	//static bool setMain;
	static std::deque<Lighting*> lights;
	static int nextId;
	int lightId;

private:
	bool wasLastActive = false;
	bool wasGOLastActive = false;
	ShadowManager shadowManager;
	Vector3 lastPosition;
	Quaternion lastRotation;

	Color color;
	Type type;
};