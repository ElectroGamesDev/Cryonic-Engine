#pragma once

#include <string>
#include "../GameObject.h"
//class GameObject;

class Component
{
public:
    //Component(GameObject& obj);
    virtual ~Component() {};
    template<typename T>
    T& GetComponent()
    {
        return _gameObject.GetComponent<T>();
    }
    GameObject& GetGameObject()
    {
        return _gameObject;
    }

protected:
    virtual void Start() {};
    virtual void Update(float deltaTime) {};
    virtual void Destroy() {};

private:
    GameObject& _gameObject;
    friend class Scene;
};
