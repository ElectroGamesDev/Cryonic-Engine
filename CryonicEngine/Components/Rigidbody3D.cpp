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
#include "../Game.h"

using namespace JPH;

Vector3 lastGameObjectPosition;
Quaternion lastGameObjectRotation;

Body* body;

#endif

Rigidbody3D::Rigidbody3D(GameObject* obj) : Component(obj) {
	name = "Rigidbody3D";
#if defined(EDITOR)
    std::string variables = R"(
        [
            0,
            [
                [
                    "BodyType",
                    "bodyType",
                    "Dyanmic",
                    "BodyType",
                    [
                        "Dynamic",
                        "Kinematic",
                        "Static"
                    ]
                ]
            ]
        ]
    )";
    exposedVariables = nlohmann::json::parse(variables);
#endif

//#if !defined(EDITOR)
    //lastGameObjectPosition = gameObject->transform.GetPosition();
    //lastGameObjectRotation = gameObject->transform.GetRotation();

    //// Todo: Need to create shape based on the collider3D shape.

    //EMotionType eMotionType;
    //switch (bodyType)
    //{
    //case Rigidbody3D::Dynamic:
    //    eMotionType = EMotionType::Dynamic;
    //    break;
    //case Rigidbody3D::Kinematic:
    //    eMotionType = EMotionType::Kinematic;
    //    break;
    //default:
    //    eMotionType = EMotionType::Static;
    //    break;
    //}

    //// Todo: Jolt only allows 1 shape per body. Either create a rigidbody per collider, or somehow combinee the shapes into a compound shape

    //BodyCreationSettings bodySettings(new BoxShape(), RVec3(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z), Quat::sIdentity(), eMotionType, Layers::NON_MOVING);
    //body = bodyInterface.CreateBody(bodySettings);
    //bodyInterface.AddBody(body->GetID(), EActivation::DontActivate);
//#endif
}

void Rigidbody3D::Update(float deltaTime)
{
#if !defined(EDITOR)
    // Todo: Change this to a switch case
    if (bodyType == Dynamic)
    {
        if (gameObject->transform.GetPosition() == lastGameObjectPosition)
            gameObject->transform.SetPosition({ body->GetPosition().GetX(), body->GetPosition().GetY(), body->GetPosition().GetZ() });
        else
            body->MoveKinematic(RVec3(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z), body->GetRotation(), 0);

        if (gameObject->transform.GetRotation() == lastGameObjectRotation)
            gameObject->transform.SetRotation({ body->GetRotation().GetX() * RAD2DEG, body->GetRotation().GetY() * RAD2DEG, body->GetRotation().GetZ() * RAD2DEG });
        else
            body->MoveKinematic(body->GetPosition(), Quat(gameObject->transform.GetRotation().x * DEG2RAD, gameObject->transform.GetRotation().y * DEG2RAD, gameObject->transform.GetRotation().z * DEG2RAD, gameObject->transform.GetRotation().w), 0);

        lastGameObjectPosition = gameObject->transform.GetPosition();
        lastGameObjectRotation = gameObject->transform.GetRotation();
    }
    else if (bodyType == Kinematic)
    {
        if (gameObject->transform.GetPosition() != lastGameObjectPosition)
            body->MoveKinematic(RVec3(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z), body->GetRotation(), 0);

        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
            body->MoveKinematic(body->GetPosition(), Quat(gameObject->transform.GetRotation().x * DEG2RAD, gameObject->transform.GetRotation().y * DEG2RAD, gameObject->transform.GetRotation().z * DEG2RAD, gameObject->transform.GetRotation().w), 0);

        lastGameObjectPosition = gameObject->transform.GetPosition();
        lastGameObjectRotation = gameObject->transform.GetRotation();
    }
    else if (bodyType == Static)
    {
        gameObject->transform.SetPosition(lastGameObjectPosition);
        gameObject->transform._rotation.y = lastGameObjectRotation.y;
    }
#endif
}

void Rigidbody3D::Destroy()
{
#if !defined(EDITOR)
    bodyInterface->RemoveBody(body->GetID());
    bodyInterface->DestroyBody(body->GetID());
#endif
}
