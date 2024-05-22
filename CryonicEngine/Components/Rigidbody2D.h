#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif

enum BodyType
{
    Dynamic,
    Kinematic,
    Static
};

class Rigidbody2D : public Component {
public:
    Rigidbody2D* Clone() override
    {
        return new Rigidbody2D(gameObject, -1);
    }
    void SetPosition(Vector2 position);
    void SetPosition(float x, float y);
    void MovePosition(Vector2 displacement);
    void MovePosition(float x, float y);
    void ApplyForce(Vector2 force);
    void ApplyForce(Vector2 force, Vector2 position);
    void ApplyImpulse(Vector2 impulse);
    void ApplyImpulse(Vector2 impulse, Vector2 position);
    void ApplyTorque(float torque);
    void SetBodyType(BodyType bodyType);
    BodyType GetBodyType();
    void SetGravityScale(float gravity);
    float GetGravityScale();
    void SetContinuous(bool value);
    bool IsContinuous();
    void SetMass(float mass);
    float GetMass();
    void SetLinearDamping(float damping);
    float GetLinearDamping();
    void SetAngularDamping(float damping);
    float GetAngularDamping();

	BodyType bodyType = Dynamic;

    Rigidbody2D(GameObject* obj, int id);
    void Update(float deltaTime) override;
    void Destroy() override;

    // Hide in API
#if !defined(EDITOR)
    b2Body* body = nullptr;

private:
    float gravityScale = 1.0f;
    bool continuousDetection = false;
    float mass = 1.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;

    Vector3 lastGameObjectPosition;
    Quaternion lastGameObjectRotation;
    BodyType oldBodyType;
#endif
};
