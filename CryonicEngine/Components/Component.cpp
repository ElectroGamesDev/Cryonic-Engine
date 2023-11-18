#include "Component.h"
#include "../ConsoleLogger.h"
#include "../GameObject.h"

Component::Component(GameObject* obj)
    : gameObject(obj)
{
}

Component::~Component()
{
}


void Component::Update(float deltaTime)
{
    // Implementation of the Update function
}

void Component::Start()
{
    // Implementation of the Start function
}

//void Component::SetName(const std::string& name) {
//    componentName = name;
//}

GameObject* Component::GetGameObject() {
    return gameObject;
}

/**
 * This function should not be used in behaviors, it is for internal use only.
 */
//void Component::SetGameObject(GameObject* object) {
//    gameObject = object;
//}

//const std::string& Component::GetName() const {
//    return componentName;
//}

//void Component::SetRenderInEditMode(bool value) {
//    updateInEditMode = value;
//}
//
//bool Component::GetRenderInEditMode()
//{
//    return updateInEditMode;
//}