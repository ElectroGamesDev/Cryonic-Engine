#pragma once

#include "Component.h"

class Collider3D : public Component {
public:
    Collider3D(GameObject* obj, int id);
    void Start() override;
    void Destroy() override;

    // Todo: Create an enum to select the shape of the collider
};
