#include "Rigidbody2D.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#include "../Game.h"
#endif

Rigidbody2D::Rigidbody2D(GameObject* obj, int id) : Component(obj, id) {
	name = "Rigidbody2D";
#if defined(EDITOR)
    std::string variables = R"(
        [
            0,
            [
                [
                    "BodyType",
                    "bodyType",
                    "Dynamic",
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

#else
    SetExposedVariables();

    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();

	b2BodyDef bodyDef;
    // Todo: Change this to a switch case
    // The exposed variables don't get set until after the cosntructor since the component doesn't have it's ID yet so bodyType is always Dynamic here
    if (bodyType == Dynamic)
    {
        bodyDef.type = b2_dynamicBody;
        //ConsoleLogger::ErrorLog("Dynamic");
    }
    else if (bodyType == Kinematic)
    {
        bodyDef.type = b2_kinematicBody;
        //ConsoleLogger::WarningLog("Kinematic");
    }
    else
        bodyDef.type = b2_staticBody;
	bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
	body = world->CreateBody(&bodyDef);
#endif
}

void Rigidbody2D::Update(float deltaTime) // Todo: should this be in the Physics Update?
{
    // Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive()
#if !defined(EDITOR)
    // Todo: Change this to a switch case
    if (bodyType == Dynamic)
    {
        //ConsoleLogger::ErrorLog("Dynamic 222222222");
        // Todo: This will update the body's rotation and position when it doesn't need to.
        if (gameObject->transform.GetPosition() == lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            gameObject->transform.SetPosition({ body->GetPosition().x, body->GetPosition().y, 0 });
        else if (gameObject->transform.GetPosition().x != body->GetTransform().p.x || gameObject->transform.GetPosition().y != body->GetTransform().p.y)
            body->SetTransform({ gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y }, body->GetAngle());

        if (gameObject->transform.GetRotation() == lastGameObjectRotation) // Todo: This shouldn't be setting the gameobject's rotation even when the body hasn't changed
            gameObject->transform.SetRotationEuler({ 0, body->GetAngle() * RAD2DEG, 0 });
        else if (DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle())
            body->SetTransform(body->GetPosition(), DEG2RAD * gameObject->transform.GetRotationEuler().y);
    }
    else if (bodyType == Kinematic)
    {
        //ConsoleLogger::WarningLog("Kinematic 11111");
        if (gameObject->transform.GetPosition() != lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            body->SetTransform({ gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y }, body->GetAngle());

        //if (gameObject->transform.GetRotation() != lastGameObjectRotation)
        if (gameObject->transform.GetRotation() != lastGameObjectRotation || DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle()) // Rotations on kinematic bodies can be changed with physics, I assume collision resolution. This fixes it
            body->SetTransform(body->GetPosition(), DEG2RAD * gameObject->transform.GetRotation().y);

        //if (gameObject->transform.GetPosition() == lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
        //    gameObject->transform.SetPosition({ body->GetPosition().x, body->GetPosition().y, 0 });
        //else if (gameObject->transform.GetPosition().x != body->GetTransform().p.x || gameObject->transform.GetPosition().y != body->GetTransform().p.y)
        //    body->SetTransform({ gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y }, body->GetAngle());

        //if (gameObject->transform.GetRotation() == lastGameObjectRotation) // Todo: This shouldn't be setting the gameobject's rotation even when the body hasn't changed
        //    gameObject->transform.SetRotationEuler({ 0, body->GetAngle() * RAD2DEG, 0 });
        //else if (DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle())
        //    body->SetTransform(body->GetPosition(), DEG2RAD * gameObject->transform.GetRotationEuler().y);
    }
    else if (bodyType == Static)
    {
        if (gameObject->transform.GetPosition() != lastGameObjectPosition)
            gameObject->transform.SetPosition(lastGameObjectPosition);
        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
            gameObject->transform._rotation.y = lastGameObjectRotation.y;
    }

    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();
#endif
}

void Rigidbody2D::Destroy()
{
#if !defined(EDITOR)
    world->DestroyBody(body);
#endif
}
