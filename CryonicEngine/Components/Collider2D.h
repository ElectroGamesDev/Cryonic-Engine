#pragma once

#include "Component.h"

class Collider2D : public Component {
public:
    Collider2D(GameObject* obj);
    void Start() override;

    // Render gizmos in Update()

    // Todo: Create an enum to select the type of collider for example; Box, Circle, Polygon/Convex, etc. Or maybe even an Array would work as it would be easier to detect as a variable
};
