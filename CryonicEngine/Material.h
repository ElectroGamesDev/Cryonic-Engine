#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include "RaylibWrapper.h"
#include "ConsoleLogger.h"
#include "json.hpp"
#ifndef EDITOR
#include "Game.h"
#endif
#include "CryonicCore.h"
#include "Sprite.h"
#ifdef EDITOR
#include "FileWatcher.h"
#endif
#include "ShadowManager.h"

class Material
{
public:
    Material(std::string path)
    {
        for (char& c : path) // Reformat the path for unix
        {
            if (c == '\\')
                c = '/';
        }

        this->path = path;

        LoadData();

#ifdef EDITOR
        FileWatcher::AddFileMoveCallback(path, [this](const std::string& oldPath, const std::string& newPath) {
            this->OnFileMoved(oldPath, newPath);
            });

        FileWatcher::AddFileModifyCallback(path, [this]() {
            this->OnFileModified();
            });
#endif

        materials[path] = this;

        id = nextId;
        raylibMaterial.params[0] = static_cast<float>(id);
        nextId++;
    }

    ~Material()
    {
        ResetData();
    }

    void OnFileMoved(const std::string oldPath, const std::string newPath)
    {
        path = newPath;
        materials[newPath] = this;
        materials.erase(oldPath);

        // Todo: Update all components using this material (or the components should find the new path themself. Maybe in FileWatcher for 10-15 seconds I could have a recentlyMoved vector which they can search though)
    }

    void OnFileModified() // Todo: I shouldn't be resetting all the data just for changing something in the file. It causes unnecessary overhead.
    {
        ResetData();
        LoadData();
    }

    void ResetData() // Todo: Move this and some of the other functions to Material.cpp
    {
        // Todo: The Sprite is not being unloaded. Unload it if its not being used anywhere else. It could actually be better to let Sprite handle the unloading, just let the Sprite know that its no longer being used by this.

        albedoColor = { 255, 255, 255, 255 };
        metallic = 0.0f;
        roughness = 0.5f;
        emission = 0.0f;

        albedoTexturePath = "";
        normalTexturePath = "";
        metallicTexturePath = "";
        roughnessTexturePath = "";
        emissionTexturePath = "";

        if (albedoSprite)
            delete albedoSprite;

        if (normalSprite)
            delete normalSprite;

        if (metallicSprite)
            delete metallicSprite;

        if (roughnessSprite)
            delete roughnessSprite;

        if (emissionSprite)
            delete emissionSprite;

        albedoSprite = nullptr;
        normalSprite = nullptr;
        metallicSprite = nullptr;
        roughnessSprite = nullptr;
        emissionSprite = nullptr;

        RaylibWrapper::UnloadMaterial(raylibMaterial); // Most likely the same material is going to be loaded again, but in case it isn't (different material or the material fails to load), I'll unload it. This shouldn't really add any overhead.

        delete[] raylibMaterial.maps;
        raylibMaterial = {};
        raylibMaterial.params[0] = static_cast<float>(id);
    }

    void LoadData()
    {
        std::ifstream file;
#ifdef EDITOR
        file.open(ProjectManager::projectData.path.string() + "/Assets/" + path);
#else
        if (exeParent.empty())
            file.open("Resources/Assets/" + path);
        else
            file.open(std::filesystem::path(exeParent) / "Resources" / "Assets" / path);
#endif
        if (!file.is_open())
        {
            ConsoleLogger::ErrorLog("Material failed to load. Path: " + path);
            return;
        }

        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        // Parse material properties from JSON
        try {
            albedoColor.r = jsonData["public"]["albedoColor"][0].get<float>();
            albedoColor.g = jsonData["public"]["albedoColor"][1].get<float>();
            albedoColor.b = jsonData["public"]["albedoColor"][2].get<float>();
            albedoColor.a = jsonData["public"]["albedoColor"][3].get<float>();

            metallic = jsonData["public"]["metallic"].get<float>();
            roughness = jsonData["public"]["roughness"].get<float>();
            emission = jsonData["public"]["emission"].get<float>();
            albedoTexturePath = jsonData["public"]["albedoTexture"].get<std::string>();
            normalTexturePath = jsonData["public"]["normalTexture"].get<std::string>();
            metallicTexturePath = jsonData["public"]["metallicTexture"].get<std::string>();
            roughnessTexturePath = jsonData["public"]["roughnessTexture"].get<std::string>();
            emissionTexturePath = jsonData["public"]["emissionTexture"].get<std::string>();
        }
        catch (const std::exception& e) {
            ConsoleLogger::ErrorLog("Failed to parse material data: " + std::string(e.what()));
            return;
        }

        // Loads textures/sprites // Todo: If a Sprite already exists for a texture, then dont create a new one
        auto setSprite = [](Sprite*& var, std::string& p) -> void {
            if (p == "")
            {
                var = nullptr;
                return;
            }

            for (char& c : p) // Reformat the path for unix.
            {
                if (c == '\\')
                    c = '/';
            }

            std::string absolutePath = p;
#if defined(EDITOR)
            absolutePath = ProjectManager::projectData.path.string() + "/Assets/" + p;
#else
            if (exeParent.empty())
                absolutePath = "Resources/Assets/" + p;
            else
                absolutePath = exeParent.string() + "/Resources/Assets/" + p;
#endif
            if (std::filesystem::exists(absolutePath))
            {
                var = new Sprite(p);
                ConsoleLogger::ErrorLog("Found");
            }
            else
            {
                ConsoleLogger::ErrorLog("not found");
                var = nullptr;
            }
        };

        setSprite(albedoSprite, albedoTexturePath);
        setSprite(normalSprite, normalTexturePath);
        setSprite(metallicSprite, metallicTexturePath);
        setSprite(roughnessSprite, roughnessTexturePath);
        setSprite(emissionSprite, emissionTexturePath);

        //albedoSprite = new Sprite(albedoTexturePath);
        //normalSprite = new Sprite(normalTexturePath);
        //metallicSprite = new Sprite(metallicTexturePath);
        //roughnessSprite = new Sprite(roughnessTexturePath);
        //emissionSprite = new Sprite(emissionTexturePath);

        raylibMaterial.maps = new RaylibWrapper::MaterialMap[RaylibWrapper::MAX_MATERIAL_MAPS];

        // Todo: If sprites ot default texture isn't loaded yet then they won't be set in the material
        auto setMaterialMap = [this](int mapIndex, Sprite* sprite, const Color& color, float value)
        {
            RaylibWrapper::Texture2D* texture;

            if (sprite)
                texture = sprite->GetTexture();
            else
                texture = &whiteTexture;

            if (!texture)
                return;

            raylibMaterial.maps[mapIndex] = { *texture , { color.r, color.g, color.b, color.a }, value };
        };

        setMaterialMap(RaylibWrapper::MATERIAL_MAP_ALBEDO, albedoSprite, albedoColor, 1);
        setMaterialMap(RaylibWrapper::MATERIAL_MAP_NORMAL, normalSprite, normalSprite ? Color{ 255, 255, 255 } : Color{ 128, 128, 255 }, 1);
        setMaterialMap(RaylibWrapper::MATERIAL_MAP_ROUGHNESS, roughnessSprite, { 255, 255, 255, 255 }, roughness);
        setMaterialMap(RaylibWrapper::MATERIAL_MAP_METALNESS, metallicSprite, { 255, 255, 255, 255 }, metallic);
        setMaterialMap(RaylibWrapper::MATERIAL_MAP_EMISSION, emissionSprite, { 255, 255, 255, 255 }, emission);

        raylibMaterial.shader.id = ShadowManager::shader.id;
        raylibMaterial.shader.locs = ShadowManager::shader.locs;
    }

    /**
     * @brief Returns the relative path to the material file.
     *
     * @return [std::string] The relative path to the material file.
     */
    std::string GetPath() const { return path; }

    /**
     * @brief Gets the albedo color of the material.
     */
    Color GetAlbedoColor() const { return albedoColor; }

    /**
     * @brief Gets the metallic value of the material.
     */
    float GetMetallic() const { return metallic; }

    /**
     * @brief Gets the roughness value of the material.
     */
    float GetRoughness() const { return roughness; }

    /**
     * @brief Gets the emission intensity of the material.
     */
    float GetEmission() const { return emission; }

    // Hide in API
    RaylibWrapper::Material* GetRaylibMaterial();

    // Textures
    Sprite* GetAlbedoSprite() { return albedoSprite; }
    Sprite* GetNormalSprite() { return normalSprite; }
    Sprite* GetMetallicSprite() { return metallicSprite; }
    Sprite* GetRoughnessSprite() { return roughnessSprite; }
    Sprite* GetEmissionSprite() { return emissionSprite; }

    int GetID() { return id; }

    // Hide in API
    static std::unordered_map<std::filesystem::path, Material*> materials;

    static void LoadDefaultMaterial();
    static void UnloadDefaultMaterial();

    static void LoadWhiteTexture();
    static void UnloadWhiteTexture();

#if defined (EDITOR)
    static void LoadPreviewMaterial();
    static void UnloadPreviewMaterial();
#endif

    /**
     * @brief Gets the material from the path and loads it if needed
     */
    static Material* GetMaterial(std::string path);

    static RaylibWrapper::Material defaultMaterial;
    static RaylibWrapper::Texture2D whiteTexture;
#if defined (EDITOR)
    static RaylibWrapper::Material previewMaterial;
#endif
    static int nextId;

private:
    std::string path = "";
    int id;
    Color albedoColor = { 255, 255, 255, 255 };
    float metallic = 0.0f;
    float roughness = 0.5f;
    float emission = 0.0f;

    std::string albedoTexturePath = "";
    std::string normalTexturePath = "";
    std::string metallicTexturePath = "";
    std::string roughnessTexturePath = "";
    std::string emissionTexturePath = "";

    Sprite* albedoSprite = nullptr;
    Sprite* normalSprite = nullptr;
    Sprite* metallicSprite = nullptr;
    Sprite* roughnessSprite = nullptr;
    Sprite* emissionSprite = nullptr;

    RaylibWrapper::Material raylibMaterial;
};