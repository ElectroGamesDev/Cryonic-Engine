#pragma once

#include "Component.h"
#include "Collider3D.h"
#include <deque>
#include <unordered_map>
#if !defined(EDITOR)
#include "../Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/MutableCompoundShape.h"
#include "../Jolt/Physics/PhysicsSystem.h"
#endif

class Rigidbody3D : public Component {
public:
    Rigidbody3D* Clone() override
    {
        return new Rigidbody3D(gameObject, -1);
    }
    void SetPosition(Vector3 position);
    void SetPosition(float x, float y, float z);
    void MovePosition(Vector3 displacement);
    void MovePosition(float x, float y, float z);
    void ApplyForce(Vector3 force);
    void ApplyForce(Vector3 force, Vector3 position);
    void ApplyImpulse(Vector3 impulse);
    void ApplyImpulse(Vector3 impulse, Vector3 position);
    void ApplyTorque(Vector3 torque);
    void SetBodyType(BodyType bodyType);
    BodyType GetBodyType();
    void SetGravityScale(float gravity);
    float GetGravityScale();
    void SetContinuous(bool continuous);
    bool IsContinuous();
    void SetMass(float mass);
    float GetMass();
    void SetFriction(float value);
    float GetFriction();
    void SetLinearVelocity(Vector3 velocity);
    Vector3 GetLinearVelocity();
    void SetAngularVelocity(Vector3 velocity);
    Vector3 GetAngularVelocity();

    BodyType bodyType = Dynamic;

    Rigidbody3D(GameObject* obj, int id);
    void Awake() override;
    void Update() override;
    void Destroy() override;
    void Enable() override;
    void Disable() override;

    // Hide in API
    std::deque<Collider3D*> colliders;
    static std::unordered_map<uint32_t, Collider3D*> colliderMap; // This is static so the body userData doesn't need to hold Rigidbody, this will be easier to check if its holding a Collider3D, although consider making it not static and compare performance.
    //std::vector<std::pair<int, std::string>> shapes; // REMOVE ME

    void AddCollider(Collider3D* collider);
    void RemoveCollider(Collider3D* collider);
    void UpdateShape(Collider3D* collider, bool updateOnlyTransform = false);

    // Hide in API
#if !defined(EDITOR)
    //void AddShape(int id, JPH::ShapeSettings* shape); // Should this be moved to Collider??
    //void RemoveShape(int id); // Should this be moved to Collider??

    JPH::Body* body = nullptr;
    static JPH::BodyInterface* bodyInterface;
    static const JPH::BodyLockInterface* bodyLockInterface;
    //JPH::MutableCompoundShapeSettings compoundSettings;
    JPH::Ref<JPH::MutableCompoundShape> compoundShape;
    //std::vector<std::pair<int, JPH::ShapeSettings>> shapes; // id, shape

private:
    float gravityScale = 1.0f;
    bool continuousDetection = false;
    float mass = 1.0f;
    float friction = 0.2f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;

    Vector3 lastGameObjectPosition;
    Quaternion lastGameObjectRotation;
    BodyType oldBodyType;
#endif
};
