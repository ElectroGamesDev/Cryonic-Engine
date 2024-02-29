#include "Collider2D.h"
#include "Rigidbody2D.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#include "../Game.h"

b2PolygonShape shape;
b2FixtureDef fixtureDef; 
b2Body* body;
// Todo: Move above into Start()??
#endif

Collider2D::Collider2D(GameObject* obj) : Component(obj) {
	name = "Collider2D";
}

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
