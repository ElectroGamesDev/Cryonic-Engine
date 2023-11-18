#include "GameObject.h"
#include "Scenes/SceneManager.h"
#include "ConsoleLogger.h"

GameObject::GameObject(int id)
{
    //this->model = model;
    //this->modelPath = modelPath;
    //this->bounds = bounds;
    //this->active = active;
    //this->name = name;
    this->id = id;
}

Model GameObject::GetModel() const
{
    return model;
}

void GameObject::SetModel(Model model)
{
    this->model = model;
    this->bounds = GetMeshBoundingBox(model.meshes[0]); // Todo: Set the correct scale depending on size variable.
}

std::filesystem::path GameObject::GetModelPath() const
{
    return modelPath;
}

void GameObject::SetModelPath(std::filesystem::path path)
{
    this->modelPath = path;
}

BoundingBox GameObject::GetBounds() const
{
    return bounds;
}

void GameObject::SetBounds(BoundingBox bounds)
{
    this->bounds = bounds;
}

bool GameObject::IsActive() const
{
    return active;
}

void GameObject::SetActive(bool active)
{
    this->active = active;
}

//Material GameObject::GetMaterial() const
//{
//    return material;
//}
//
//void GameObject::SetMaterial(Material material)
//{
//    this->material = material;
//}

//std::string GameObject::GetPath() const
//{
//    return path;
//}
//
//void GameObject::SetPath(std::string path)
//{
//    this->path = path;
//}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string name)
{
    this->name = name;
}


int GameObject::GetId() const
{
    return id;
}

bool GameObject::AddComponent(Component* component)
{
    // Todo: Check if a component already exists first and return true or false
    components.push_back(component);
    return true;
}

bool GameObject::RemoveComponent(Component* component)
{
    // Check if component exists and return true or false
    auto iter = std::find(components.begin(), components.end(), component);
    if (iter != components.end())
    {
        components.erase(iter);
        delete component;
    }
    return true;
}

std::vector<Component*> GameObject::GetComponents()
{
    return components;
}

bool GameObject::operator==(const GameObject& other) const
{
    return this->id == other.id;
}

bool GameObject::operator!=(const GameObject& other) const
{
    return this->id != other.id;
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other) {
        model = other.model;
        modelPath = other.modelPath;
        bounds = other.bounds;
        name = other.name;
        id = other.id;
        transform = other.transform;
    }
    return *this;
}

GameObject::~GameObject()
{

}