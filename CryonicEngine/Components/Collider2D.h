#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../box2d/box2d.h"
#endif

enum Shape
{
    Square,
    Circle
};

class Collider2D : public Component {
public:
    Collider2D(GameObject* obj, int id);
    Collider2D* Clone() override
    {
        return new Collider2D(gameObject, -1);
    }

    // Hide in API
    void Highlight(Color color, bool highlightChildren);

    void Start() override;
    void EditorUpdate() override;
    void Destroy() override;

    Shape GetShape();
    void SetTrigger(bool value);
    bool IsTrigger();
    void SetOffset(Vector2 offset);
    Vector2 GetOffset();
    void SetSize(Vector2 size);
    Vector2 GetSize();

private:
    Shape shape;
    bool trigger;
    Vector2 offset;
    Vector2 size = {1, 1};

#if !defined(EDITOR)
    b2FixtureDef fixtureDef;
    b2Body* body;
    b2Fixture* fixture;
#endif
};
