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
    Component(GameObject* obj);
    virtual ~Component() {};
    template<typename T>
    T& GetComponent()
    {
        return gameObject->GetComponent<T>();
    }
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

    void SetActive(bool active) { this->active = active; };
    bool IsActive() const { return active; };

    // Hide In API
    virtual void SetExposedVariables() {};

    virtual void Start() {};
    virtual void Update(float deltaTime) {};
    virtual void FixedUpdate(float fixedDeltaTime) {};
    virtual void EditorUpdate() {};
    virtual void Destroy() {};
    virtual void OnCollisionEnter2D(Collider2D* other) {};
    virtual void OnCollisionExit2D(Collider2D* other) {};
    virtual void OnCollisionStay2D(Collider2D* other) {};
    virtual void OnCollisionEnter3D(Collider3D* other) {};
    virtual void OnCollisionExit3D(Collider3D* other) {};
    virtual void OnCollisionStay3D(Collider3D* other) {};

    GameObject* gameObject;
};
