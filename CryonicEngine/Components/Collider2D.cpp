#include "Collider2D.h"
#include "Rigidbody2D.h"
#include "SpriteRenderer.h"
#include "../EventSystem.h"
#if !defined(EDITOR)
#include "../Game.h"
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
                ]
            ]
        ]
    )";
    exposedVariables = nlohmann::json::parse(variables);
#endif
}

void Test(GameObject* go)
{
	ConsoleLogger::ErrorLog("Object selected: " + go->GetName());
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
