#pragma once

#include "Scene.h"

class SceneManager {
public:
    SceneManager(); 
    static Scene* GetActiveScene();
    static void SetActiveScene(Scene* scene);
    static std::vector<Scene>* GetScenes();
    static bool SaveScene(Scene* scene);
    static bool LoadScene(std::filesystem::path filePath); // Todo: Should this return the scene?
    static void AddScene(Scene scene);
    static Scene* CreateScene();
    static void ResetScene(Scene* scene);
    static void BackupScene(Scene* scene);

private:
    static std::vector<Scene> m_scenes;
    static Scene* m_activeScene;
};
