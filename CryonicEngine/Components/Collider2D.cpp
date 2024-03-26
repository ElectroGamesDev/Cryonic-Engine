#include "Collider2D.h"
#include "Rigidbody2D.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#include "../Game.h"

// Todo: Move shape to Start()
b2PolygonShape shape;
b2FixtureDef fixtureDef; 
b2Body* body;
#endif

Collider2D::Collider2D(GameObject* obj) : Component(obj) {
	name = "Collider2D";
}

// Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive(). Also check if Rigidbody2D is destroyed, if it is then look for a new one, or create one. (Check when its destroyed in the Rigidbody2D Destroy() )

void Collider2D::Start()
{
#if !defined(EDITOR)
	Rigidbody2D* rb = gameObject->GetComponent<Rigidbody2D>();
	if (rb == nullptr)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
		body = world->CreateBody(&bodyDef);
	}
	else
	{
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		body = rb->body;
	}

	shape.SetAsBox(gameObject->transform.GetScale().x / 2, gameObject->transform.GetScale().y / 2);

	fixtureDef.shape = &shape;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	body->CreateFixture(&fixtureDef);
#endif
}

void Collider2D::Destroy()
{
#if !defined(EDITOR)
	body->DestroyFixture(fixtureDef);
#endif
}