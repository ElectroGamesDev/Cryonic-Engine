#pragma once

#include <string>
//#include "GameObject.h"
class GameObject;

class Component
{
public:
    Component(GameObject* obj);
    virtual ~Component();
    virtual void Update(float deltaTime);
    virtual void Start();
    //void SetName(const std::string& name);
    GameObject* GetGameObject();
    //void SetGameObject(GameObject* object);
    //const std::string& GetName() const;
    //void SetRenderInEditMode(bool value);
    //bool GetRenderInEditMode();

private:
    //std::string componentName = "Unnamed Component";
    GameObject* gameObject;
    //bool updateInEditMode = false;
};
