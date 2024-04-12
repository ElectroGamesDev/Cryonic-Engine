#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../Jolt/Jolt.h"
#endif


class Rigidbody3D : public Component {
public:
    Rigidbody3D(GameObject* obj, int id);
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
//#if !defined(EDITOR)
//    b2Body* body = nullptr;
//#endif
};
