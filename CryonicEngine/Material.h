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

        std::ifstream file;
#ifndef EDITOR
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
        albedoSprite = new Sprite(albedoTexturePath);
        normalSprite = new Sprite(normalTexturePath);
        metallicSprite = new Sprite(metallicTexturePath);
        roughnessSprite = new Sprite(roughnessTexturePath);
        emissionSprite = new Sprite(emissionTexturePath);

        materials[path] = this;

        this->path = path;
    }

    ~Material()
    {
        // Textures are unloaded in Game.cpp
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

    // Hide in API
    static std::unordered_map<std::filesystem::path, Material*> materials;

    static void LoadWhiteTexture();
    static void UnloadWhiteTexture();

    /**
     * @brief Gets the material from the path and loads it if needed
     */
    static Material* GetMaterial(std::string path);

    static RaylibWrapper::Texture2D whiteTexture;

private:
    std::string path = "";
    Color albedoColor = { 255, 255, 255, 255 };
    float metallic = 0.0f;
    float roughness = 1.0f;
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
};