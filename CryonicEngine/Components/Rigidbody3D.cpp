#include "Rigidbody3D.h"
#if !defined(EDITOR)
// Todo: Remove unnecessary includes.
#include "../Game.h"
#include "../Jolt/RegisterTypes.h"
#include "../Jolt/Physics/PhysicsSystem.h"
#include "../Jolt/Physics/Collision/Shape/BoxShape.h"
#include "../Jolt/Physics/Collision/Shape/SphereShape.h"
#include "../Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "../Jolt/Physics/Collision/Shape/CompoundShape.h"
#include "../Jolt/Physics/Body/BodyCreationSettings.h"
#include "../Jolt/Physics/Body/BodyActivationListener.h"

static JPH::BodyInterface* Rigidbody3D::bodyInterface = nullptr;
#endif

Rigidbody3D::Rigidbody3D(GameObject* obj, int id) : Component(obj, id) {

    if (gameObject->GetComponent<Rigidbody3D>())
    {
        ConsoleLogger::ErrorLog("Failed to add Rigidbody3D to " + gameObject->GetName() + ". This game object already has a Rigidbody3D attached to it.");
        valid = false;
    }

    name = "Rigidbody3D";
    iconUnicode = "\xee\x96\x9d";
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
                ]
            ]
        ]
    )";
    exposedVariables = nlohmann::json::parse(variables);
#endif
}

void Rigidbody3D::Awake()
{
#if !defined(EDITOR)
    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();
    oldBodyType = bodyType;

    JPH::EMotionType eMotionType;

    switch (bodyType) {
        case Dynamic:
            eMotionType = JPH::EMotionType::Dynamic;
            break;
        case Kinematic:
            eMotionType = JPH::EMotionType::Kinematic;
            break;
        default:
            eMotionType = JPH::EMotionType::Static;
            break;
    }

    Vector3 goPosition = gameObject->transform.GetPosition();
    Quaternion goRotation = gameObject->transform.GetRotation();
    JPH::BodyCreationSettings bodySettings(compoundSettings.Create().Get(), { goPosition.x, goPosition.y, goPosition.z }, { goRotation.x, goRotation.y, goRotation.z, goRotation.w }, eMotionType, 0); // Last parameter is the layer
    bodySettings.mGravityFactor = gravityScale;
    bodyInterface->mMotionQuality = (value ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete);
    //bodySettings.mLinearDamping = linearDamping;
    //bodySettings.mLinearVelocity = angularDamping;
    // Todo: I can set the mUserData in body here, but this should be set in colliders/shaopes
    body = bodyInterface->CreateBody(bodySettings);
    bodyInterface->AddBody(body->GetBodyID());
#endif
}

void Rigidbody3D::Enable()
{
#if !defined(EDITOR)
    bodyInterface->AddBody(body->GetBodyID());
#endif

    for (Component* component : gameObject->GetComponents())
    {
        Collider3D* collider = dynamic_cast<Collider3D*>(component);
        if (collider)
        {
#if !defined(EDITOR)
            collider->SetRigidbody(this);
#endif
            colliders.push_back(collider);
        }
    }
}

void Rigidbody3D::Disable()
{
#if !defined(EDITOR)
    bodyInterface->RemoveBody(body->GetBodyID());
#endif

    for (Collider3D* collider : colliders)
        collider->RemoveRigidbody();
    colliders.clear();
}

void Rigidbody3D::Update() // Todo: should this be in the Physics Update?
{
    // Todo: Check if the game object or component is enabled/disabled, if it is then body->SetActive()
#if !defined(EDITOR)
    if (bodyType != oldBodyType)
        SetBodyType(bodyType);

    // Todo: Change this to a switch case
    if (bodyType == Dynamic)
    {
        // Todo: Change this to if-else and make one checking if they were both changed
        // Todo: This will update the body's rotation and position when it doesn't need to.
        if (gameObject->transform.GetPosition() == lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            gameObject->transform.SetPosition({ body->GetPosition().x, body->GetPosition().y, 0 });
        else if (gameObject->transform.GetPosition().x != body->GetTransform().p.x || gameObject->transform.GetPosition().y != body->GetTransform().p.y)
            bodyInterface->SetPosition(body->GetBodyID(), { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z }, JPH::EActivation::Activate)

        if (gameObject->transform.GetRotation() == lastGameObjectRotation) // Todo: This shouldn't be setting the gameobject's rotation even when the body hasn't changed
        {
            JPH::Vec3 rotation = bodyInterface->GetRotation(body->GetBodyID()).GetEulerAngles();
            gameObject->transform.SetRotationEuler({ rotation.x * RAD2DEG, rotation.z * RAD2DEG, rotation.z * RAD2DEG });
        }
        else if (DEG2RAD * gameObject->transform.GetRotation().y != body->GetTransform().q.GetAngle())
        {
            Quaternion rotation = gameObject->transform.GetPosition();
            bodyInterface->SetPosition(body->GetBodyID(), { rotation.x, rotation.y, rotation.z, rotation.w }, JPH::EActivation::Activate)
        }
    }
    else if (bodyType == Kinematic)
    {
        // Todo: Change this to if-else and make one checking if they were both changed
        if (gameObject->transform.GetPosition() != lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            bodyInterface->SetPosition(body->GetBodyID(), { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z }, JPH::EActivation::Activate)

        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
        {
            Quaternion rotation = gameObject->transform.GetPosition();
            bodyInterface->SetPosition(body->GetBodyID(), { rotation.x, rotation.y, rotation.z, rotation.w }, JPH::EActivation::Activate)
        }
    }
    else if (bodyType == Static)
    {
        if (gameObject->transform.GetPosition() != lastGameObjectPosition)
            gameObject->transform.SetPosition(lastGameObjectPosition);
        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
            gameObject->transform.SetRotation(lastGameObjectRotation);
    }

    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();

    oldBodyType = bodyType;
#endif
}

void Rigidbody3D::Destroy()
{
#if !defined(EDITOR)
    bodyInterface.RemoveBody(body->GetID());
    bodyInterface.DestroyBody(body->GetID());
#endif
}


void Rigidbody3D::SetPosition(Vector3 position)
{
    if (bodyType == Static)
    {
        gameObject->transform.SetPosition(position);
        ConsoleLogger::WarningLog("SetPosition() or MovePosition() is being called on \"" + gameObject->GetName() + "'s\" Rigidbody3D while the Rigidbody Body Type is static. If this Rigidbody is moved often, then it is recommended to make it Kinematic.");
    }
    if (bodyType == Kinematic) // Kinematic doesn't collide so the game object's position should be set here
        gameObject->transform.SetPosition(position);

#if !defined(EDITOR)
    bodyInterface->SetPosition(body->GetBodyID(), { position.x, position.y, position.z }, JPH::EActivation::Activate)
#endif
}

void Rigidbody3D::SetPosition(float x, float y, float z)
{
    SetPosition({ x, y, z });
}

void Rigidbody3D::MovePosition(Vector3 displacement)
{
#if !defined(EDITOR)
    JPH::Vec3 position = bodyInterface->GetPosition(body->GetBodyID());
    SetPosition({ position.x + displacement.x, position.y + displacement.y, position.z + displacement.z }});
#endif
}

void Rigidbody3D::MovePosition(float x, float y, float z)
{
    MovePosition({ x, y, z });
}

void Rigidbody3D::ApplyForce(Vector3 force)
{
#if !defined(EDITOR)
    bodyInterface->AddForce(body->GetBodyID(), { force.x, force.y, force.z }, JPH::EActivation::Activate);
#endif
}

void Rigidbody3D::ApplyForce(Vector3 force, Vector3 position)
{
#if !defined(EDITOR)
    bodyInterface->AddImpulse(body->GetBodyID(), { force.x, force.y, force.z }, { body->GetCenterOfMassPosition().x + position.x, body->GetCenterOfMassPosition().y + position.y, body->GetCenterOfMassPosition().z + position.z }, JPH::EActivation::Activate);
#endif
}

void Rigidbody3D::ApplyImpulse(Vector3 impulse)
{
#if !defined(EDITOR)
    bodyInterface->AddImpulse(body->GetBodyID(), { impulse.x, impulse.y, impulse.z }); 
#endif
}

void Rigidbody3D::ApplyImpulse(Vector3 impulse, Vector3 position)
{
#if !defined(EDITOR)
    bodyInterface->AddImpulse(body->GetBodyID(), { impulse.x, impulse.y, impulse.z }, { body->GetCenterOfMassPosition().x + position.x, body->GetCenterOfMassPosition().y + position.y, body->GetCenterOfMassPosition().z + position.z });
#endif
}

void Rigidbody3D::ApplyTorque(Vector3 torque)
{
#if !defined(EDITOR)
    bodyInterface->AddTorque(body->GetBodyID(), { torque.x, torque.y, torque.z }, JPH::EActivation::Activate);
#endif
}

void Rigidbody3D::SetBodyType(BodyType bodyType)
{
#if !defined(EDITOR)
    this->bodyType = bodyType;

    if (bodyType == Dynamic)
        bodyInterface->SetMotionType(body->GetBodyID(), JPH::EMotionType::Dynamic);
    else if (bodyType == Kinematic)
        bodyInterface->SetMotionType(body->GetBodyID(), JPH::EMotionType::Kinematic);
    else
        bodyInterface->SetMotionType(body->GetBodyID(), JPH::EMotionType::Static);
#endif
}

BodyType Rigidbody3D::GetBodyType()
{
    return bodyType;
}

void Rigidbody3D::SetGravityScale(float gravity)
{
#if !defined(EDITOR)
    body->SetGravityFactor(body->GetBodyID(), gravity);
#endif
}

float Rigidbody3D::GetGravityScale()
{
#if !defined(EDITOR)
    return body->GetGravityFactor();
#else
    return 0.0f;
#endif
}

void Rigidbody3D::SetContinuous(bool value)
{
#if !defined(EDITOR)
    bodyInterface->SetMotionQuality(body->GetBodyID(), (value ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete));
    continuous = value;
#endif
}

bool Rigidbody3D::IsContinuous()
{
#if !defined(EDITOR)
    return continuous;
#else
    return false;
#endif
}

// Jolt automatically calculates the mass. This will be added later.
void Rigidbody3D::SetMass(float mass)
{
    // Body Creation Settings - mMassPropertiesOverride
}

float Rigidbody3D::GetMass()
{
#if !defined(EDITOR)
    return mass;
#else
    return 0.0f;
#endif
}

void Rigidbody3D::SetLinearVelocity(Vector3 velocity)
{
#if !defined(EDITOR)
    body->SetLinearVelocity({ velocity.x, velocity.y, velocity.z });
#endif
}

Vector3 Rigidbody3D::GetLinearVelocity()
{
#if !defined(EDITOR)
    JPH::Vec3 velocity = body->GetLinearVelocity();
    return { velocity.x, velocity.y, velocity.x };
#else
    return {0,0,0};
#endif
}

void Rigidbody3D::SetAngularVelocity(Vector3 velocity)
{
#if !defined(EDITOR)
    body->SetAngularVelocity({ velocity.x, velocity.y, velocity.z });
#endif
}

Vector3 Rigidbody3D::GetAngularVelocity()
{
#if !defined(EDITOR)
    JPH::Vec3 velocity = body->GetAngularVelocity();
    return { velocity.x, velocity.y, velocity.z };
#else
    return {0,0,0};
#endif
}

#if !defined(EDITOR)
void Rigidbody3D::AddShape(int id, JPH::ShapeSettings* shape) // Todo: finish this // Should this be moved to Collider??
{
    //shapes.push_back({ id, *shape });
    //JPH::CompoundShapeSettings newCompoundSettings;

    //for (auto& shape : shapes)
    //    newCompoundSettings.AddShape(shape.second.mPosition, shape.second.mRotation, shape.second.mShape); // ShapeSetings nor Shape has position, rotation, scale, etc. subShape does have position and rotation though.

    //compoundSettings = newCompoundSettings;

    //bodyInterface->SetShape(body->GetBodyID(), compoundSettings.Create().Get(), true, JPH::EActivation::Activate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
}

void Rigidbody3D::RemoveShape(int id) // Todo: finish this // Should this be moved to Collider??
{
    //for (std::pair<int, JPH::Shape>& shape : shapes)
    //    if (shape->first == id)

    //compoundSettings = newCompoundSettings;

    //bodyInterface->SetShape(body->GetBodyID(), compoundSettings.Create().Get(), true, JPH::EActivation::Activate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
}
#endif