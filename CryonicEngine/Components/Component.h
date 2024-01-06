#pragma once

#include <string>
#include "../GameObject.h"
//class GameObject;

class Component
{
public:
    Component(GameObject* obj) : gameObject(obj) {};
    virtual ~Component() {};
    template<typename T>
    T& GetComponent()
    {
        return gameObject.GetComponent<T>();
    }
    GameObject* GetGameObject()
    {
        return gameObject;
    }
    
    std::string name;
    bool runInEditor = false;
    bool runInGame = true;
    bool active = true;
    void SetActive(bool active) { this->active = active; };
    bool IsActive() const { return active; };

    virtual void Start() {};
    virtual void Update(float deltaTime) {};
    virtual void Destroy() {};

    GameObject* gameObject;
};
