#pragma once

#include "CryonicCore.h"
#include <string>
#include <vector>
#include <deque>
#include "ConsoleLogger.h"
#include <filesystem>
#include <memory>
#include <algorithm>

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
        T* newComponent = new T(this, -1);
        static_cast<Component*>(newComponent)->gameObject = this; // Todo: This may cause a crash if its not a component
        //Component* componentPtr = static_cast<Component*>(newComponent);
        //if (componentPtr)
        //    componentPtr->gameObject = this;
        //else
        //    return nullptr;
        components.push_back(newComponent);
        return *newComponent;
    }

    // Hide in API
    template <typename T>
    T& AddComponent(int id) {
        T* newComponent = new T(this, id);
        static_cast<Component*>(newComponent)->gameObject = this; // Todo: This may cause a crash if its not a component
        components.push_back(newComponent);
        return *newComponent;
    }

    // Hide in API
    void AddClonedComponent(Component* component) {
        //components.push_back(component->Clone()); // This is causing a compile errors
        //components.back()->exposedVariables = component->exposedVariables;
        //components.back()->gameObject = this;
    }

    template<typename T>
    bool RemoveComponent();

    bool RemoveComponent(Component* component);

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
        void SetPosition(Vector3 position)
        {
            for (GameObject* child : gameObject->childGameObjects)
                child->transform.SetPosition(child->transform.GetLocalPosition() + position);

            _position = position;
        }
        void SetPosition(Vector2 position) { SetPosition({position.x, position.y, _position.z}); }
        void SetPosition(float x, float y, float z) { SetPosition({ x, y, z }); }
        void SetPosition(float x, float y) { SetPosition({ x, y,  _position.z }); }
        Vector3 GetPosition() { return _position; }

        void SetLocalPosition(Vector3 position) { gameObject->parentGameObject == nullptr ? SetPosition(position) : SetPosition(gameObject->parentGameObject->transform._position + position); }
        void SetLocalPosition(Vector2 position) { SetLocalPosition({ position.x, position.y, _position.z }); }
        void SetLocalPosition(float x, float y, float z) { SetLocalPosition({ x, y, z }); }
        void SetLocalPosition(float x, float y) { SetLocalPosition({ x, y,  _position.z }); }
        Vector3 GetLocalPosition() { return gameObject->parentGameObject == nullptr ? _position : (_position - gameObject->parentGameObject->transform._position); }

        void SetRotation(Quaternion rotation) { _rotation = rotation; }
        Quaternion GetRotation() { return _rotation; }

        /**
        Set the game object's rotation in degrees
        */
        void SetRotationEuler(Vector3 rotation)
        {
            for (GameObject* child : gameObject->childGameObjects)
                child->transform.SetRotationEuler(child->transform.GetLocalRotationEuler() + rotation);

            _rotation = EulerToQuaternion((float)rotation.x * DEG2RAD, rotation.y * DEG2RAD, rotation.z * DEG2RAD);
        }
        void SetRotationEuler(Vector2 rotation) { SetRotationEuler({ rotation.x, rotation.y, QuaternionToEuler(_rotation).z * RAD2DEG}); }
        /**
        Set the game object's rotation in degrees
        */
        void SetRotationEuler(float x, float y, float z) { SetRotationEuler({ x, y, z }); }
        void SetRotationEuler(float x, float y) { SetRotationEuler({ x, y, QuaternionToEuler(_rotation).z * RAD2DEG }); }
        /**
        Get the game object's rotation in degrees
        @return Vector3 euler of the rotation
        */
        Vector3 GetRotationEuler() { return QuaternionToEuler(_rotation) * RAD2DEG; }

        /**
        Set the game object's local rotation in degrees
        */
        // { gameObject->parentGameObject == nullptr ? _scale = scale : _scale = (gameObject->parentGameObject->transform._scale - scale); }
        void SetLocalRotationEuler(Vector3 rotation) { gameObject->parentGameObject == nullptr ? SetRotationEuler({ rotation.x, rotation.y, rotation.z}) : SetRotationEuler({ gameObject->parentGameObject->transform.GetRotationEuler() + (rotation.x, rotation.y, rotation.z) }); }
        void SetLocalRotationEuler(Vector2 rotation) { SetLocalRotationEuler({ rotation.x, rotation.y, QuaternionToEuler(_rotation).z * RAD2DEG }); }
        /**
        Set the game object's local rotation in degrees
        */
        void SetLocalRotationEuler(float x, float y, float z) { SetLocalRotationEuler({ x, y, z }); }
        void SetLocalRotationEuler(float x, float y) { SetLocalRotationEuler({ x, y,  QuaternionToEuler(_rotation).z * RAD2DEG }); }
        /**
        Get the game object's local rotation in degrees
        @return Vector3 euler of the rotation
        */
        Vector3 GetLocalRotationEuler() { return gameObject->parentGameObject == nullptr ? QuaternionToEuler(_rotation) * RAD2DEG : ((QuaternionToEuler(_rotation) * RAD2DEG) - (QuaternionToEuler(gameObject->parentGameObject->transform._rotation) * RAD2DEG)); }

        void SetScale(Vector3 scale)
        { 
            for (GameObject* child : gameObject->childGameObjects)
                child->transform.SetScale(child->transform.GetLocalScale() * scale);

            _scale = scale;
        }
        void SetScale(Vector2 scale) { SetScale({ scale.x, scale.y, _scale.z }); }
        void SetScale(float x, float y, float z) { SetScale({ x, y, z }); }
        void SetScale(float x, float y) { SetScale({ x, y,  _scale.z }); }
        Vector3 GetScale() { return _scale; }

        void SetLocalScale(Vector3 scale) { gameObject->parentGameObject == nullptr ? SetScale(scale) : SetScale(gameObject->parentGameObject->transform._scale * scale); }
        void SetLocalScale(Vector2 scale) { SetLocalScale({ scale.x, scale.y, _scale.z }); }
        void SetLocalScale(float x, float y, float z) { SetLocalScale({ x, y, z }); }
        void SetLocalScale(float x, float y) { SetLocalScale({ x, y, _scale.z }); }
        Vector3 GetLocalScale() { return gameObject->parentGameObject == nullptr ? _scale : (_scale / gameObject->parentGameObject->transform._scale); }

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

        GameObject* gameObject;

    private:
        Vector3 _position = { 0,0,0 };
        Quaternion _rotation = Quaternion::Identity();
        Vector3 _scale = { 1,1,1 };
    };
    Transform transform;

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