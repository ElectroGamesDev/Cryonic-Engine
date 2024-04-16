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
                ],
                [
                    "float",
                    "gravityScale",
                    1.0,
                    "Gravity Scale"
                ],
                [
                    "float",
                    "mass",
                    1.0,
                    "Mass"
                ],
                [
                    "bool",
                    "continuousDetection",
                    false,
                    "Continuous Detection"
                ],
                [
                    "float",
                    "linearDamping",
                    0.0,
                    "Linear Damping"
                ],
                [
                    "float",
                    "angularDamping",
                    0.0,
                    "Angular Damping"
                ]
            ]
        ]
    )";
    exposedVariables = nlohmann::json::parse(variables);

#else
    SetExposedVariables(); // SetExposedVariables() is currently called on all components after the constructor. This should fix the issue.

    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();

	b2BodyDef bodyDef;
    // Todo: Change this to a switch case
    // The exposed variables don't get set until after the cosntructor since the component doesn't have it's ID yet so bodyType is always Dynamic here
    if (bodyType == Dynamic)
        bodyDef.type = b2_dynamicBody;
    else if (bodyType == Kinematic)
        bodyDef.type = b2_kinematicBody;
    else
        bodyDef.type = b2_staticBody;
	bodyDef.position.Set(gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y);
	body = world->CreateBody(&bodyDef);

    SetGravityScale(gravityScale);
    SetContinuous(continuousDetection);
    SetLinearDamping(linearDamping);
    SetAngularDamping(angularDamping);
    SetMass(mass);

    oldBodyType = bodyType;
#endif
}

void Rigidbody2D::SetPosition(Vector2 position)
{
#if !defined(EDITOR)
    body->SetTransform({ position.x, position.y }, body->GetAngle());
#endif
}

void Rigidbody2D::SetPosition(int x, int y)
{
    SetPosition({x, y});
}

void Rigidbody2D::MovePosition(Vector2 displacement)
{
#if !defined(EDITOR)
    body->SetAwake(true);
    body->SetTransform({ body->GetTransform().p.x + displacement.x, body->GetTransform().p.y + displacement.y}, body->GetAngle());
#endif
}

void Rigidbody2D::MovePosition(int x, int y)
{
    MovePosition({ x, y });
}

void Rigidbody2D::ApplyForce(Vector2 force)
{
#if !defined(EDITOR)
    body->ApplyForce({ force.x, force.y }, { 0, 0 }, true);
#endif
}

void Rigidbody2D::ApplyForce(Vector2 force, Vector2 position)
{
#if !defined(EDITOR)
    body->ApplyForce({force.x, force.y}, { position.x, position.y }, true);
#endif
}

void Rigidbody2D::ApplyImpulse(Vector2 impulse)
{
#if !defined(EDITOR)
    body->ApplyLinearImpulse({ impulse.x, impulse.y }, { 0, 0 }, true);
#endif
}

void Rigidbody2D::ApplyImpulse(Vector2 impulse, Vector2 position)
{
#if !defined(EDITOR)
    body->ApplyLinearImpulse({ impulse.x, impulse.y }, { position.x, position.y }, true);
#endif
}

void Rigidbody2D::ApplyTorque(float torque)
{
#if !defined(EDITOR)
    body->ApplyTorque(torque, true);
#endif
}

void Rigidbody2D::SetBodyType(BodyType bodyType)
{
#if !defined(EDITOR)
    this->bodyType = bodyType;

    if (bodyType == Dynamic)
        body->SetType(b2_dynamicBody);
    else if (bodyType == Kinematic)
        body->SetType(b2_kinematicBody);
    else
        body->SetType(b2_staticBody);
#endif
}

BodyType Rigidbody2D::GetBodyType()
{
    return bodyType;
}

void Rigidbody2D::SetGravityScale(float gravity)
{
#if !defined(EDITOR)
    body->SetGravityScale(gravity);
#endif
}

float Rigidbody2D::GetGravityScale()
{
#if !defined(EDITOR)
    return body->GetGravityScale();
#else
    return 0.0f;
#endif
}

void Rigidbody2D::SetContinuous(bool value)
{
#if !defined(EDITOR)
    body->SetBullet(value);
#endif
}

bool Rigidbody2D::IsContinuous()
{
#if !defined(EDITOR)
    return body->IsBullet();
#else
    return false;
#endif
}

void Rigidbody2D::SetMass(float mass)
{
#if !defined(EDITOR)
    this->mass = mass;
    for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        fixture->SetDensity(mass);
    body->ResetMassData();
#endif
}

float Rigidbody2D::GetMass()
{
#if !defined(EDITOR)
    return mass;
#else
    return 0.0f;
#endif
}

void Rigidbody2D::SetLinearDamping(float damping)
{
#if !defined(EDITOR)
    body->SetLinearDamping(damping);
#endif
}

float Rigidbody2D::GetLinearDamping()
{
#if !defined(EDITOR)
    return GetLinearDamping();
#else
    return 0.0f;
#endif
}

void Rigidbody2D::SetAngularDamping(float damping)
{
#if !defined(EDITOR)
    body->SetAngularDamping(damping);
#endif
}

float Rigidbody2D::GetAngularDamping()
{
#if !defined(EDITOR)
    return GetAngularDamping();
#else
    return 0.0f;
#endif
}

void Rigidbody2D::Update(float deltaTime) // Todo: should this be in the Physics Update?
{
    // Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive()
#if !defined(EDITOR)
    if (bodyType != oldBodyType)
        SetBodyType(bodyType);

    // Todo: Change this to a switch case
    if (bodyType == Dynamic)
    {
        // Todo: This will update the body's rotation and position when it doesn't need to.
        if (gameObject->transform.GetPosition() == lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            gameObject->transform.SetPosition({ body->GetPosition().x, body->GetPosition().y, 0 });
        else if (gameObject->transform.GetPosition().x != body->GetTransform().p.x || gameObject->transform.GetPosition().y != body->GetTransform().p.y)
        {
            body->SetAwake(true);
            body->SetTransform({ gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y }, body->GetAngle());
        }

        if (gameObject->transform.GetRotation() == lastGameObjectRotation) // Todo: This shouldn't be setting the gameobject's rotation even when the body hasn't changed
            gameObject->transform.SetRotationEuler({ 0, body->GetAngle() * RAD2DEG, 0 });
        else if (DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle())
        {
            body->SetAwake(true);
            body->SetTransform(body->GetPosition(), DEG2RAD * gameObject->transform.GetRotationEuler().y);
        }
    }
    else if (bodyType == Kinematic)
    {
        if (gameObject->transform.GetPosition() != lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            body->SetTransform({ gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y }, body->GetAngle());

        //if (gameObject->transform.GetRotation() != lastGameObjectRotation || DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle())
        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
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

    oldBodyType = bodyType;
#endif
}

void Rigidbody2D::Destroy()
{
#if !defined(EDITOR)
    world->DestroyBody(body);
#endif
}
