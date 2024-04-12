#include "Collider3D.h"
#include "Rigidbody3D.h"
#if !defined(EDITOR)
// Todo: Remove unnecessary includes.
#include "../Jolt/Jolt.h"
#include "../Jolt/RegisterTypes.h"
#include "../Jolt/Core/Factory.h"
#include "../Jolt/Core/TempAllocator.h"
#include "../Jolt/Core/JobSystemThreadPool.h"
#include "../Jolt/Physics/PhysicsSettings.h"
#include "../Jolt/Physics/PhysicsSystem.h"
#include "../Jolt/Physics/Collision/Shape/BoxShape.h"
#include "../Jolt/Physics/Collision/Shape/SphereShape.h"
#include "../Jolt/Physics/Body/BodyCreationSettings.h"
#include "../Jolt/Physics/Body/BodyActivationListener.h"

// Removed to fix "Color" conflictions
//using namespace JPH;

//#include "../Game.h"
//
//b2PolygonShape shape;
//b2FixtureDef fixtureDef; 
//b2Body* body;
// Todo: Move above into Start()??
#endif

Collider3D::Collider3D(GameObject* obj, int id) : Component(obj, id) {
	name = "Collider3D";
}

// Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive(). Also check if Rigidbody3D is destroyed, if it is then look for a new one, or create one. (Check when its destroyed in the Rigidbody3D Destroy() )

void Collider3D::Start()
{
//#if !defined(EDITOR)
	//Rigidbody3D* rb = gameObject->GetComponent<Rigidbody3D>();
	//if (rb == nullptr)
	//{
	//	b2BodyDef bodyDef;
	//	bodyDef.type = b2_staticBody;
	//	bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
	//	body = world->CreateBody(&bodyDef);
	//}
	//else
	//{
	//	fixtureDef.density = 1.0f;
	//	fixtureDef.friction = 0.3f;
	//	body = rb->body;
	//}

	//shape.SetAsBox(gameObject->transform.GetScale().x / 2, gameObject->transform.GetScale().y / 2);

	//fixtureDef.shape = &shape;
	//fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	//body->CreateFixture(&fixtureDef);
//#endif
}

void Collider3D::Destroy()
{
//#if !defined(EDITOR)
	//body->DestroyFixture(fixtureDef);
//#endif
}