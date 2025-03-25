#include "Rigidbody3D.h"
#include <cstdint>
#if !defined(EDITOR)
// Todo: Remove unnecessary includes.
#include "../Game.h"
#include "../Jolt/RegisterTypes.h"
#include "../Jolt/Physics/PhysicsSystem.h"
#include "../Jolt/Physics/Collision/Shape/BoxShape.h"
#include "../Jolt/Physics/Collision/Shape/SphereShape.h"
#include "../Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "../Jolt/Physics/Collision/Shape/EmptyShape.h"
#include "../Jolt/Physics/Body/BodyCreationSettings.h"
#include "../Jolt/Physics/Body/BodyActivationListener.h"

JPH::BodyInterface* Rigidbody3D::bodyInterface = nullptr;
const JPH::BodyLockInterface* Rigidbody3D::bodyLockInterface = nullptr;
#endif

std::unordered_map<uint32_t, Collider3D*> Rigidbody3D::colliderMap;

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
                    "float",
                    "friction",
                    0.2,
                    "Friction"
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
    JPH::ObjectLayer layer;

    JPH::EMotionType eMotionType;

    switch (bodyType) {
        case Dynamic:
            eMotionType = JPH::EMotionType::Dynamic;
            layer = Layers::MOVING;
            break;
        case Kinematic:
            eMotionType = JPH::EMotionType::Kinematic;
            layer = Layers::MOVING;
            break;
        default:
            eMotionType = JPH::EMotionType::Static;
            layer = Layers::NON_MOVING;
            break;
    }

    Vector3 goPosition = gameObject->transform.GetPosition();
    Quaternion goRotation = gameObject->transform.GetRotation();
    JPH::MutableCompoundShapeSettings compoundSettings;
    JPH::EmptyShapeSettings emptyShape;
    compoundSettings.AddShape({ 0,0,0 }, { 0,0,0,1 }, emptyShape.Create().Get());
    compoundShape = static_cast<JPH::MutableCompoundShape*>(compoundSettings.Create().Get().GetPtr());
    JPH::BodyCreationSettings bodySettings(compoundShape, { goPosition.x, goPosition.y, goPosition.z }, { goRotation.x, goRotation.y, goRotation.z, goRotation.w }, eMotionType, layer); // Last parameter is the layer
    bodySettings.mGravityFactor = gravityScale;
    bodySettings.mFriction = friction;
    bodySettings.mMotionQuality = (continuousDetection ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete);
    //bodySettings.mLinearDamping = linearDamping;
    //bodySettings.mLinearVelocity = angularDamping;
    body = bodyInterface->CreateBody(bodySettings);
    bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
#endif
}

void Rigidbody3D::Enable()
{
#if !defined(EDITOR)
    //bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
    bodyInterface->ActivateBody(body->GetID()); // Todo: When the component is first created, this will activate it despite it already being activated from Awake()
#endif

    for (Component* component : gameObject->GetComponents())
    {
        Collider3D* collider = dynamic_cast<Collider3D*>(component);
        if (collider && !collider->IsTrigger())
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
    //bodyInterface->RemoveBody(body->GetID());
    bodyInterface->DeactivateBody(body->GetID());
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
            gameObject->transform.SetPosition({ body->GetPosition().GetX(), body->GetPosition().GetY(), body->GetPosition().GetZ() });
        else if (gameObject->transform.GetPosition().x != body->GetPosition().GetX() || gameObject->transform.GetPosition().y != body->GetPosition().GetY() || gameObject->transform.GetPosition().z != body->GetPosition().GetZ())
            bodyInterface->SetPosition(body->GetID(), { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z }, JPH::EActivation::DontActivate);

        if (gameObject->transform.GetRotation() == lastGameObjectRotation) // Todo: This shouldn't be setting the gameobject's rotation even when the body hasn't changed
        {
            //JPH::Vec3 rotation = bodyInterface->GetRotation(body->GetID()).GetEulerAngles();
            JPH::Quat rotation = body->GetRotation();
            gameObject->transform.SetRotation({ rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW() });
        }
        else if (gameObject->transform.GetRotation().x != body->GetRotation().GetX() || gameObject->transform.GetRotation().y != body->GetRotation().GetY() || gameObject->transform.GetRotation().z != body->GetRotation().GetZ() || gameObject->transform.GetRotation().w != body->GetRotation().GetW())
        {
            Quaternion rotation = gameObject->transform.GetRotation();
            bodyInterface->SetRotation(body->GetID(), { rotation.x, rotation.y, rotation.z, rotation.w }, JPH::EActivation::DontActivate);
        }
    }
    else if (bodyType == Kinematic)
    {
        // Todo: Change this to if-else and make one checking if they were both changed
        if (gameObject->transform.GetPosition() != lastGameObjectPosition) // Todo: This shouldn't be setting the gameobject's position even when the body hasn't changed
            bodyInterface->SetPosition(body->GetID(), { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetPosition().z }, JPH::EActivation::DontActivate);

        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
        {
            Quaternion rotation = gameObject->transform.GetRotation();
            bodyInterface->SetRotation(body->GetID(), { rotation.x, rotation.y, rotation.z, rotation.w }, JPH::EActivation::DontActivate);
        }
    }
    else if (bodyType == Static)
    {
        if (gameObject->transform.GetPosition() != lastGameObjectPosition)
        {
            Vector3 position = gameObject->transform.GetPosition();
            bodyInterface->SetPosition(body->GetID(), { position.x, position.y, position.z }, JPH::EActivation::DontActivate);
            //gameObject->transform.SetPosition(lastGameObjectPosition);
        }
        if (gameObject->transform.GetRotation() != lastGameObjectRotation)
        {
            Quaternion rotation = gameObject->transform.GetRotation();
            bodyInterface->SetRotation(body->GetID(), { rotation.x, rotation.y, rotation.z, rotation.w }, JPH::EActivation::DontActivate);
            //gameObject->transform.SetRotation(lastGameObjectRotation);
        }
    }

    lastGameObjectPosition = gameObject->transform.GetPosition();
    lastGameObjectRotation = gameObject->transform.GetRotation();

    oldBodyType = bodyType;
#endif
}

void Rigidbody3D::Destroy()
{
#if !defined(EDITOR)
    bodyInterface->RemoveBody(body->GetID());
    bodyInterface->DestroyBody(body->GetID());
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
    bodyInterface->SetPosition(body->GetID(), { position.x, position.y, position.z }, JPH::EActivation::DontActivate);
#endif
}

void Rigidbody3D::SetPosition(float x, float y, float z)
{
    SetPosition({ x, y, z });
}

void Rigidbody3D::MovePosition(Vector3 displacement)
{
#if !defined(EDITOR)
    JPH::Vec3 position = bodyInterface->GetPosition(body->GetID());
    SetPosition({ position.GetX() + displacement.x, position.GetY() + displacement.y, position.GetZ() + displacement.z });
#endif
}

void Rigidbody3D::MovePosition(float x, float y, float z)
{
    MovePosition({ x, y, z });
}

void Rigidbody3D::ApplyForce(Vector3 force)
{
#if !defined(EDITOR)
    bodyInterface->AddForce(body->GetID(), { force.x, force.y, force.z }, JPH::EActivation::DontActivate);
#endif
}

void Rigidbody3D::ApplyForce(Vector3 force, Vector3 position)
{
#if !defined(EDITOR)
    bodyInterface->AddForce(body->GetID(), { force.x, force.y, force.z }, { body->GetCenterOfMassPosition().GetX() + position.x, body->GetCenterOfMassPosition().GetY() + position.y, body->GetCenterOfMassPosition().GetZ() + position.z}, JPH::EActivation::DontActivate);
#endif
}

void Rigidbody3D::ApplyImpulse(Vector3 impulse)
{
#if !defined(EDITOR)
    bodyInterface->AddImpulse(body->GetID(), { impulse.x, impulse.y, impulse.z }); 
#endif
}

void Rigidbody3D::ApplyImpulse(Vector3 impulse, Vector3 position)
{
#if !defined(EDITOR)
    bodyInterface->AddImpulse(body->GetID(), { impulse.x, impulse.y, impulse.z }, { body->GetCenterOfMassPosition().GetX() + position.x, body->GetCenterOfMassPosition().GetY() + position.y, body->GetCenterOfMassPosition().GetZ() + position.z });
#endif
}

void Rigidbody3D::ApplyTorque(Vector3 torque)
{
#if !defined(EDITOR)
    bodyInterface->AddTorque(body->GetID(), { torque.x, torque.y, torque.z }, JPH::EActivation::DontActivate);
#endif
}

void Rigidbody3D::SetBodyType(BodyType bodyType)
{
#if !defined(EDITOR)
    this->bodyType = bodyType;

    if (bodyType == Dynamic)
    {
        bodyInterface->SetMotionType(body->GetID(), JPH::EMotionType::Dynamic, JPH::EActivation::DontActivate);
        bodyInterface->SetObjectLayer(body->GetID(), Layers::MOVING);
    }
    else if (bodyType == Kinematic)
    {
        bodyInterface->SetMotionType(body->GetID(), JPH::EMotionType::Kinematic, JPH::EActivation::DontActivate);
        bodyInterface->SetObjectLayer(body->GetID(), Layers::MOVING);
    }
    else
    {
        bodyInterface->SetMotionType(body->GetID(), JPH::EMotionType::Static, JPH::EActivation::DontActivate);
        bodyInterface->SetObjectLayer(body->GetID(), Layers::NON_MOVING);
    }
#endif
}

BodyType Rigidbody3D::GetBodyType()
{
    return bodyType;
}

void Rigidbody3D::SetGravityScale(float gravity)
{
#if !defined(EDITOR)
    bodyInterface->SetGravityFactor(body->GetID(), gravity);
#endif
}

float Rigidbody3D::GetGravityScale()
{
#if !defined(EDITOR)
    return bodyInterface->GetGravityFactor(body->GetID());
#else
    return 0.0f;
#endif
}

void Rigidbody3D::SetContinuous(bool continuous)
{
#if !defined(EDITOR)
    if (continuous == continuousDetection)
        return;

    bodyInterface->SetMotionQuality(body->GetID(), (continuous ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete));
    continuousDetection = continuous;
#endif
}

bool Rigidbody3D::IsContinuous()
{
#if !defined(EDITOR)
    return continuousDetection;
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
    //return mass;
    return body->GetShape()->GetMassProperties().mMass;
#else
    return 0.0f;
#endif
}

void Rigidbody3D::SetFriction(float value)
{
#if !defined(EDITOR)
    friction = value;
    body->SetFriction(friction);
#endif
}

float Rigidbody3D::GetFriction()
{
#if !defined(EDITOR)
    return friction;
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
    return { velocity.GetX(), velocity.GetY(), velocity.GetZ()};
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
    return { velocity.GetX(), velocity.GetY(), velocity.GetZ() };
#else
    return {0,0,0};
#endif
}

void Rigidbody3D::AddCollider(Collider3D* collider)
{
    colliders.push_back(collider);

#if !defined(EDITOR)
    // Todo: Check if there is any scenario where AddCollider or RemoveCollider would be called while the body does not exist. If it doesn't exist, is there even a point to updating the compoundShape since it should get overwritten  when the body is created
    if (body)
        JPH::BodyLockWrite lock(*bodyLockInterface, body->GetID()); // This is needed for multi-threading

    Vector3 position = gameObject->transform.GetPosition() + collider->GetOffset();
    Quaternion rotation = gameObject->transform.GetRotation();
    ////JPH::MutableCompoundShape* compoundShape = dynamic_cast<JPH::MutableCompoundShape*>(const_cast<JPH::Shape*>(body->GetShape()));
    compoundShape->AddShape({ position.x, position.y, position.z }, { rotation.x, rotation.y, rotation.z, rotation.w }, collider->joltShape, collider->id);
    // Todo: Do I need to fix the IDs from the other shapes now?
    colliderMap[collider->id] = collider;
    if (body)
        bodyInterface->NotifyShapeChanged(body->GetID(), body->GetCenterOfMassPosition(), true, JPH::EActivation::DontActivate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false

    //compoundShape->AddShape({ position.x, position.y, position.z }, { rotation.x, rotation.y, rotation.z, rotation.w }, collider->joltShape, collider->id);

    //if (body)
    //    bodyInterface->SetShape(body->GetID(), compoundSettings.Create().Get(), true, JPH::EActivation::DontActivate ); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
#endif
}

void Rigidbody3D::RemoveCollider(Collider3D* collider)
{
    auto it = std::find(colliders.begin(), colliders.end(), collider);
    if (it != colliders.end())
        colliders.erase(it);

#if !defined(EDITOR)
    ////JPH::MutableCompoundShape* compoundShape = dynamic_cast<JPH::MutableCompoundShape*>(const_cast<JPH::Shape*>(body->GetShape()));
    uint32_t numSubShapes = compoundShape->GetNumSubShapes();
    for (uint32_t i = 0; i < numSubShapes; ++i)
    {
        //if (compoundShape->GetSubShapes()[i].mUserData == reinterpret_cast<uint32_t>(collider))
        if (colliderMap[compoundShape->GetSubShapeUserData(compoundShape->GetSubShapeIDFromIndex(i, JPH::SubShapeIDCreator()).GetID())] == collider)
        {
            if (body)
                JPH::BodyLockWrite lock(*bodyLockInterface, body->GetID()); // This is needed for multi-threading
            compoundShape->RemoveShape(i);
            colliderMap.erase(collider->id);
            if (body)
                bodyInterface->NotifyShapeChanged(body->GetID(), body->GetCenterOfMassPosition(), true, JPH::EActivation::DontActivate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false

            break;
        }
    }

    //compoundSettings = new JPH::MutableCompoundShapeSettings();

    //for (Collider3D* currentCollider : colliders)
    //{
    //    JPH::Shape* shape = currentCollider->joltShape;
    //    Vector3 localPosition = currentCollider->gameObject->transform.GetLocalPosition();
    //    Quaternion localRotation = currentCollider->gameObject->transform.GetLocalRotation()
    //    m_compoundShape->AddShape({ localPosition.x, localPosition.y, localPosition.z }, { localRotation.x, localRotation.y, localRotation.z, localRotation.w }, shape);
    //}

    //if (body)
    //    bodyInterface->SetShape(body->GetID(), compoundSettings.Create().Get(), true, JPH::EActivation::DontActivate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
#endif
}

void Rigidbody3D::UpdateShape(Collider3D* collider, bool updateOnlyTransform)
{
#if !defined(EDITOR)
    ////JPH::MutableCompoundShape* compoundShape = dynamic_cast<JPH::MutableCompoundShape*>(const_cast<JPH::Shape*>(body->GetShape()));
    JPH::Vec3 position = { gameObject->transform.GetPosition().x + collider->GetOffset().x, gameObject->transform.GetPosition().y + collider->GetOffset().y, gameObject->transform.GetPosition().z + collider->GetOffset().z};
    JPH::Quat rotation = { gameObject->transform.GetRotation().x, gameObject->transform.GetRotation().y, gameObject->transform.GetRotation().z, gameObject->transform.GetRotation().w };
    uint32_t numSubShapes = compoundShape->GetNumSubShapes();
    for (uint32_t i = 0; i < numSubShapes; ++i)
    {
        //if (compoundShape->GetSubShapes()[i].mUserData == reinterpret_cast<uint32_t>(collider))
        if (colliderMap[compoundShape->GetSubShapeUserData(compoundShape->GetSubShapeIDFromIndex(i, JPH::SubShapeIDCreator()).GetID())] == collider)
        {
            if (body)
                JPH::BodyLockWrite lock(*bodyLockInterface, body->GetID()); // This is needed for multi-threading
            if (updateOnlyTransform)
                compoundShape->ModifyShape(i, position, rotation);
            else
                compoundShape->RemoveShape(i);
            break;
        }
    }

    if (!updateOnlyTransform)
        compoundShape->AddShape(position, rotation, collider->joltShape, collider->id);

    if (body)
        bodyInterface->NotifyShapeChanged(body->GetID(), body->GetCenterOfMassPosition(), true, JPH::EActivation::DontActivate); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
#endif
}

#if !defined(EDITOR)
//void Rigidbody3D::AddShape(int id, JPH::ShapeSettings* shape) // Todo: finish this // Should this be moved to Collider??
//{
//    shapes.push_back({ id, *shape });
//    JPH::CompoundShapeSettings newCompoundSettings;
//
//    for (auto& shape : shapes)
//        newCompoundSettings.AddShape(shape.second.mPosition, shape.second.mRotation, shape.second.mShape); // ShapeSetings nor Shape has position, rotation, scale, etc. subShape does have position and rotation though.
//
//    compoundSettings = newCompoundSettings;
//
//    bodyInterface->SetShape(body->GetID(), compoundSettings.Create().Get(), true, JPH::EActivation::DontActivate ); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
//}
//
//void Rigidbody3D::RemoveShape(int id) // Todo: finish this // Should this be moved to Collider??
//{
//    //for (std::pair<int, JPH::Shape>& shape : shapes)
//    //    if (shape->first == id)
//
//    //compoundSettings = newCompoundSettings;
//
//    //bodyInterface->SetShape(body->GetID(), compoundSettings.Create().Get(), true, JPH::EActivation::DontActivate ); // Todo: This will update the mass. If its using a custom mass, then the bool parameter should be set false
//}
#endif