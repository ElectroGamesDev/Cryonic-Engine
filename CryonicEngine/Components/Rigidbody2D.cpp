#include "Rigidbody2D.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#include "../Game.h"
#endif


Rigidbody2D::Rigidbody2D(GameObject* obj) : Component(obj) {
	name = "Rigidbody2D";

#if !defined(EDITOR)
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
	body = world->CreateBody(&bodyDef);
#endif
}

void Rigidbody2D::Update(float deltaTime)
{
	// This is probably not a good solution since then scripts can't set gameobject position by gameObject->transform.SetPositon
#if !defined(EDITOR)
	gameObject->transform.SetPosition({ body->GetPosition().x, body->GetPosition().y, 0});
	gameObject->transform._rotation.y = body->GetAngle() * RAD2DEG;
#endif
}
