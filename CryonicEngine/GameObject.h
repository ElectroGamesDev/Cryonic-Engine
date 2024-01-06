#pragma once

#include <string>
#include <vector>
#include "imgui.h"
#include "ConsoleLogger.h"
#include "raylib.h"
#include "raymath.h"
#include <filesystem>

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

    template <typename T>
    T& AddComponent() {
        T* newComponent = new T(this);
        static_cast<Component*>(newComponent)->gameObject = this; // Todo: This may cause a crash if its not a component
        components.push_back(newComponent);
        return *newComponent;
    }
    template <typename T>
    bool RemoveComponent();
    template<typename T>
    T& GetComponent();

    std::vector<Component*> GetComponents();
    //GameObject& operator=(const GameObject& other);
    bool operator==(const GameObject& other) const;
    bool operator!=(const GameObject& other) const;

    struct Transform
    {
        const float DEG = 180.0f / PI;
        const float RAD = PI / 180.0f;

        Vector3 _position = {0,0,0};
        Quaternion _rotation = QuaternionIdentity();
        Vector3 _scale = { 1,1,1 };
        Vector3 _realScale;


        void SetPosition(Vector3 position) { _position = position; }
        Vector3 GetPosition() { return _position; }

        void SetRotation(Quaternion rotation) { _rotation = rotation; }
        Quaternion GetRotation() { return _rotation; }

        /**
        Set the game object's rotation in degrees
        */
        void SetRotationEuler(Vector3 rotation) { _rotation = QuaternionFromEuler((float)rotation.x * RAD, rotation.y * RAD, rotation.z * RAD); }
        /**
        Get the game object's rotation in degrees
        @return Vector3 euler of the rotation
        */
        Vector3 GetRotationEuler() { return Vector3Scale(QuaternionToEuler(_rotation), DEG); }

        void SetScale(Vector3 scale) { _scale = scale; }
        Vector3 GetScale() { return _scale; }

        Transform& operator=(const Transform& other) {
            if (this != &other) {
                _position = other._position;
                _rotation = other._rotation;
                _scale = other._scale;
                _realScale = other._realScale;
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
                _scale.z == other._scale.z &&
                _realScale.x == other._realScale.x &&
                _realScale.y == other._realScale.y &&
                _realScale.z == other._realScale.z);
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
};