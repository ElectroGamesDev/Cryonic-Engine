#include "SceneManager.h"
#include <fstream>
#include <iomanip>
#include "../ConsoleLogger.h"
#include <iostream>
#include <sstream>
//#include "../Editor.h"
#include <algorithm>
#include <cctype>
#include "../json.hpp"

#include "../Components/ScriptLoader.h"
#include "../Components/Component.h"
#include "../Components/MeshRenderer.h"
#include "../Components/ScriptComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/Lighting.h"
#include "../Components/SpriteRenderer.h"
#include "../Components/Collider2D.h"
#include "../Components/Rigidbody2D.h"

#if defined(EDITOR)
#include "../ProjectManager.h"
#endif
#include "../RaylibWrapper.h"

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
    for (GameObject* object : scene->GetGameObjects())
    {
        json gameObjectData;

        // Save texture
        //if (object->GetTexture() != nullptr) {
        //    gameObjectData["texture_path"] = object->GetPath();
        //}

        //gameObjectData["model_path"] = object->GetModelPath().string();
        gameObjectData["name"] = object->GetName();
        gameObjectData["position"] = { object->transform.GetPosition().x, object->transform.GetPosition().y, object->transform.GetPosition().z };
        //gameObjectData["real_size"] = { object->GetRealSize().x, object->GetRealSize().y, object->GetRealSize().z };
        gameObjectData["size"] = { object->transform.GetScale().x, object->transform.GetScale().y, object->transform.GetScale().z};
        gameObjectData["rotation"] = { object->transform.GetRotation().x, object->transform.GetRotation().y, object->transform.GetRotation().z, object->transform.GetRotation().w };
        gameObjectData["id"] = object->GetId();
        gameObjectData["active"] = object->IsActive();

        //gameObjectData["tint"] = { object->GetTint().Value.x, object->GetTint().Value.y, object->GetTint().Value.z, object->GetTint().Value.w };
        //gameObjectData["z_order"] = object->GetZOrder();

        if (object->GetParent() != nullptr)
            gameObjectData["parent_id"] = object->GetParent()->GetId();
        else
            gameObjectData["parent_id"] = -1;

        // Save components
        json componentsData;
        for (Component* component : object->GetComponents())
        {
            json componentData;
            componentData["name"] = component->name;
            componentData["active"] = component->IsActive();
            componentData["id"] = component->id;
            #if defined(EDITOR)
            componentData["exposed_variables"] = component->exposedVariables;
            #endif
            //componentData["runInEditor"] = component->runInEditor;

            // Temporary solution
            // Todo: maybe put these in Exposed Variables (but not visible in Properties), or something similar to ExposedVariables like SerializedVariables
            if (dynamic_cast<MeshRenderer*>(component))
            {
                componentData["model_path"] = dynamic_cast<MeshRenderer*>(component)->GetModelPath();
            }
            if (dynamic_cast<SpriteRenderer*>(component))
            {
                componentData["texture_path"] = dynamic_cast<SpriteRenderer*>(component)->GetTexturePath().string();
            }
            else if (dynamic_cast<ScriptComponent*>(component))
            {
                componentData["cpp_path"] = dynamic_cast<ScriptComponent*>(component)->GetCppPath();
                componentData["header_path"] = dynamic_cast<ScriptComponent*>(component)->GetHeaderPath();
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

bool SceneManager::LoadScene(std::filesystem::path filePath)
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

    std::unordered_map<int, int> parentObjects;

    // Create game objects
    for (const auto& gameObjectData : sceneData["game_objects"])
    {
        // Create new game object
        GameObject* gameObject = scene.AddGameObject(gameObjectData["id"]);
        std::string objectName = gameObjectData["name"];

        // Load name, position, real size, size, rotation, id, tint, zOrder
        //gameObject->SetModelPath(gameObjectData["model_path"]);
        //gameObject->SetModel(LoadModel(gameObject->GetModelPath().string().c_str()));
        gameObject->SetName(gameObjectData["name"]);
        gameObject->transform.SetPosition(Vector3{ gameObjectData["position"][0], gameObjectData["position"][1], gameObjectData["position"][2] });
        //gameObject->transform.SetRealSize(Vector3{gameObjectData["real_size"][0], gameObjectData["real_size"][1], gameObjectData["real_size"][2] });
        gameObject->transform.SetScale(Vector3{ gameObjectData["size"][0], gameObjectData["size"][1], gameObjectData["size"][2] });
        //gameObject->SetRotation(Quaternion{ gameObjectData["rotation"][0], gameObjectData["rotation"][1], gameObjectData["rotation"][2] });
        gameObject->transform.SetRotation(Quaternion{ gameObjectData["rotation"][0], gameObjectData["rotation"][1], gameObjectData["rotation"][2], gameObjectData["rotation"][3]});
        //gameObject->SetTint(ImVec4(gameObjectData["tint"][0], gameObjectData["tint"][1], gameObjectData["tint"][2], gameObjectData["tint"][3]));
        //gameObject->SetZOrder(gameObjectData["z_order"]);
        gameObject->SetActive(gameObjectData["active"]);

        parentObjects[gameObjectData["id"]] = gameObjectData["parent_id"];

        // Load components
        for (const auto& componentData : gameObjectData["components"])
        {
            // Temporary solution
            if (componentData["name"] == "MeshRenderer")
            {
                MeshRenderer& component = gameObject->AddComponent<MeshRenderer>();
                //component.gameObject = &gameObject;
                component.SetModelPath(componentData["model_path"]);
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
                #if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
                #endif

                if (component.GetModelPath().string() == "Cube")
                    component.SetModel(Cube, component.GetModelPath().string(), LitStandard);
                else if (component.GetModelPath().string() == "Plane")
                    component.SetModel(Plane, component.GetModelPath().string(), LitStandard);
                else if (component.GetModelPath().string() == "Sphere")
                    component.SetModel(Sphere, component.GetModelPath().string(), LitStandard);
                else if (component.GetModelPath().string() == "Cylinder")
                    component.SetModel(Cylinder, component.GetModelPath().string(), LitStandard);
                else if (component.GetModelPath().string() == "Cone")
                    component.SetModel(Cone, component.GetModelPath().string(), LitStandard);
                else
                    component.SetModel(Custom, component.GetModelPath().string(), LitStandard);
            }
            else if (componentData["name"] == "SpriteRenderer")
            {
                SpriteRenderer& component = gameObject->AddComponent<SpriteRenderer>();
                component.SetTexturePath(componentData["texture_path"]);
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
#if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
#endif
                if (component.GetTexturePath().string() != "Square" && component.GetTexturePath().string() != "Circle")
                {
                    std::filesystem::path path;
                    #if defined(EDITOR)
                    path = ProjectManager::projectData.path / "Assets";
                    #else
                    path = std::filesystem::path(RaylibWrapper::GetWorkingDirectory()) / "Resources" / "Assets";
                    #endif
                    RaylibWrapper::Texture2D texture = RaylibWrapper::LoadTexture((path / component.GetTexturePath()).string().c_str()); // Todo: Don't create a new texture if one is already created for the texture
                    component.SetTexture({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format });
                }
            }
            else if (componentData["name"] == "ScriptComponent")
            {
#if defined(EDITOR)
                ScriptComponent& component = gameObject->AddComponent<ScriptComponent>();
                //component.gameObject = &gameObject;
                component.SetCppPath(componentData["cpp_path"]);
                component.SetHeaderPath(componentData["header_path"]);
                component.SetName(component.GetHeaderPath().stem().string());
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
                component.exposedVariables = componentData["exposed_variables"];
                //component.name = component.GetName();
#else
                SetupScriptComponent(gameObject, componentData["id"], componentData["active"], std::filesystem::path(componentData["header_path"]).stem().string());
#endif
            }
            else if (componentData["name"] == "CameraComponent")
            {
                CameraComponent& component = gameObject->AddComponent<CameraComponent>();
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
                #if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
                #endif
            }
            else if (componentData["name"] == "Lighting")
            {
                Lighting& component = gameObject->AddComponent<Lighting>();
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
#if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
#endif
            }
            else if (componentData["name"] == "Collider2D")
            {
                Collider2D& component = gameObject->AddComponent<Collider2D>();
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
#if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
#endif
            }
            else if (componentData["name"] == "Rigidbody2D")
            {
                Rigidbody2D& component = gameObject->AddComponent<Rigidbody2D>();
                component.SetActive(componentData["active"]);
                component.id = componentData["id"];
#if defined(EDITOR)
                component.exposedVariables = componentData["exposed_variables"];
#endif
            }
            // Todo: make Component of type Component so then I can set the Component variables like SetActive, id, expoedVariables, etc only once and not in each if statement
        }


        // Add game object to scene
        //scene.AddGameObject();
    }

    for (GameObject* gameObject : scene.GetGameObjects())
    {
        // Set exposed variables values
#if !defined(EDITOR)
        for (Component* component : gameObject->GetComponents())
        {
            component->SetExposedVariables();
    }
        #endif

        // Set parents
        if (parentObjects[gameObject->GetId()] != 0)
            for (GameObject* go : scene.GetGameObjects())
                if (go->GetId() == parentObjects[gameObject->GetId()])
                {
                    gameObject->SetParent(go);
                    break;
                }
    }


    bool sceneFound = false;
    for (Scene& scenes : m_scenes)
        if (scenes.GetPath() == scene.GetPath())
        {
            SetActiveScene(&scenes);
            sceneFound = true;
        }

    if (!sceneFound)
    {
        AddScene(scene);
        SetActiveScene(&GetScenes()->back());

        for (GameObject* gameObject : GetActiveScene()->GetGameObjects())
            for (Component* component : gameObject->GetComponents())
                component->gameObject = gameObject;
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

#if defined(EDITOR)
    GameObject* cameraObject = m_scenes.back().AddGameObject();
    if (ProjectManager::projectData.is3D)
        cameraObject->transform.SetPosition({ 0,0, -25 });
    else
        cameraObject->transform.SetPosition({ 0,0, -50 });
    cameraObject->transform.SetScale({ 1,1,1 });
    cameraObject->transform.SetRotation(Quaternion::Identity());
    cameraObject->SetName("Camera");
    CameraComponent& camera = cameraObject->AddComponent<CameraComponent>();
    camera.gameObject = cameraObject;
#endif

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

