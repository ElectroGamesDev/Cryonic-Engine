#pragma once

#include <string>
#include "../GameObject.h"
//class GameObject;
#if defined(EDITOR)
#include <json.hpp>
#endif

class Collider2D;
class Collider3D;

#define Expose
#define Hide

class Component
{
public:
    Component(GameObject* obj, int id = -1);
    virtual ~Component() {};
    GameObject* GetGameObject()
    {
        return gameObject;
    }
    
    // Hide in API
    std::string iconUnicode = "";
    std::string name;
    bool runInEditor = false;
    bool runInGame = true;
    // Hide In API
    bool active = true;
    // Hide In API
    int id;

#if defined(EDITOR)
    nlohmann::json exposedVariables = nullptr;
#endif

    void SetActive(bool active)
    {
        if (active == this->active)
            return;
        this->active = active;
        if (gameObject->IsGlobalActive())
        {
            if (active)
                Enable();
            else
                Disable();
        }
    };
    bool IsActive() const { return active; };

    // Hide In API
    virtual void SetExposedVariables() {};
    // Hide In API
    virtual Component* Clone() { return nullptr; };

    virtual void Start() {};
    virtual void Update() {};
    virtual void FixedUpdate() {};
    virtual void EditorUpdate() {};
    virtual void Destroy() {};
    // Called when the component or gameobject is enabled/activated
    virtual void Enable() {};
    // Called when the component or gameobject is disabled/deactivated
    virtual void Disable() {};
    virtual void OnCollisionEnter2D(Collider2D* other) {};
    virtual void OnCollisionExit2D(Collider2D* other) {};
    virtual void OnCollisionStay2D(Collider2D* other) {};
    virtual void OnCollisionEnter3D(Collider3D* other) {};
    virtual void OnCollisionExit3D(Collider3D* other) {};
    virtual void OnCollisionStay3D(Collider3D* other) {};

    GameObject* gameObject;
};
