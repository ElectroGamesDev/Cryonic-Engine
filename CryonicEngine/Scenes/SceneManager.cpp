#include "SceneManager.h"
#include <fstream>
#include <iomanip>
#include "../ConsoleLogger.h"
#include <iostream>
#include <sstream>
//#include "../Editor.h"
#include "imgui.h"
#include <algorithm>
#include <cctype>
#include "json.hpp"
#include "../Components/Component.h"

#include "../Components/MeshRenderer.h"

using json = nlohmann::json;

std::vector<Scene> SceneManager::m_scenes;
Scene* SceneManager::m_activeScene;

SceneManager::SceneManager() {
    m_activeScene = nullptr;
}

Scene* SceneManager::GetActiveScene() {
    return m_activeScene;
}

void SceneManager::SetActiveScene(Scene* scene) {
    m_activeScene = scene;
}

std::vector<Scene>* SceneManager::GetScenes() {
    return &m_scenes; 
}

bool SceneManager::SaveScene(Scene* scene)
{
    json sceneData;

    // Save game objects
    for (GameObject& object : scene->GetGameObjects())
    {
        json gameObjectData;

        // Save texture
        //if (object.GetTexture() != nullptr) {
        //    gameObjectData["texture_path"] = object.GetPath();
        //}

        //gameObjectData["model_path"] = object.GetModelPath().string();
        gameObjectData["name"] = object.GetName();
        gameObjectData["position"] = { object.transform.GetPosition().x, object.transform.GetPosition().y, object.transform.GetPosition().z };
        //gameObjectData["real_size"] = { object.GetRealSize().x, object.GetRealSize().y, object.GetRealSize().z };
        gameObjectData["size"] = { object.transform.GetScale().x, object.transform.GetScale().y, object.transform.GetScale().z};
        gameObjectData["rotation"] = { object.transform.GetRotation().x, object.transform.GetRotation().y, object.transform.GetRotation().z, object.transform.GetRotation().w };
        gameObjectData["id"] = object.GetId();

        //gameObjectData["tint"] = { object.GetTint().Value.x, object.GetTint().Value.y, object.GetTint().Value.z, object.GetTint().Value.w };
        //gameObjectData["z_order"] = object.GetZOrder();

        // Save components
        json componentsData;
        for (Component* component : object.GetComponents())
        {
            json componentData;
            componentData["name"] = component->name;
            componentData["active"] = component->IsActive();
            componentData["runInEditor"] = component->runInEditor;

            // Temporary solution
            if (dynamic_cast<MeshRenderer*>(component))
            {
                componentData["model_path"] = dynamic_cast<MeshRenderer*>(component)->GetModelPath();
            }

            componentsData.push_back(componentData);
        }
        gameObjectData["components"] = componentsData;

        // Add game object data to scene data
        sceneData["game_objects"].push_back(gameObjectData);
    }

    //std::cout << sceneData.dump(4) << std::endl;
    // 
    // Write JSON data to file
    //ConsoleLogger::InfoLog("Saved scene at " + scene->GetPath().string());

    std::string formattedPath = scene->GetPath().string();
    std::size_t found = formattedPath.find('\\');
    while (found != std::string::npos) {
        formattedPath.replace(found, 1, "\\\\");
        found = formattedPath.find('\\', found + 2);
    }
    formattedPath.erase(std::remove_if(formattedPath.begin(), formattedPath.end(), [](char c) {
        return !std::isprint(static_cast<unsigned char>(c));
        }), formattedPath.end());

    std::ofstream file(formattedPath);
    file << std::setw(4) << sceneData << std::endl;
    file.close();

    if (file.fail() && file.bad())
    {
        if (file.eof())
        {
            ConsoleLogger::ErrorLog("End of file reached while saving the scene '" + scene->GetPath().stem().string() + "' at the path '" + scene->GetPath().string() + "'");
        }
        else if (file.fail())
        {
            ConsoleLogger::ErrorLog("The scene \"" + scene->GetPath().stem().string() + "\" at the path \"" + scene->GetPath().string() + "\" failed to open. The file can't be found or you have invalid permissions.");
        }
        else if (file.bad())
        {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The scene \"" + scene->GetPath().stem().string() + "\" failed to save. Error: " + std::string(errorMessage));
        }
        else if (file.is_open())
        {
            ConsoleLogger::ErrorLog("The scene \"" + scene->GetPath().string() + "\" failed to save because it is open in another program");
        }
        else
        {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The scene \"" + scene->GetPath().stem().string() + "\" failed to save. Error: " + std::string(errorMessage));
        }
        return false;
    }
    ConsoleLogger::InfoLog("The scene \"" + scene->GetPath().stem().string() + "\" has been saved");
    return true;
}

bool SceneManager::LoadScene(const std::filesystem::path& filePath)
{
    if (!std::filesystem::exists(filePath) || filePath.extension() != ".scene")
    {
        ConsoleLogger::WarningLog("The path for the scene \"" + filePath.stem().string() + "\" is invalid, \"" + filePath.string() + "\"");
        return false;
    }

    std::string filePathString = filePath.string();
    filePathString.erase(std::remove_if(filePathString.begin(), filePathString.end(),
        [](char c) { return !std::isprint(c); }), filePathString.end());

    // Load file into string
    std::ifstream file(filePathString);
    if (!file.is_open()) {
        ConsoleLogger::WarningLog("Can not open the scene \"" + filePath.stem().string() + "\" at the path \"" + filePath.string() + "\"");
        return false;
    }
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    //ConsoleLogger::InfoLog(fileStream.str());

    // Parse JSON data
    json sceneData = json::parse(fileStream.str());

    // Create new scene
    Scene scene = Scene(filePath, {});

    // Create game objects
    for (const auto& gameObjectData : sceneData["game_objects"])
    {
        // Create new game object
        GameObject gameObject = GameObject(gameObjectData["id"]);
        std::string objectName = gameObjectData["name"];

        // Load name, position, real size, size, rotation, id, tint, zOrder
        //gameObject.SetModelPath(gameObjectData["model_path"]);
        //gameObject.SetModel(LoadModel(gameObject.GetModelPath().string().c_str()));
        gameObject.SetName(gameObjectData["name"]);
        gameObject.transform.SetPosition(Vector3{ gameObjectData["position"][0], gameObjectData["position"][1], gameObjectData["position"][2] });
        //gameObject.transform.SetRealSize(Vector3{gameObjectData["real_size"][0], gameObjectData["real_size"][1], gameObjectData["real_size"][2] });
        gameObject.transform.SetScale(Vector3{ gameObjectData["size"][0], gameObjectData["size"][1], gameObjectData["size"][2] });
        //gameObject.SetRotation(Quaternion{ gameObjectData["rotation"][0], gameObjectData["rotation"][1], gameObjectData["rotation"][2] });
        gameObject.transform.SetRotation(Quaternion{ gameObjectData["rotation"][0], gameObjectData["rotation"][1], gameObjectData["rotation"][2], gameObjectData["rotation"][3]});
        //gameObject.SetTint(ImVec4(gameObjectData["tint"][0], gameObjectData["tint"][1], gameObjectData["tint"][2], gameObjectData["tint"][3]));
        //gameObject.SetZOrder(gameObjectData["z_order"]);

        // Load components
        for (const auto& componentData : gameObjectData["components"])
        {
            // Temporary solution
            if (componentData["name"] == "MeshRenderer")
            {
                MeshRenderer& meshRenderer = gameObject.AddComponent<MeshRenderer>();
                meshRenderer.gameObject = gameObject;
                meshRenderer.SetModelPath(componentData["model_path"]);
                meshRenderer.SetModel(LoadModel(meshRenderer.GetModelPath().string().c_str()));
            }
        }

        // Add game object to scene
        scene.AddGameObject(gameObject);
    }
    bool sceneFound = false;
    for (Scene& scenes : m_scenes) {
        if (scenes.GetPath() == scene.GetPath()) {
            SetActiveScene(&scenes);
            sceneFound = true;
        }
    }
    if (!sceneFound)
    {
        AddScene(scene);
        SetActiveScene(&GetScenes()->back());
    }
    ConsoleLogger::InfoLog("The scene \"" + filePath.stem().string() + "\" has been loaded");
    return true;
}

void SceneManager::AddScene(Scene scene) {
    m_scenes.push_back(scene);
}

Scene* SceneManager::CreateScene()
{
    Scene scene;
    AddScene(scene);
    return &m_scenes.back();
}

void SceneManager::ResetScene(Scene* scene)
{
    //scene->GetGameObjects().clear();
    //for (GameObject obj : scene->gameObjectsBackup)
    //{
    //    scene->AddGameObject(obj);
    //}
}

void SceneManager::BackupScene(Scene* scene) 
{
    //scene->gameObjectsBackup.clear();
    //for (GameObject obj : scene->GetGameObjects())
    //{
    //    scene->gameObjectsBackup.push_back(obj);
    //}
}

// Todo:: Add Backup Cleanup function

