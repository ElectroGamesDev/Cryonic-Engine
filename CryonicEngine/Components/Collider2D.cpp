#include "Collider2D.h"
#include "Rigidbody2D.h"
#include "SpriteRenderer.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#include "../Game.h"

// Todo: Move shape to Start()
b2PolygonShape shape;
b2FixtureDef fixtureDef; 
b2Body* body;
#endif

Collider2D::Collider2D(GameObject* obj, int id) : Component(obj, id) {
	name = "Collider2D";
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
	if (spriteRenderer != nullptr && spriteRenderer->GetTexturePath() == "Square")
		shape.SetAsBox(gameObject->transform.GetScale().x * 3 / 2, gameObject->transform.GetScale().y * 3 / 2);
	else if (spriteRenderer != nullptr && spriteRenderer->GetTexturePath() == "Circle")
		shape.SetAsBox(gameObject->transform.GetScale().x * 1.5f / 2, gameObject->transform.GetScale().y * 1.5f / 2);
	else // Assumes its a texture or doesn't have a sprite
		shape.SetAsBox(gameObject->transform.GetScale().x * 10 / 2, gameObject->transform.GetScale().y * 10 / 2);

	fixtureDef.shape = &shape;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	body->CreateFixture(&fixtureDef);
#endif
}

void Collider2D::Destroy()
{
#if !defined(EDITOR)
	world->DestroyBody(body);
#endif
}