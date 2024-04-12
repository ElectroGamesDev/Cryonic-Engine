#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif


class Rigidbody2D : public Component {
public:
    Rigidbody2D(GameObject* obj, int id);
    void Update(float deltaTime) override;
    void Destroy() override;

    enum BodyType
    {
        Dynamic,
        Kinematic,
        Static
    };

    void SetPosition(Vector2 position);
    void MovePosition(Vector2 displacement);
    void ApplyForce(Vector2 force);
    void ApplyForce(Vector2 force, Vector2 position = {0,0});
    void ApplyImpulse(Vector2 impulse);
    void ApplyImpulse(Vector2 impulse, Vector2 position = { 0,0 });
    void ApplyTorque(float torque);
    void SetType(BodyType bodyType);

	BodyType bodyType = Dynamic;

    // Hide in API
#if !defined(EDITOR)
    b2Body* body = nullptr;

    Vector3 lastGameObjectPosition;
    Quaternion lastGameObjectRotation;
    BodyType oldBodyType;
#endif
};
