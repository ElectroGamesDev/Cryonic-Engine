#pragma once

#include "../GameObject.h"
#include <vector>
#include <filesystem>
#include <algorithm>

class Scene {
public:
    Scene(const std::filesystem::path& path = {}, std::vector<GameObject> gameObjects = {});
    ~Scene();

    std::filesystem::path GetPath();
    void SetPath(std::filesystem::path path);
    void AddGameObject(GameObject& gameObject);
    void RemoveGameObject(GameObject* gameObject);
    std::vector<GameObject>& GetGameObjects();
    GameObject* GetGameObject(const std::string& name); // Using reference since it doesn't need to copy the string

private:
    std::filesystem::path m_Path;
    std::vector<GameObject> m_GameObjects;
};
