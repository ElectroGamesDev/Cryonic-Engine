#pragma once

#include <string>
#include "../GameObject.h"
//class GameObject;
#if defined(EDITOR)
#include <json.hpp>
#endif

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
    virtual void EditorUpdate() {};
    virtual void Destroy() {};

    GameObject* gameObject;
};
