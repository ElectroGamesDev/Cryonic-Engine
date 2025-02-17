#pragma once

#include "Component.h"
#if !defined(EDITOR)
#include "../Jolt/Jolt.h"
#include "../Jolt/RegisterTypes.h"
#include "../Jolt/RegisterTypes.h"
#include "../Jolt/Physics/PhysicsSettings.h"
#include "../Jolt/Physics/PhysicsSystem.h"
#include "../Jolt/Physics/Collision/Shape/BoxShape.h"
#include "../Jolt/Physics/Collision/Shape/SphereShape.h"
#include "../Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "../Jolt/Physics/Collision/Shape/CompoundShape.h"
#include "../Jolt/Physics/Body/BodyCreationSettings.h"
#include "../Jolt/Physics/Body/BodyActivationListener.h"
#endif

class Rigidbody3D;

class Collider3D : public Component {
public:
    Collider3D(GameObject* obj, int id);
    Collider3D* Clone() override
    {
        return new Collider3D(gameObject, -1);
    }

    enum Shape
    {
        Box,
        Sphere
    };

    // Hide in API
    void Highlight(Color color, bool highlightChildren);
    // Hide in API
    void SetRigidbody(Rigidbody3D* rigidbody);
    // Hide in API
    void RemoveRigidbody();

    void Start() override;
    void Update() override;
    void EditorUpdate() override;
    void Destroy() override;
    void Enable() override;
    void Disable() override;

    Shape GetShape();
    void SetTrigger(bool value);
    bool IsTrigger();
    void SetOffset(Vector3 offset);
    Vector3 GetOffset();
    void SetSize(Vector3 size);
    Vector3 GetSize();

    // Hide in API
    bool highlight = false;
    Rigidbody3D* rb;
#if !defined(EDITOR)
    JPH::RefConst<JPH::Shape> joltShape;
#endif
private:
    Shape shape;
    bool trigger;
    Vector3 offset;
    Vector3 size = { 1, 1 };
    bool continuousDetection;
    Vector3 lastPosition;
    Quaternion lastRotation;

#if !defined(EDITOR)
    void CreateShape();

    JPH::Body* body = nullptr;
    bool ownBody = false;
#endif
};