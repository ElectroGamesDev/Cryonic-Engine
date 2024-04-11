#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif


class Rigidbody2D : public Component {
public:
    Rigidbody2D(GameObject* obj);
    void Update(float deltaTime) override;
    void Destroy() override;

    enum BodyType
    {
        Dynamic,
        Kinematic,
        Static
    };

	BodyType bodyType = Dynamic;

    // Hide in API
#if !defined(EDITOR)
    b2Body* body = nullptr;

    Vector3 lastGameObjectPosition;
    Quaternion lastGameObjectRotation;
#endif
};
