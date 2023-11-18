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

template <typename T>
T* GameObject::GetComponent()
{
    if (components.data() == nullptr || components.empty()) return nullptr;
    for (auto& component : components)
    {
        if (component == nullptr) return nullptr;
        T* tcomponent = dynamic_cast<T*>(component);
        if (tcomponent) return tcomponent;
    }
    return nullptr;
}

template <typename T>
inline bool GameObject::AddComponent(T component)
{
    if (GetComponent<T>() != nullptr) return false;
    components.push_back(component);
    return true;
}

template <typename T>
inline bool GameObject::RemoveComponent(T component)
{
    auto it = std::remove(components.begin(), components.end(), component);
    if (it != components.end())
    {
        components.erase(it, components.end());
        return true;
    }
    return false;
}

//template <typename T>
//T* GameObject::GetComponent()
//{
//    for (auto* comp : components)
//    {
//        if (dynamic_cast<T*>(comp) != nullptr)
//        {
//            return static_cast<T*>(comp);
//        }
//    }
//    return nullptr;
//}


std::vector<Component>& GameObject::GetComponents()
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