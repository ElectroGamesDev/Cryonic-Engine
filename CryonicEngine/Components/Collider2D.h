#pragma once

#include "Component.h"

enum Shape
{
    Square,
    Circle
};

class Collider2D : public Component {
public:
    Collider2D(GameObject* obj, int id);
    void Start() override;
    void Destroy() override;

    Shape GetShape();
    void SetTrigger(bool value);
    bool IsTrigger();

private:
    Shape shape;
    bool trigger;

#if !defined(EDITOR)
    b2FixtureDef fixtureDef;
    b2Body* body;
    b2Fixture fixture;
#endif
};
