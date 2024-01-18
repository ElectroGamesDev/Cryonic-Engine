#pragma once

#include "../GameObject.h"
#include <vector>
#include <deque>
#include <filesystem>
#include <algorithm>

class Scene {
public:
    Scene(const std::filesystem::path& path = {}, std::deque<GameObject*> gameObjects = {});
    ~Scene();

    std::filesystem::path GetPath();
    void SetPath(std::filesystem::path path);
    GameObject* AddGameObject(int id = 0);
    void RemoveGameObject(GameObject* gameObject);
    std::deque<GameObject*>& GetGameObjects();
    GameObject* GetGameObject(const std::string& name); 

private:
    std::filesystem::path m_Path;
    std::deque<GameObject*> m_GameObjects;
};
