#include "Collider2D.h"
#include "Rigidbody2D.h"
#include "SpriteRenderer.h"
#include "../EventSystem.h"
#if !defined(EDITOR)
#include "../Game.h"
#else
#include "../Editor.h"
#endif

Collider2D::Collider2D(GameObject* obj, int id) : Component(obj, id) {
	name = "Collider2D";
	iconUnicode = "\xef\x89\x8d";

#if defined(EDITOR)
    std::string variables = R"(
        [
            0,
            [
                [
                    "Shape",
                    "shape",
                    "Square",
                    "Shape",
                    [
                        "Square",
                        "Circle"
                    ]
                ],
                [
                    "bool",
                    "trigger",
                    false,
                    "Trigger"
                ],
                [
                    "Vector2",
                    "offset",
                    [0,0],
                    "Offset"
                ],
				[
                    "Vector2",
                    "size",
                    [1,1],
                    "Size"
                ]
            ]
        ]
    )";
    exposedVariables = nlohmann::json::parse(variables);
#endif
}

// Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive(). Also check if Rigidbody2D is destroyed, if it is then look for a new one, or create one. (Check when its destroyed in the Rigidbody2D Destroy() )

void Collider2D::Start() // Todo: Move to Awake()
{
#if !defined(EDITOR)
	Rigidbody2D* rb = gameObject->GetComponent<Rigidbody2D>();
	if (rb == nullptr)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
		body = world->CreateBody(&bodyDef);

		// Setting the density and friction in case a Rigidbody2D is added to the game object
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
	}
	else
	{
		fixtureDef.density = rb->GetMass();
		fixtureDef.friction = 0.3f;
		body = rb->body;
	}

	// Todo: This needs to be fixed. This is a horrible solution to check if the sprite is a texture or a specific built in shape to apply the right scale multiplier to it.
	SpriteRenderer* spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
	b2PolygonShape shape;
	if (spriteRenderer != nullptr && spriteRenderer->GetTexturePath() == "Square")
		shape.SetAsBox(gameObject->transform.GetScale().x * 3 / 2, gameObject->transform.GetScale().y * 3 / 2);
	else if (spriteRenderer != nullptr && spriteRenderer->GetTexturePath() == "Circle")
		shape.SetAsBox(gameObject->transform.GetScale().x * 1.5f / 2, gameObject->transform.GetScale().x * 1.5f / 2);
	else // Assumes its a texture or doesn't have a sprite
		shape.SetAsBox(gameObject->transform.GetScale().x * 10 / 2, gameObject->transform.GetScale().y * 10 / 2);

	fixtureDef.shape = &shape;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	fixture = body->CreateFixture(&fixtureDef);

	SetTrigger(trigger);


	// Subscribes to the ObjectSelected and ObjectDeselected editor events

#endif
	//EventSystem::Subscribe("ObjectSelected", Test);
	// Todo: Move this to the above if defined, and handle if the object is selected, deslected, changed, moved, or if this object moves.
	//EventSystem::Subscribe("ObjectSelected", [](GameObject* gameObject) {
	//	ConsoleLogger::InfoLog("Object selected: " + gameObject->GetName());
	//	});

	//EventSystem::Subscribe("ObjectMoved", [](GameObject* gameObject) {
	//	});

	//EventSystem::Subscribe("ObjectDeselected", [](GameObject* gameObject) {
	//	ConsoleLogger::InfoLog("Object deselected: " + gameObject->GetName());
	//	});

	//EventSystem::Subscribe("ObjectSelected", [](GameObject* gameObject) {});

}

void Collider2D::Highlight(Color color, bool highlightChildren)
{
	// Must go inside this if defined to prevent compiler error
#if defined(EDITOR)
	offset.x = exposedVariables[1][2][2][0].get<float>();
	offset.y = exposedVariables[1][2][2][1].get<float>();
	size.x = exposedVariables[1][3][2][0].get<float>();
	size.y = exposedVariables[1][3][2][1].get<float>();

	//ConsoleLogger::WarningLog("Highlighting " + gameObject->GetName());

	Vector3 position = gameObject->transform.GetPosition();
	Vector3 scale = gameObject->transform.GetScale();

	// Getting type from exposedVariables since shape variable isn't up-to-date with exposed variable
	std::string type = exposedVariables[1][0][2].get<std::string>();

	if (type == "Square")
	{
		RaylibWrapper::DrawRectangleOutline({ position.x + offset.x, position.y + offset.y, scale.x * 3 * size.x, scale.y * 3 * size.y },
			{ scale.y * 3 * size.x / 2, scale.y * 3 * size.y / 2 },
			gameObject->transform.GetRotationEuler().y,
			0.1f,
			{ color.r, color.g, color.b, color.a });
	}
	else if (type == "Circle")
	{
		// Todo: This is not a good solution
		for (int i = 0; i < 7; i++)
			RaylibWrapper::DrawCircleLinesV({ position.x + offset.x, position.y + offset.y }, scale.x * 1.5f * size.x + 0.01f * i, { color.r, color.g, color.b, color.a });
	}

	// Highlights child objects
	if (!highlightChildren)
		return;
	
	for (GameObject* child : gameObject->GetChildren())
	{
		// Using a bool so it only highlights children once
		bool colliderFound = false;
		for (Component* component : child->GetComponents())
		{
			Collider2D* collider = dynamic_cast<Collider2D*>(component);
			if (collider)
			{
				//ConsoleLogger::WarningLog("Highlighting child: " + collider->gameObject->GetName());
				collider->Highlight({ 24, 201, 24, 200 }, !colliderFound);
				colliderFound = true;
			}
		}
	}
#endif
}

void Collider2D::EditorUpdate()
{
	// Must be inside the if defined since the game compiled version doesn't include Varient
#if defined(EDITOR)
	// Todo: Replace this solution with Events. Another possible solution would be to have another EditorUpdate() with a selectedGameObject parameter, but this is still a bad solution.
	if (std::holds_alternative<GameObject*>(Editor::objectInProperties) && std::get<GameObject*>(Editor::objectInProperties) == gameObject)
		Highlight({ 0, 255, 0, 200 }, true);
#endif
}

void Collider2D::Destroy()
{
#if !defined(EDITOR)
	world->DestroyBody(body);
#endif
}

Shape Collider2D::GetShape()
{
	return shape;
}

void Collider2D::SetTrigger(bool value)
{
#if !defined(EDITOR)
	fixture->SetSensor(value);
#endif
}

bool Collider2D::IsTrigger()
{
#if !defined(EDITOR)
	return fixture->IsSensor();
#else
	return false;
#endif
}

void Collider2D::SetOffset(Vector2 offset)
{
	this->offset = offset;
}

Vector2 Collider2D::GetOffset()
{
	return offset;
}

void Collider2D::SetSize(Vector2 size)
{
	this->size = size;
}

Vector2 Collider2D::GetSize()
{
	return size;
}
