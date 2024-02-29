#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif


class Rigidbody2D : public Component {
public:
    Rigidbody2D(GameObject* obj);
    void Update(float deltaTime) override;

    // Temporary solution, remove this
    int bodyType = 0;
    // Todo: Add an enum to select body type. Maybe do this by when looking in header if it finds "enum", it will get the next word knowing that is an enum variable type so then it can detect enums as an exposedVariable

    // Hide in API
#if !defined(EDITOR)
    b2Body* body = nullptr;
#endif
};
