#pragma once

#include "CryonicCore.h"
#include <string>
#include <vector>
#include <deque>
#include "ConsoleLogger.h"
#include <filesystem>
#include <memory>

class Component;

class GameObject
{
public:
    GameObject(int id = 0);
    ~GameObject();

    //Model GetModel() const;
    //void SetModel(Model model);
    //std::filesystem::path GetModelPath() const;
    //void SetModelPath(std::filesystem::path path);
    //BoundingBox GetBounds() const;
    //void SetBounds(BoundingBox bounds);
    //Material GetMaterial() const;
    //void SetMaterial(Material material);
    //std::string GetPath() const;
    //void SetPath(std::string path);
    std::string GetName() const;
    void SetName(std::string name);
    //Vector3 GetPosition() const;
    //void SetPosition(Vector3 position);
    //Vector3 GetRealSize() const;
    //Vector3 GetSize() const;
    //void SetSize(Vector3 size);
    //void SetRealSize(Vector3 realSize);
    //Quaternion GetRotation() const;
    //void SetRotation(Quaternion rotation);
    //Vector3 GetRotationEuler();
    //void SetRotationEuler();
    void SetActive(bool active);
    bool IsActive() const;
    int GetId() const;
    bool active = true;

    void SetParent(GameObject* gameObject);
    GameObject* GetParent();

    GameObject* GetChild(int index);
    GameObject* FindChild(std::string name);

    std::deque<GameObject*>& GetChildren();

    void SetSiblingIndex(int index);
    int GetSiblingIndex();

    std::deque<GameObject*>& GetSiblings();

    bool IsChild(GameObject& gameObject, GameObject* parent = nullptr);

    template <typename T>
    T& AddComponent() {
        T* newComponent = new T(this);
        static_cast<Component*>(newComponent)->gameObject = this; // Todo: This may cause a crash if its not a component
        //Component* componentPtr = static_cast<Component*>(newComponent);
        //if (componentPtr)
        //    componentPtr->gameObject = this;
        //else
        //    return nullptr;
        components.push_back(newComponent);
        return *newComponent;
    }

    template<typename T>
    bool RemoveComponent()
    {
        for (size_t i = 0; i < components.size(); ++i)
        {
            T* comp = dynamic_cast<T*>(components[i]);
            if (comp != nullptr)
            {
                delete comp;
                components.erase(components.begin() + i);
                return true;
            }
        }
        return false;
    }

    template<typename T>
    T* GetComponent()
    {
        for (Component* comp : components)
        {
            T* tcomp = dynamic_cast<T*>(comp);
            if (tcomp != nullptr)
            {
                return tcomp;
            }
        }
        return nullptr;
    }


    std::vector<Component*> GetComponents();
    //GameObject& operator=(const GameObject& other);
    bool operator==(const GameObject& other) const;
    bool operator!=(const GameObject& other) const;

    struct Transform
    {
        Vector3 _position = {0,0,0};
        Quaternion _rotation = Quaternion::Identity();
        Vector3 _scale = { 1,1,1 };


        void SetPosition(Vector3 position) { _position = position; }
        Vector3 GetPosition() { return _position; }

        void SetLocalPosition(Vector3 position) { _position = position; }
        Vector3 GetLocalPosition() { return _position; }

        void SetRotation(Quaternion rotation) { _rotation = rotation; }
        Quaternion GetRotation() { return _rotation; }

        /**
        Set the game object's rotation in degrees
        */
        void SetRotationEuler(Vector3 rotation) { _rotation = EulerToQuaternion((float)rotation.x * DEG2RAD, rotation.y * DEG2RAD, rotation.z * DEG2RAD); }
        /**
        Get the game object's rotation in degrees
        @return Vector3 euler of the rotation
        */
        Vector3 GetRotationEuler() { return QuaternionToEuler(_rotation) * RAD2DEG; }

        /**
        Set the game object's local rotation in degrees
        */
        void SetLocalRotationEuler(Vector3 rotation) { _rotation = EulerToQuaternion((float)rotation.x * DEG2RAD, rotation.y * DEG2RAD, rotation.z * DEG2RAD); }
        /**
        Get the game object's local rotation in degrees
        @return Vector3 euler of the rotation
        */
        Vector3 GetLocalRotationEuler() { return QuaternionToEuler(_rotation) * RAD2DEG; }

        void SetScale(Vector3 scale) { _scale = scale; }
        Vector3 GetScale() { return _scale; }

        void SetLocalScale(Vector3 scale) { _scale = scale; }
        Vector3 GetLocalScale() { return _scale; }

        Transform& operator=(const Transform& other) {
            if (this != &other) {
                _position = other._position;
                _rotation = other._rotation;
                _scale = other._scale;
            }
            return *this;
        }

        bool operator==(const Transform& other) const {
            return (_position.x == other._position.x &&
                _position.y == other._position.y &&
                _position.z == other._position.z &&
                _rotation.x == other._rotation.x &&
                _rotation.y == other._rotation.y &&
                _rotation.z == other._rotation.z &&
                _rotation.w == other._rotation.w &&
                _scale.x == other._scale.x &&
                _scale.y == other._scale.y &&
                _scale.z == other._scale.z);
        }

        bool operator!=(const Transform& other) const {
            return !(*this == other);
        }
    }; Transform transform;

private:
    //Model model;
    //std::filesystem::path modelPath;
    //BoundingBox bounds;
    //Material material;

    std::string name;
    int id;
    std::vector<Component*> components;
    GameObject* parentGameObject = nullptr;
    std::deque<GameObject*> childGameObjects;
};