#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <variant>
#include <unordered_map>
#include "Editor.h"
#include "FontManager.h"
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include <imgui_internal.h>
#include "IconsFontAwesome6.h"
#include <ImGuiNotify.hpp>
#include <fstream>
#include "Utilities.h"
#include <any>
#include "Components/MeshRenderer.h"
#include "Components/ScriptComponent.h"
#include "Components/CameraComponent.h"
#include "Components/Lighting.h"
#include "Components/SpriteRenderer.h"
#include "Components/Collider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/AnimationPlayer.h"
//#include "Components/AudioPlayer.h"
#include "IconManager.h"
#include "ShaderManager.h"
#include "ProjectManager.h"
#include "RaylibModelWrapper.h"
#include "RaylibDrawWrapper.h"
#include "imnodes.h"
#include <random>
#include <cmath>
#include "EventSystem.h"

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Jolt/Jolt.h"
JPH_SUPPRESS_WARNINGS

RaylibWrapper::Camera Editor::camera = { 0 };

// Todo: Switch these to use DEG2RAD and RAD2DEG from CryonicCore
const float DEG = 180.0f / 3.14159265358979323846f;
const float RAD = 3.14159265358979323846f / 180.0f;

ImGuiID dockspaceID;

bool closeEditor = false;
bool viewportOpen = true;
bool viewportFocused = false;
bool viewportHovered = false;
bool rmbDown = false;
RaylibWrapper::RenderTexture ViewTexture;
RaylibWrapper::RenderTexture cameraRenderTexture;
RaylibWrapper::Texture2D GridTexture = { 0 };

bool viewportOpened = true;
Vector4 viewportPosition;

std::variant<std::monostate, GameObject*, DataFile> Editor::objectInProperties = std::monostate{}; // Make a struct or something that holds a Path and ifstream String. Not specific to material so prefabs and stuff can use
GameObject* selectedObject = nullptr;
bool movingObjectX = false;
bool movingObjectY = false;
bool movingObjectZ = false;
bool cameraSelected = false;

RaylibWrapper::Vector2 lastMousePosition = { 0 };
bool animationGraphHovered = false;

bool explorerContextMenuOpen = false;
bool hierarchyContextMenuOpen = false;
GameObject* objectInHierarchyContextMenu = nullptr;
bool hierarchyObjectClicked = false;
bool componentsWindowOpen = false;
bool scriptCreateWinOpen = false;
bool animationGraphWinOpen = false;
bool projectSettingsWinOpen = false;

bool resetComponentsWin = true;
bool resetPropertiesWin = true;
bool resetViewportWin = true;
bool resetFileExplorerWin = true;
bool resetHierarchy = true;
bool resetCameraView = true;
bool resetProjectSettings = true;

nlohmann::json animationGraphData = nullptr;

std::filesystem::path fileExplorerPath;
enum DragTypes {None, ImageFile, ModelFile, Folder, Other};
std::pair<DragTypes, std::unordered_map<std::string, std::any>> dragData;

//std::unordered_map<std::string, Texture2D*> imageTextures;
std::vector<RaylibWrapper::Texture2D*> tempTextures;
std::vector<RaylibWrapper::RenderTexture2D*> tempRenderTextures;

//Quaternion Orientation = Quaternion::Identity();
float cameraSpeed = 1;
float oneSecondDelay = 1;

enum Tool
{
    Move,
    Rotate,
    Scale
}; Tool toolSelected = Move;

RaylibWrapper::RenderTexture2D* Editor::CreateModelPreview(std::filesystem::path modelPath, int textureSize)
{
    // Todo: Don't load and unload the models every frame if the file path is the same
    
    // Load the 3D model

    RaylibModel model;
    model.Create(Custom, modelPath.string().c_str(), LitStandard, ProjectManager::projectData.path);

    // Set a basic camera to view the model
    RaylibWrapper::Camera modelCamera = { 0 };
    modelCamera.position = { 0.0f, 0.0f, 6.0f };
    modelCamera.target = { 0.0f, 0.0f, 0.0f };
    modelCamera.up = { 0.0f, 1.0f, 0.0f };
    modelCamera.fovy = 45.0f;
    modelCamera.projection = RaylibWrapper::CAMERA_PERSPECTIVE;

    // Create a render texture
    //RenderTexture2D target = LoadRenderTexture(textureSize, textureSize);
    tempRenderTextures.push_back(new RaylibWrapper::RenderTexture2D(RaylibWrapper::LoadRenderTexture(textureSize, textureSize)));

    // Render model to texture
    BeginTextureMode(*tempRenderTextures.back());

    // Clear the render texture
    RaylibWrapper::ClearBackground(RaylibWrapper::Color{ 63, 63, 63, 255 });

    // Set camera position and projection for rendering
    RaylibWrapper::BeginMode3D(modelCamera);

    // Draw the model
    model.DrawModelWrapper(0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 255, 255, 255, 255);

    RaylibWrapper::EndMode3D();

    RaylibWrapper::EndTextureMode();

    //std::cout << "Path: " << modelPath << ", Texture Size: " << textureSize << std::endl;

    // Unload the model
    model.Unload();

    //tempTextures.push_back(new Texture2D(target.texture));

    //UnloadRenderTexture(target);

    // Return the generated texture
    return tempRenderTextures.back();
}

void Editor::RenderViewport()
{
    if (!viewportOpen) return;
    //if (resetPropertiesWin)
    //{
    //    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
    //    //ImGui::SetNextWindowSize(ImVec2(1280, 720));
    //    ImGui::SetNextWindowSize(ImVec2(1366, 736));
    //    ImGui::SetNextWindowPos(ImVec2(277, 52));
    //}
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // Todo: Use resize events

    if (ImGui::Begin((ICON_FA_CUBES + std::string(" Viewport")).c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
    {
        viewportPosition = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y };
        viewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
        rlImGuiImageRenderTextureFit(&ViewTexture, true);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::SetCursorPos(ImVec2(5, 27));
        std::string version = "";
        if (toolSelected == Move)
            version ="Selected";
        if (RaylibWrapper::rlImGuiImageButtonSize("##MoveTool", IconManager::imageTextures["MoveTool" + version + "Icon"], { 20, 20 }))
            toolSelected = Move;
        ImGui::SetCursorPos(ImVec2(27, 27));
        version = "";
        if (toolSelected == Rotate)
            version = "Selected";
        if (RaylibWrapper::rlImGuiImageButtonSize("##RotateTool", IconManager::imageTextures["RotateTool" + version + "Icon"], { 20, 20 }))
            toolSelected = Rotate;
        ImGui::SetCursorPos(ImVec2(49, 27));
        version = "";
        if (toolSelected == Scale)
            version = "Selected";
        if (RaylibWrapper::rlImGuiImageButtonSize("##ScaleTool", IconManager::imageTextures["ScaleTool" + version + "Icon"], { 20, 20 }))
            toolSelected = Scale;
        ImGui::PopStyleColor(3);

        viewportHovered = ImGui::IsWindowHovered();

        if (ImGui::IsWindowHovered() && (RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_RIGHT) || !ProjectManager::projectData.is3D && RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_MIDDLE))) // Todo: Maybe change to viewportFocused instead of IsWindowFocused
        {
            //    // Todo: Add SHIFT to speed up by x2, and scroll weel to change speed
            //rmbDown = true;
            //HideCursor();
            //DisableCursor();

            if (ProjectManager::projectData.is3D)
                RaylibWrapper::UpdateCamera(&camera, RaylibWrapper::CAMERA_PERSPECTIVE);

            else
            {
                RaylibWrapper::Vector2 deltaMouse = RaylibWrapper::Vector2Subtract(RaylibWrapper::GetMousePosition(), lastMousePosition);
                camera.position.x += deltaMouse.x * 0.1f;
                camera.position.y -= deltaMouse.y * 0.1f;
                camera.target.x = camera.position.x;
                camera.target.y = camera.position.y;
            }
            //    UpdateCamera(&camera, CAMERA_FREE);
            //    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 forward = { -viewMatrix.m12, -viewMatrix.m13, -viewMatrix.m14 };
            //        camera.position.x += forward.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += forward.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += forward.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 left = { -viewMatrix.m0, -viewMatrix.m1, -viewMatrix.m2 };
            //        camera.position.x += left.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += left.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += left.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 backward = { viewMatrix.m12, viewMatrix.m13, viewMatrix.m14 };
            //        camera.position.x += backward.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += backward.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += backward.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
            //    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
            //    {
            //        Matrix viewMatrix = GetCameraMatrix(camera);
            //        Vector3 right = { viewMatrix.m0, viewMatrix.m1, viewMatrix.m2 };
            //        camera.position.x += right.x * cameraSpeed * GetFrameTime();
            //        camera.position.y += right.y * cameraSpeed * GetFrameTime();
            //        camera.position.z += right.z * cameraSpeed * GetFrameTime();
            //        viewMatrix = GetCameraMatrix(camera);
            //        //UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            //    }
        }
        else if (rmbDown && RaylibWrapper::IsMouseButtonUp(RaylibWrapper::MOUSE_BUTTON_RIGHT))
        {
            //ShowCursor();
            //EnableCursor();
        }

        // Move Tool Arrows
        if (selectedObject != nullptr)
        {
            Vector3 position = selectedObject->transform.GetPosition();
            RaylibWrapper::Vector2 pos = RaylibWrapper::GetWorldToScreen({ position.x, position.y, position.z}, camera);
            // Divding positions by Raylib window size then multiply it by Viewport window size.
            pos.x = pos.x / RaylibWrapper::GetScreenWidth() * ImGui::GetWindowSize().x;
            pos.y = pos.y / RaylibWrapper::GetScreenHeight() * ImGui::GetWindowSize().y;
            ImGui::SetCursorPos(ImVec2(pos.x - 10, pos.y - 35));
            RaylibWrapper::rlImGuiImageSizeV(IconManager::imageTextures["GreenArrow"], { 20, 40 });
            if (!movingObjectY && ImGui::IsItemHovered() && RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_LEFT))
                movingObjectY = true;
            ImGui::SetCursorPos(ImVec2(pos.x + 5, pos.y));
            RaylibWrapper::rlImGuiImageSizeV(IconManager::imageTextures["RedArrow"], { 40, 20 });
            if (!movingObjectX && ImGui::IsItemHovered() && RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_LEFT))
                movingObjectX = true;
            ImGui::SetCursorPos(ImVec2(pos.x + 3, pos.y - 8));
            RaylibWrapper::rlImGuiImageSizeV(IconManager::imageTextures["XYMoveTool"], { 15, 15 });
            if (!movingObjectX && !movingObjectY && ImGui::IsItemHovered() && RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_LEFT)) // Todo: Fix this so if the user is moving with the X or Y arrow and move mouse up to XY move square, it will not switch to the XY square move tool
            {
                movingObjectX = true;
                movingObjectY = true;
            }
            if (RaylibWrapper::IsMouseButtonReleased(RaylibWrapper::MOUSE_BUTTON_LEFT))
            {
                movingObjectY = false;
                movingObjectX = false;
            }

            if (movingObjectX || movingObjectY || movingObjectZ)
            {
                RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
                RaylibWrapper::Vector2 deltaMouse = RaylibWrapper::Vector2Subtract(mousePosition, lastMousePosition);
                float x = selectedObject->transform.GetPosition().x;
                float y = selectedObject->transform.GetPosition().y;
                float z = selectedObject->transform.GetPosition().z;
                if (movingObjectX) // Todo: Dividing by 15 is a horrible solution. Instead I should set it to the mouse position - an offset
                    x += deltaMouse.x / 15;
                if (movingObjectY)
                    y -= deltaMouse.y / 15;
                if (movingObjectZ)
                    z -= deltaMouse.x / 15;
                selectedObject->transform.SetPosition({ x, y, z });
            }
        }
        lastMousePosition = RaylibWrapper::GetMousePosition();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::UpdateViewport()
{
    if (!viewportOpen) return;
    if (RaylibWrapper::IsWindowResized())
    {
        UnloadRenderTexture(ViewTexture);
        ViewTexture = RaylibWrapper::LoadRenderTexture(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight());
    }

    // Load new models/textures on drag&drop
    //if (viewportHovered && RaylibWrapper::IsFileDropped())
    //{
    //    FilePathList droppedFiles = LoadDroppedFiles();

    //    if (droppedFiles.count == 1) // Todo: Add support for multiple files dropped.
    //    {
    //        if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
    //            IsFileExtension(droppedFiles.paths[0], ".gltf") ||
    //            IsFileExtension(droppedFiles.paths[0], ".glb") ||
    //            IsFileExtension(droppedFiles.paths[0], ".vox") ||
    //            IsFileExtension(droppedFiles.paths[0], ".iqm") ||
    //            IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
    //        {
    //            std::filesystem::path folderPath = ProjectManager::projectData.path / "Assets" / "Models";
    //            if (!std::filesystem::exists(folderPath))
    //                std::filesystem::create_directories(folderPath);

    //            fileExplorerPath = folderPath;
    //            std::filesystem::path filePath = droppedFiles.paths[0];

    //            std::string filename = filePath.filename().string();
    //            std::string extension = filePath.extension().string();

    //            int counter = 1;
    //            while (std::filesystem::exists(folderPath / filename)) {
    //                // File with the same name exists, append a number to the filename
    //                filename = filePath.stem().string() + std::to_string(counter) + extension;
    //                counter++;
    //            }

    //            std::filesystem::copy_file(droppedFiles.paths[0], folderPath / filename);

    //            GameObject* gameObject = SceneManager::GetActiveScene()->AddGameObject();
    //            Material material;
    //            MeshRenderer& meshRenderer = gameObject->AddComponent<MeshRenderer>();
    //            meshRenderer.SetModelPath(folderPath / filename);
    //            meshRenderer.SetModel(LoadModel(meshRenderer.GetModelPath().string().c_str()));
    //            gameObject->transform.SetPosition({ 0,0,0 });
    //            //gameObject->SetRealSize(gameObject->GetModel().s);
    //            gameObject->transform.SetScale({ 1,1,1 });
    //            gameObject->transform.SetRotation(QuaternionIdentity());
    //            gameObject->SetName((folderPath / filename).stem().string());
    //            //material.SetDiffuseMap();
    //            // gameObject->GetModel().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture
    //            //gameObject->SetMaterial(.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

    //            //SceneManager::GetActiveScene()->AddGameObject(gameObject);

    //            for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back()->GetComponents())
    //            {
    //                component->gameObject = SceneManager::GetActiveScene()->GetGameObjects().back();
    //            }

    //            selectedObject = SceneManager::GetActiveScene()->GetGameObjects().back();
    //            objectInProperties = SceneManager::GetActiveScene()->GetGameObjects().back();

    //            // TODO: Move camera position from target enough distance to visualize model properly
    //        }
    //        else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
    //        {
    //            // Unload current model texture and load new one
    //            //UnloadTexture(texture);
    //            //texture = LoadTexture(droppedFiles.paths[0]);
    //            //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    //        }
    //    }

    //    UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
    //}

    //float period = 10;
    //float magnitude = 25;

    //camera.position.x = (float)(sinf((float)GetTime() / period) * magnitude);

    RaylibWrapper::BeginTextureMode(ViewTexture);

    if (ProjectManager::projectData.is3D)
    {
        RaylibWrapper::ClearBackground({ 135, 206, 235, 255 });
    }
    else
        RaylibWrapper::ClearBackground({128, 128, 128, 255});
    RaylibWrapper::BeginMode3D(camera);

    if (ProjectManager::projectData.is3D)
        RaylibWrapper::DrawGrid(100, 10.0f);

    for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
    {
        if (!gameObject->IsActive()) continue;
        for (Component* component : gameObject->GetComponents())
        {
            if (!component->IsActive()) continue;
            if (component->runInEditor)
                component->Update(RaylibWrapper::GetFrameTime());
            component->EditorUpdate();
        }
    }

    switch (dragData.first)
    {
    case ImageFile:
        ImVec2 mousePos = ImGui::GetMousePos();
        viewportHovered = (mousePos.x >= viewportPosition.x && mousePos.x <= viewportPosition.z &&
            mousePos.y >= viewportPosition.y && mousePos.y <= viewportPosition.w);
        if (viewportHovered)
        {
            RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
            mousePosition.x = (mousePosition.x - viewportPosition.x) / (viewportPosition.z - viewportPosition.x) * RaylibWrapper::GetScreenWidth();
            mousePosition.y = (mousePosition.y - viewportPosition.y) / (viewportPosition.w - viewportPosition.y) * RaylibWrapper::GetScreenHeight();

            RaylibWrapper::Vector3 position = RaylibWrapper::GetMouseRay(mousePosition, camera).position;
            RaylibWrapper::Texture2D texture = std::any_cast<RaylibWrapper::Texture2D>(dragData.second["Texture"]);
            float centerWidth = texture.width / 2.0f;
            float centerHeight = texture.height / 2.0f;
            //DrawRectangleWrapper(position.x, position.y, 20, 20, 0, 255, 255, 255, 255);
            //RaylibWrapper::DrawTexturePro(texture,
            //    { 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) },
            //    { position.x - centerWidth, position.y - centerHeight },
            //    {centerWidth, centerHeight},
            //    0,
            //    {255, 255, 255, 255});

            RaylibWrapper::DrawTextureProFlipped(texture,
                { 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) * -1 },
                { position.x, position.y, static_cast<float>(texture.width) / 10, static_cast<float>(texture.height) / 10},
                { static_cast<float>(texture.width) / 10 / 2, static_cast<float>(texture.height) / 10 / 2 },
                0,
                { 255, 255, 255, 255 });
        }
        break;
    }

    RaylibWrapper::EndMode3D();
    RaylibWrapper::EndTextureMode();
}

void Editor::RenderFileExplorer() // Todo: Handle if path is in a now deleted folder.
{
    if (resetFileExplorerWin)
    {
        resetFileExplorerWin = false;
    //    ImGui::SetNextWindowSize(ImVec2(1920, 282));
    //    ImGui::SetNextWindowPos(ImVec2(0, 788));
        fileExplorerPath = ProjectManager::projectData.path / "Assets"; // Todo: Make sure Assets path exists, if not then create it.
    }

    static std::filesystem::path renamingFile = "";
    static char newFileName[256] = "";
    static bool fileRenameFirstFrame = true;

    // Checks if the directory changed, or if the file is deleted/moved
    if (!renamingFile.empty() && (fileExplorerPath != renamingFile.parent_path() || !std::filesystem::exists(renamingFile)))
        renamingFile = "";

    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse;

    float nextX = 310;
    float nextY = 55;

    if (ImGui::Begin((ICON_FA_FOLDER_OPEN + std::string(" Content Browser")).c_str(), nullptr, windowFlags))
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));

        ImGui::SetCursorPos(ImVec2(305, 22));
        if (RaylibWrapper::rlImGuiImageButtonSize("##FileAddButton", IconManager::imageTextures["AddIcon"], ImVec2(16, 16)))
            explorerContextMenuOpen = true; // Todo: Not working

        //ImGui::SetCursorPos(ImVec2(325, 23));
        //if (rlImGuiImageButtonSize("##FileHomeButton", IconManager::imageTextures["HomeIcon"], ImVec2(17, 17)))
        //    fileExplorerPath = ProjectManager::projectData.path / "Assets";

        std::filesystem::path fileExplorerTempPath = fileExplorerPath;
        std::filesystem::path path = ProjectManager::projectData.path;
        ImGui::SetCursorPos(ImVec2(335, 22));
        bool relative = false;
        for (const auto& dir : fileExplorerTempPath)
        {
            if (!relative && dir == "Assets")
                relative = true;
            if (relative)
            {
                if (dir != "Assets")
                {
                    ImGui::Text(ICON_FA_ARROW_RIGHT);
                    ImGui::SameLine();
                }
                path /= dir;
                if (ImGui::Button((dir.string() + "##" + std::to_string(ImGui::GetCursorPosX())).c_str()))
                    fileExplorerPath = path;
                ImGui::SameLine();
                //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20); // Todo: Shouldn't use fixed value here. Should calculate the length of the button
            }
        }

        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 260, 25));
        ImGui::SetNextItemWidth(250);
        static char searchBuffer[256] = "";
        ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 10, false));
        if (ImGui::InputText("##FileSearch", searchBuffer, sizeof(searchBuffer), 0))
        {
            // Todo: Add Search
        }
        ImGui::PopFont();
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 262, 25));
        RaylibWrapper::rlImGuiImageSize(IconManager::imageTextures["SearchIcon"], 17, 17);

        ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetWindowPos().x + 300, ImGui::GetWindowPos().y + 45), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + 45), IM_COL32(0, 0, 0, 255), 1);

        // Back Button if not in /Assets folder
        if (fileExplorerPath != ProjectManager::projectData.path / "Assets")
        {
            ImGui::PushID(fileExplorerPath.string().c_str()); // set unique ID based on the path string
            // Creates back button
            ImGui::SetCursorPosY(nextY);
            ImGui::SetCursorPosX(nextX);
            if (RaylibWrapper::rlImGuiImageButtonSize("##FileBackButton", IconManager::imageTextures["FolderIcon"], ImVec2(32, 32)))
            {
                fileExplorerPath = fileExplorerPath.parent_path();
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::End();
                return;
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY());
            ImGui::SetCursorPosX(nextX + 5);
            ImGui::Text("Back");
            nextX += 60;
            ImGui::PopID();
        }

        std::filesystem::path folderHovering; // For file drag and drop
        if (!std::filesystem::exists(fileExplorerPath))
        {
            if (!std::filesystem::exists(ProjectManager::projectData.path / "Assets"))
                std::filesystem::create_directories(ProjectManager::projectData.path / "Assets");
            fileExplorerPath = ProjectManager::projectData.path / "Assets";
        }
        for (const auto& entry : std::filesystem::directory_iterator(fileExplorerPath))
        {
            ImGui::PushID(entry.path().string().c_str());
            std::string id = entry.path().string().c_str();
            std::string fileName = entry.path().stem().string();
            if (fileName.length() >= 6)
            {
                fileName = fileName.substr(0, 6);
                fileName = fileName + "..";
            }
            ImGui::SetCursorPosY(nextY);
            ImGui::SetCursorPosX(nextX);
            ImVec2 pos = ImGui::GetCursorPos();
            if (entry.is_directory())
            {
                // Need to create custom hover check code since ImGui::IsItemHovered() won't trigger as when dragging a file the ImGui Mouse Position doesn't update
                //static bool hoveringButton = false;
                //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(), { ImGui::GetCursorScreenPos().x + 40, ImGui::GetCursorScreenPos().y + 38 }, IM_COL32(255,0,0,255));
                bool hovered = (dragData.first != None && ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(), { ImGui::GetCursorScreenPos().x + 40, ImGui::GetCursorScreenPos().y + 38 })); // Using this since ImGui::IsItemHovered() won't work. ImGui::IsWindowHovered() also won't work for this. It seems these break when attempting to drag a button
                if (hovered)
                {
                    folderHovering = entry;
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.38f, 0.38f, 0.38f, 1.00f));
                }
                //ImVec2 mousePos = { RaylibWrapper::GetMousePosition().x - ImGui::GetWindowPos().x, RaylibWrapper::GetMousePosition().y - ImGui::GetWindowPos().y};
                //ImVec2 itemPos = ImGui::GetCursorPos();
                RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["FolderIcon"], ImVec2(32, 32));

                if (ImGui::IsItemHovered())
                {
                    if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over a folder, it will select that folder
                    {
                        dragData.first = Folder;
                        dragData.second["Path"] = entry.path();
                    }
                    else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        fileExplorerPath = entry.path();
                        if (hovered)
                            ImGui::PopStyleColor(3);
                        else
                            ImGui::PopStyleColor(2);
                        ImGui::PopID();
                        ImGui::End();
                        return;
                    }
                }

                //if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) // Using this since ImGui::IsItemHovered() won't work. ImGui::IsWindowHovered() also won't work for this. It seems these break when attempting to drag a button
                    //ConsoleLogger::ErrorLog("Hovered");
                if (hovered)
                    ImGui::PopStyleColor();
                //ConsoleLogger::InfoLog("Mouse Pos: " + std::to_string(mousePos.x) + " " + std::to_string(mousePos.y));
                //ConsoleLogger::InfoLog("Item Pos: " + std::to_string(itemPos.x) + " " + std::to_string(itemPos.y));
                //if (ImGui::IsWindowHovered() && (mousePos.x >= itemPos.x && mousePos.x <= (itemPos.x + 32) &&
                //    mousePos.y >= itemPos.y && mousePos.y <= (itemPos.y + 32)))
                //{
                //    hoveringButton = true;
                //    ConsoleLogger::ErrorLog("Hovering");
                //}
                //else
                //    hoveringButton = false;

            }
            else if (entry.is_regular_file())
            {
                std::string extension = entry.path().extension().string();
                if (extension == ".cpp")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["CppIcon"], ImVec2(32, 32));
                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            //std::string command = "code " + entry.path().string(); // VSCode
                            std::system(("\"" + entry.path().string() + "\"").c_str()); // Use prefered editor
                        }
                    }
                }
                else if (extension == ".h")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["HeaderIcon"], ImVec2(32, 32));
                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            //std::string command = "code " + entry.path().string(); // VSCode
                            std::system(("\"" + entry.path().string() + "\"").c_str()); // Use prefered editor
                        }
                    }
                }
                else if (extension == ".png") // Todo: Add jpg support
                {
                    tempTextures.push_back(new RaylibWrapper::Texture2D(RaylibWrapper::LoadTexture(entry.path().string().c_str())));
                    float aspectRatio = (float)tempTextures.back()->width / (float)tempTextures.back()->height;
                    ImVec2 imageSize = ImVec2(0, 0);
                    imageSize.x = aspectRatio > 1.0f ? 32 : 32 * aspectRatio;
                    imageSize.y = aspectRatio > 1.0f ? 32 / aspectRatio : 32;
                    int buttonSize = 32 + ImGui::GetStyle().FramePadding.x * 2;

                    ImVec2 cursorPos = ImGui::GetCursorPos();

                    //RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), tempTextures.back(), ImVec2(32, 32));
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::Button(("##" + id).c_str(), ImVec2(buttonSize, buttonSize));
                    ImGui::PopStyleColor();

                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = ImageFile;
                            dragData.second["Texture"] = RaylibWrapper::LoadTexture(entry.path().string().c_str());
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            std::system(("\"" + entry.path().string() + "\"").c_str());
                    }

                    ImVec2 newCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(ImVec2(cursorPos.x + (buttonSize - imageSize.x) / 2, cursorPos.y + (buttonSize - imageSize.y) / 2)); // Sets the position to the button position and centers it
                    RaylibWrapper::rlImGuiImageSize(tempTextures.back(), imageSize.x, imageSize.y);
                    ImGui::SetCursorPos(newCursorPos);
                }
                else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac" || extension == ".qoa" || extension == ".xm" || extension == ".mod")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["SoundIcon"], ImVec2(32, 32)) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        {
                            // Puts the data into objectInProperties so it is displayed in the properties window
                            if (!std::holds_alternative<DataFile>(objectInProperties) || std::filesystem::path(std::get<DataFile>(objectInProperties).path) != (entry.path().string() + ".data"))
                            {
                                if (!std::filesystem::exists(entry.path().string() + ".data"))
                                    Utilities::CreateDataFile(entry.path());

                                DataFile dataFile;
                                dataFile.path = entry.path().string() + ".data";
                                dataFile.type = DataFileTypes::Sound;

                                std::ifstream jsonFile(dataFile.path);
                                dataFile.json = nlohmann::json::parse(jsonFile);

                                objectInProperties = dataFile;
                            }
                        }
                    }
                }
                else if (extension == ".gltf" || extension == ".glb")
                {
                    // Todo: This is causing errors if there are game objects with a MeshRenderer
                    //if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), &CreateModelPreview(entry.path(), 32)->texture, ImVec2(32, 32)))
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["CubeIcon"], ImVec2(32, 32));

                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = ModelFile;
                            dragData.second["Path"] = entry.path();
                        }
                    }
                }
                else if (extension == ".animgraph")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["AnimationGraphIcon"], ImVec2(32, 32));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        // Todo: If something is already in the Animation Graph window and its not saved, popup asking to save, don't save, or cancel
                        
                        // Todo: Create a AnimationGraph script and put OpenAnimationGraph() in it.
                        std::ifstream dataFile(entry.path());
                        if (dataFile.is_open())
                        {
                            // Todo: Check if the dataFile has data in it. If it doesn't then add default data to it so it won't crash.
                            dataFile >> animationGraphData;
                            animationGraphData["path"] = entry.path(); // Updating the path incase it changed.

                            if (animationGraphWinOpen) // If the animation graph is open, I need to focus it
                            {
                                ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_PERSON_RUNNING + std::string(" Animation Graph")).c_str());
                                if (window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL)
                                    window->DockNode->TabBar->NextSelectedTabId = window->TabId;
                            }
                            else
                                animationGraphWinOpen = true; // This should focus the animation graph window
                        }
                        else
                        {
                            // Todo: Send error message
                        }
                    }

                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                    }
                }
                else if (extension == ".scene")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["SceneIcon"], ImVec2(32, 32));
                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && SceneManager::GetActiveScene()->GetPath() != entry.path())
                        {
                            // Todo: Popup save window if the scene isn't saved
                            objectInProperties = std::monostate{};
                            SceneManager::SaveScene(SceneManager::GetActiveScene());
                            SceneManager::LoadScene(entry.path());
                        }
                    }
                }
                else if (extension != ".data")
                {
                    RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["UnknownFile"], ImVec2(32, 32)) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
                    if (ImGui::IsItemHovered())
                    {
                        if (dragData.first == None && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 5)) // Todo: If the user holds down on nothing and moves mouse over an image file, it will select that file
                        {
                            dragData.first = Other;
                            dragData.second["Path"] = entry.path();
                        }
                        else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            std::system(("\"" + entry.path().string() + "\"").c_str());
                    }
                }
                else
                {
                    ImGui::PopID();
                    continue;
                }
            }
            ImGui::PushFont(FontManager::GetFont("Roboto-Bold", 15, false));
            ImGui::SetCursorPosY(ImGui::GetCursorPosY());
            ImGui::SetCursorPosX(nextX + (32/2) - (ImGui::CalcTextSize(fileName.c_str()).x / 2) + ImGui::CalcTextSize(" ").x); // ImGui::CalcTextSize(" ").x adds one space so its properly aligned
            //ImGui::SetCursorPosX(nextX - 2);
            if (!renamingFile.empty() && renamingFile.filename().string() == entry.path().filename().string())
            {
                ImGui::SetNextItemWidth(44);
                ImGui::SetKeyboardFocusHere(0);
                if (ImGui::InputText("##RenameInput", newFileName, sizeof(newFileName), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (newFileName != "" || newFileName == renamingFile.stem())
                    {
                        // Todo: If a file already exists with that name and extension, popup a warning asking if they want to replace/overwrite or cancel
                        std::filesystem::rename(renamingFile, (renamingFile.parent_path() / (newFileName + renamingFile.extension().string())));
                        fileRenameFirstFrame = true;
                    }
                    renamingFile = "";
                    strcpy_s(newFileName, sizeof(newFileName), "");
                }

                if (!ImGui::IsItemClicked() && ImGui::IsKeyPressed(ImGuiKey_MouseLeft))
                {
                    renamingFile = "";
                    strcpy_s(newFileName, sizeof(newFileName), "");
                    fileRenameFirstFrame = true;
                }
            }
            else ImGui::Text(fileName.c_str()); // Todo: Center text
            ImGui::PopFont();

            nextX += 60;

            if (nextX > ImGui::GetWindowWidth() - 32)
            {
                nextX = 310;
                nextY = nextY + 75;
            }
            ImGui::PopID();
        }
        ImGui::PopStyleColor(2);

        // Drag and drop code
        // Todo: For ImageFile and ModelFile, add support for dropping into hierarchy
        if (dragData.first == ImageFile)
        {
            //ImGui::SetNextWindowSize(ImVec2(32, 32));
            //ImGui::SetNextWindowPos(ImGui::GetMousePos()); // Todo: Add offset
            //ImGui::SetNextWindowBgAlpha(0.0f);

            //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            //ImGui::Begin("DragWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
            //RaylibWrapper::rlImGuiImageSize(std::any_cast<RaylibWrapper::Texture2D*>(dragData.second["Texture"]), ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
            //ImGui::End();
            //ImGui::PopStyleVar(3);

            static RaylibWrapper::MouseCursor currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;

            //viewportHovered = ImGui::IsMouseHoveringRect({viewportPosition.x, viewportPosition.y}, { viewportPosition.z, viewportPosition.w }); // This doesn't work for some reason, neither does IsWindowHovered() in the viewport window
            ImVec2 mousePos = ImGui::GetMousePos();
            viewportHovered = (mousePos.x >= viewportPosition.x && mousePos.x <= viewportPosition.z &&
                mousePos.y >= viewportPosition.y && mousePos.y <= viewportPosition.w); // Todo: This being set 3 times a frame. First in UpdateViewport (if dragging), then RenderViewport, then here.

            if (viewportHovered)
            {
                if (currentCursor != RaylibWrapper::MOUSE_CURSOR_DEFAULT)
                {
                    RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                    currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
                }
            }
            else if (currentCursor != RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED)
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (ImGui::IsWindowHovered())
                {
                    if (!folderHovering.empty()) // Todo: Add support for dropping files onto folders in the file explorer tree, and the previous folders buttons near the top of the file explorer
                    {
                        std::filesystem::path path = std::any_cast<std::filesystem::path>(dragData.second["Path"]);
                        std::filesystem::rename(path, folderHovering / path.filename());
                    }
                }
                else if (viewportHovered)
                {
                    RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
                    mousePosition.x = (mousePosition.x - viewportPosition.x) / (viewportPosition.z - viewportPosition.x) * RaylibWrapper::GetScreenWidth();
                    mousePosition.y = (mousePosition.y - viewportPosition.y) / (viewportPosition.w - viewportPosition.y) * RaylibWrapper::GetScreenHeight();
                    RaylibWrapper::Vector3 position = RaylibWrapper::GetMouseRay(mousePosition, camera).position;

                    std::filesystem::path texturePath = std::any_cast<std::filesystem::path>(dragData.second["Path"]);

                    GameObject* gameObject = SceneManager::GetActiveScene()->AddGameObject();
                    gameObject->transform.SetPosition({ position.x, position.y, 0 });
                    gameObject->transform.SetScale({ 1,1,1 });
                    gameObject->transform.SetRotation(Quaternion::Identity());
                    gameObject->SetName(texturePath.stem().string());

                    auto assetsPosition = texturePath.string().find("Assets");
                    if (assetsPosition != std::string::npos)
                        texturePath = texturePath.string().substr(assetsPosition + 7);

                    SpriteRenderer& spriteRenderer = gameObject->AddComponent<SpriteRenderer>();
                    spriteRenderer.SetTexture(ProjectManager::projectData.path / "Assets" / texturePath);
                    gameObject->AddComponent<Collider2D>(); // Todo: Set to convex/texture type

                    for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back()->GetComponents())
                        component->gameObject = SceneManager::GetActiveScene()->GetGameObjects().back();
                    
                    //if (selectedObject != nullptr)
                        //EventSystem::Invoke("ObjectDeselected", selectedObject);
                    selectedObject = SceneManager::GetActiveScene()->GetGameObjects().back();
                    objectInProperties = SceneManager::GetActiveScene()->GetGameObjects().back();
                    //EventSystem::Invoke("ObjectSelected", selectedObject);
                }

                dragData.first = None;
                RaylibWrapper::UnloadTexture(std::any_cast<RaylibWrapper::Texture2D>(dragData.second["Texture"]));
                dragData.second.clear();
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
                // Check if in in file explorer and hovering over folder
            }
            if (!folderHovering.empty())
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
            }
        }
        else if (dragData.first == ModelFile && ProjectManager::projectData.is3D)
        {
            // Todo: Show the model while placing it down, like with sprites
            static RaylibWrapper::MouseCursor currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;

            //viewportHovered = ImGui::IsMouseHoveringRect({viewportPosition.x, viewportPosition.y}, { viewportPosition.z, viewportPosition.w }); // This doesn't work for some reason, neither does IsWindowHovered() in the viewport window
            ImVec2 mousePos = ImGui::GetMousePos();
            viewportHovered = (mousePos.x >= viewportPosition.x && mousePos.x <= viewportPosition.z &&
                mousePos.y >= viewportPosition.y && mousePos.y <= viewportPosition.w); // Todo: This being set 3 times a frame. First in UpdateViewport (if dragging), then RenderViewport, then here.

            if (viewportHovered)
            {
                if (currentCursor != RaylibWrapper::MOUSE_CURSOR_DEFAULT)
                {
                    RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                    currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
                }
            }
            else if (currentCursor != RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED)
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                // Checks if hovering Content Browser
                ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_FOLDER_OPEN + std::string(" Content Browser")).c_str());
                if (ImGui::IsWindowHovered() && window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL && window->DockNode->TabBar->SelectedTabId == window->TabId)
                {
                    if (!folderHovering.empty()) // Todo: Add support for dropping files onto folders in the file explorer tree, and the previous folders buttons near the top of the file explorer
                    {
                        std::filesystem::path path = std::any_cast<std::filesystem::path>(dragData.second["Path"]);
                        std::filesystem::rename(path, folderHovering / path.filename());
                    }
                }
                else
                {
                    // Checks if hoveirng viewport
                    window = ImGui::FindWindowByName((ICON_FA_CUBES + std::string(" Viewport")).c_str());
                    if (viewportHovered && window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL && window->DockNode->TabBar->SelectedTabId == window->TabId)
                    {
                        RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
                        mousePosition.x = (mousePosition.x - viewportPosition.x) / (viewportPosition.z - viewportPosition.x) * RaylibWrapper::GetScreenWidth();
                        mousePosition.y = (mousePosition.y - viewportPosition.y) / (viewportPosition.w - viewportPosition.y) * RaylibWrapper::GetScreenHeight();
                        RaylibWrapper::Vector3 position = RaylibWrapper::GetMouseRay(mousePosition, camera).position;

                        // Todo: Properly set the Z position

                        //ConsoleLogger::ErrorLog("Current Z: " + std::to_string(camera.position.z));
                        //ConsoleLogger::ErrorLog("Z Ray: " + std::to_string(position.z));

                        //for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
                        //{
                        //    if (!gameObject->IsActive())
                        //        continue;
                        //    MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
                        //    if (meshRenderer == nullptr)
                        //        continue;

                        //    for (int i = 0; i < model.mesh.vertexCount; i += 3)
                        //    {
                        //        Vector3 v0 = model.transform * model.mesh.vertices[i];
                        //        Vector3 v1 = model.transform * model.mesh.vertices[i + 1];
                        //        Vector3 v2 = model.transform * model.mesh.vertices[i + 2];
                        //        Triangle triangle = { v0, v1, v2 };

                        //        if (CheckCollisionRayTriangle(ray, triangle, &hitPosition))
                        //        {
                        //            DrawSphere(hitPosition, 0.1f, GREEN); // Draw a sphere at the intersection point
                        //            break; // Exit loop if intersection found
                        //        }
                        //    }
                        //}

                        // Todo: Add raycast (that detects every game object, including ones without colliders)


                        std::filesystem::path modelPath = std::any_cast<std::filesystem::path>(dragData.second["Path"]);
                        auto assetsPosition = modelPath.string().find("Assets");
                        if (assetsPosition != std::string::npos)
                            modelPath = modelPath.string().substr(assetsPosition + 7);

                        GameObject* gameObject = SceneManager::GetActiveScene()->AddGameObject();
                        gameObject->transform.SetPosition({ position.x, position.y, position.z });
                        gameObject->transform.SetScale({ 1,1,1 });
                        gameObject->transform.SetRotation(Quaternion::Identity());
                        gameObject->SetName(modelPath.stem().string());

                        MeshRenderer& meshRenderer = gameObject->AddComponent<MeshRenderer>();
                        meshRenderer.SetModelPath(modelPath);
                        meshRenderer.SetModel(ModelType::Custom, modelPath, LitStandard);

                        for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back()->GetComponents())
                            component->gameObject = SceneManager::GetActiveScene()->GetGameObjects().back();

                        //if (selectedObject != nullptr)
                            //EventSystem::Invoke("ObjectDeselected", selectedObject);
                        selectedObject = SceneManager::GetActiveScene()->GetGameObjects().back();
                        objectInProperties = SceneManager::GetActiveScene()->GetGameObjects().back();
                        //EventSystem::Invoke("ObjectSelected", selectedObject);
                    }
                    else
                    {
                        // Checks if hovering animation graph
                        ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_PERSON_RUNNING + std::string(" Animation Graph")).c_str());
                        if (animationGraphWinOpen && animationGraphHovered && window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL && window->DockNode->TabBar->SelectedTabId == window->TabId)
                        {
                            RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
                            mousePosition.x = (mousePosition.x - viewportPosition.x) / (viewportPosition.z - viewportPosition.x) * RaylibWrapper::GetScreenWidth();
                            mousePosition.y = (mousePosition.y - viewportPosition.y) / (viewportPosition.w - viewportPosition.y) * RaylibWrapper::GetScreenHeight();
                            RaylibWrapper::Vector3 position = RaylibWrapper::GetMouseRay(mousePosition, camera).position;

                            // Todo: Place the nodes at the mouse position

                            bool updatedAnimations = false;

                            int index = 0;
                            std::random_device rd;
                            std::mt19937 gen(rd());
                            std::uniform_int_distribution<int> distribution(999999, 99999999);
                            // Todo: Popup a window with a progress bar
                            // Todo: This function is really slow to get animations
                            for (std::string& animationName : Utilities::GetGltfAnimationNames(std::any_cast<std::filesystem::path>(dragData.second["Path"])))
                            {
                                int id = 0;
                                while (id == 0)
                                {
                                    id = distribution(gen);
                                    for (auto& node : animationGraphData["nodes"])
                                    {
                                        if (id == node["id"])
                                        {
                                            id = 0;
                                            break;
                                        }
                                    }
                                }

                                nlohmann::json node = {
                                    {"id", id},
                                    {"name", animationName},
                                    {"index", index},
                                    {"x", 350 + (index * 30)},
                                    {"y", 350 + (index * 30)},
                                    {"loop", true},
                                    {"speed", 1.0f}
                                };

                                animationGraphData["nodes"].push_back(node);
                                updatedAnimations = true;
                                index++;
                            }

                            if (updatedAnimations)
                            {
                                std::ofstream file(animationGraphData["path"].get<std::filesystem::path>());
                                if (file.is_open())
                                {
                                    file << std::setw(4) << animationGraphData << std::endl;
                                    file.close();
                                }
                            }
                        }
                    }
                }
                dragData.first = None;
                dragData.second.clear();
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
                // Check if in in file explorer and hovering over folder
            }
            if (!folderHovering.empty())
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
            }
        }
        else if (dragData.first == Folder)
        {
            static RaylibWrapper::MouseCursor currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;

            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            // Using this since ImGui::IsWindowHovered() doesn't seem to work when dragging
            bool windowHovered = (mousePos.x >= windowPos.x && mousePos.x <= (windowPos.x + ImGui::GetWindowWidth()) &&
                mousePos.y >= windowPos.y && mousePos.y <= (windowPos.y + ImGui::GetWindowHeight()));
            ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_FOLDER_OPEN + std::string(" Content Browser")).c_str());
            if (windowHovered && window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL && window->DockNode->TabBar->SelectedTabId == window->TabId)
            {
                if (!folderHovering.empty() && folderHovering != std::any_cast<std::filesystem::path>(dragData.second["Path"])) // Todo: Add support for dropping folders onto folders in the file explorer tree, and the previous folders buttons near the top of the file explorer
                {
                    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                    {
                        try {
                            std::filesystem::path path = std::any_cast<std::filesystem::path>(dragData.second["Path"]);
                            std::filesystem::rename(path, folderHovering / path.filename());
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            ConsoleLogger::ErrorLog(std::string("Error moving folder: ") + e.what(), false);
                        }
                        catch (const std::bad_any_cast& e) {
                            ConsoleLogger::ErrorLog(std::string("Error moving folder: ") + e.what());
                        }
                        catch (const std::exception& e) {
                            ConsoleLogger::ErrorLog(std::string("Error moving folder: ") + e.what());
                        }
                    }
                    else if (currentCursor != RaylibWrapper::MOUSE_CURSOR_DEFAULT)
                    {
                        RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                        currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;
                    }
                }
                else if (currentCursor != RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED)
                {
                    RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED);
                    currentCursor = RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED;
                }
            }
            else if (currentCursor != RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED)
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED;
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
                currentCursor = RaylibWrapper::MOUSE_CURSOR_DEFAULT;

                dragData.first = None;
                dragData.second.clear();
            }
        }
        else if (dragData.first != None)
        {
            // Todo: This is not optimized and will be setting the cursor each frame
            if (!folderHovering.empty())
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);
            else
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_NOT_ALLOWED);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                RaylibWrapper::SetMouseCursor(RaylibWrapper::MOUSE_CURSOR_DEFAULT);

                dragData.first = None;
                dragData.second.clear();
            }
        }


        // File Explorer Tree
        int xSize = 300;
        ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetWindowPos().x + xSize, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + xSize, ImGui::GetWindowPos().y + ImGui::GetWindowWidth()), IM_COL32(0, 0, 0, 255), 1);
        ImGui::SetCursorPos(ImVec2(0, 25));
        if (ImGui::BeginChild("##FileExplorerTree", ImVec2(xSize, ImGui::GetWindowHeight() - 35))) // Todo: Make this a dockable window or something so it resizes properly and can be resized. Check if I need this -35 (or if 35 is the best number)
        {
            ImGui::BeginTable("FileExplorerTreeTable", 1);
            RenderFileExplorerTreeNode(ProjectManager::projectData.path / "Assets", true);
            ImGui::EndTable();
        }
        ImGui::EndChild();


        // Context Menu
        if (explorerContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
        {
            explorerContextMenuOpen = true;
            if (ImGui::BeginPopupContextWindow())
            {
                // Todo: Make Rename only fix right click on a file
                //if (ImGui::MenuItem("Rename", "F2"))
                //{
                //    explorerContextMenuOpen = false;
                //}
                if (ImGui::MenuItem("Create Script"))
                {
                    explorerContextMenuOpen = false;
                    scriptCreateWinOpen = true;
                }
                if (ImGui::MenuItem("Create Scene"))
                {
                    explorerContextMenuOpen = false;
                    std::filesystem::path filePath = Utilities::CreateUniqueFile(fileExplorerPath, "New Scene", "scene");
                    SceneManager::CreateScene(filePath);
                    if (filePath != "")
                    {
                        renamingFile = filePath;
                        strcpy_s(newFileName, sizeof(newFileName), filePath.stem().string().c_str());
                    }
                    else
                    {
                        // Todo: Handle if it wasn't created
                    }
                }
                if (ImGui::MenuItem("Create Sprite"))
                {
                    explorerContextMenuOpen = false;
                }
                if (ImGui::MenuItem("Create Material"))
                {
                    explorerContextMenuOpen = false;
                    std::filesystem::path filePath = Utilities::CreateUniqueFile(fileExplorerPath, "New Material", "mat");
                    if (filePath != "")
                    {
                        renamingFile = filePath;
                        strcpy_s(newFileName, sizeof(newFileName), filePath.stem().string().c_str());
                    }
                    else
                    {
                        // Todo: Handle if it wasn't created
                    }
                }
                //if (ImGui::MenuItem("Create Animation"))
                //{
                //    explorerContextMenuOpen = false;
                //    std::filesystem::path filePath = Utilities::CreateUniqueFile(fileExplorerPath, "New Animation", "anim");
                //    if (filePath != "")
                //    {
                //        std::ofstream file(filePath);
                //        if (file.is_open())
                //        {
                //            nlohmann::json jsonData = {
                //                {"version", 1},
                //                {"path", filePath},
                //                {"loop", true},
                //                {"speed", 1.0f},
                //                {"sprites", {}}
                //            };

                //            file << std::setw(4) << jsonData << std::endl;
                //            file.close();
                //        }
                //        else
                //        {
                //            // Todo: Properly handle if the file couldn't be opened. Maybe retry a few times, then popup with a message and delete the file.
                //            std::filesystem::remove(filePath);
                //        }


                //        renamingFile = filePath;
                //        strcpy_s(newFileName, sizeof(newFileName), filePath.stem().string().c_str());
                //    }
                //    else
                //    {
                //        // Todo: Handle if it wasn't created
                //    }
                //}
                if (ImGui::MenuItem("Create Animation Graph"))
                {
                    explorerContextMenuOpen = false;
                    std::filesystem::path filePath = Utilities::CreateUniqueFile(fileExplorerPath, "Animation Graph", "animgraph");
                    if (filePath != "")
                    {
                        std::ofstream file(filePath);
                        if (file.is_open())
                        {
                            nlohmann::json jsonData = {
                                {"version", 1},
                                {"path", filePath},
                                {"model_path", ""},
                                {"nodes", {
                                    {
                                        {"id", -5},
                                        {"name", "Start"},
                                        {"x", 80},
                                        {"y", 350},
                                        {"loop", false},
                                        {"speed", 0.0f}
                                    }
                                }},
                                {"links", nlohmann::json::array()}
                            };

                            file << std::setw(4) << jsonData << std::endl;
                            file.close();
                        }
                        else
                        {
                            // Todo: Properly handle if the file couldn't be opened. Maybe retry a few times, then popup with a message and delete the file.
                            std::filesystem::remove(filePath);
                        }


                        renamingFile = filePath;
                        strcpy_s(newFileName, sizeof(newFileName), filePath.stem().string().c_str());
                    }
                    else
                    {
                        // Todo: Handle if it wasn't created
                    }
                }

                if (ImGui::MenuItem("Create Folder"))
                {
                    explorerContextMenuOpen = false;
                    std::filesystem::path folderPath = Utilities::CreateUniqueFile(fileExplorerPath, "New Folder", "");
                    if (folderPath != "")
                    {
                        renamingFile = folderPath;
                        strcpy_s(newFileName, sizeof(newFileName), folderPath.stem().string().c_str());
                    }
                    else
                    {
                        // Todo: Handle if it wasn't created
                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Open In File Explorer"))
                {
                    explorerContextMenuOpen = false;
                    Utilities::OpenPathInExplorer(fileExplorerPath);
                }

                ImGui::EndPopup();
            }

        }

        // Drag and drop files
        if (ImGui::IsWindowHovered() && RaylibWrapper::IsFileDropped()) 
        {
            // Todo: Consider adding a progress bar, especially if there is a lot of files and directories, so then the program doesn't freeze and confuse the user
            RaylibWrapper::FilePathList droppedFiles = RaylibWrapper::LoadDroppedFiles();

            for (int i = 0; i < (int)droppedFiles.count; i++)
            {
                Utilities::ImportFile(droppedFiles.paths[i], fileExplorerPath / std::filesystem::path(droppedFiles.paths[i]).filename());
                // Todo: Popup a window with the error. ImportFile() returns a bool, but this won't work if a file within a folder fails. Maybe have it return a vector of strings holding the errors? Or handle this within the ImportFile()
            }

            UnloadDroppedFiles(droppedFiles);
        }
    }
    ImGui::End();
}

void Editor::RenderFileExplorerTreeNode(std::filesystem::path path, bool openOnDefault)
{
    bool hasChildren = false;
    if (!std::filesystem::exists(path)) // This is important as it prevents a crash if the file is deleted from an external application like File Explorer
        return;
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            hasChildren = true;
            break;
        }
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (!hasChildren)
        flags |= ImGuiTreeNodeFlags_Leaf;
    else
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;

    if (path == fileExplorerPath)
        flags |= ImGuiTreeNodeFlags_Selected;

    if (openOnDefault)
        flags |= ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (ImGui::TreeNodeEx((ICON_FA_FOLDER_OPEN + std::string(" ") + path.stem().string() + "##" + path.string()).c_str(), flags))
    {
        if (!ImGui::IsItemToggledOpen())
        {
            if (ImGui::IsItemClicked())
                fileExplorerPath = path;
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if (entry.is_directory())
                    RenderFileExplorerTreeNode(entry, false);
            }
        }

        ImGui::TreePop();
    }
}

std::string RenderFileSelector(int id, std::string type, std::string selectedPath, std::vector<std::string> extensions, ImVec2 position)
{
    static bool open = true;
    static char searchBuffer[256];
    // Using oldId to identify if the same thing is being set as previously so it will know if it needs to reset some values
    static int oldId = -9999;
    if (id != oldId)
    {
        open = true;
        memset(searchBuffer, '\0', sizeof(searchBuffer));
        ImGui::SetNextWindowScroll({0,0});
    }
    std::string selectedFile = "";
    ImGui::SetNextWindowSize(ImVec2(200, 250));
    ImGui::SetNextWindowPos(position);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    ImGui::Begin((" Select " + type).c_str(), &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);

    ImGui::SetCursorPos({ 10, 20 });
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 23);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::InputTextWithHint("##FileSearch", "Search...", searchBuffer, sizeof(searchBuffer));
    ImGui::PopStyleVar();

    ImGui::SetCursorPos({0, 45});
    ImGui::BeginTable("##FileSelectorTable", 1);

    // Todo: It may not be best to iterate the files each frame. Maybe store them in a static and only update them when the window is opened
    // Todo: Order the files in alphabetical order
    // Todo: Make a gap between the bottom of the table and bottom of the window since the nodes make it so the bottom left of the window isn't rounded

    // "None" node
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;
    if (!selectedPath.empty())
        flags |= ImGuiTreeNodeFlags_Selected;
    if (ImGui::TreeNodeEx("None", flags))
    {
        if (ImGui::IsItemClicked())
            selectedFile = "None";
        ImGui::TreePop();
    }

    try
    {
        for (const auto& file : std::filesystem::recursive_directory_iterator(ProjectManager::projectData.path / "Assets"))
        {
            if (std::filesystem::is_regular_file(file) && std::find(extensions.begin(), extensions.end(), file.path().extension().string()) != extensions.end())
            {
                // This converts the search and file name to lower case so its case insensitive
                std::string search = searchBuffer;
                std::string fileName = file.path().stem().string();
                std::transform(search.begin(), search.end(), search.begin(), ::tolower);
                std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
                if (searchBuffer[0] == '\0' || fileName.find(search) != std::string::npos)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    // Using ImGuiTreeNodeFlags_Framed flag adds a frame and aligns it to the left, but then selected flag doesn't work. I would need to set the node color if I want to make it so they have a frame or aligned to the left
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;
                    // Checks to see if the file path contains the selectedPath and if it does, then select it
                    if (!selectedPath.empty() && file.path().string().find(selectedPath) != std::string::npos)
                        flags |= ImGuiTreeNodeFlags_Selected;
                    if (ImGui::TreeNodeEx((file.path().stem().string()).c_str(), flags))
                    {
                        if (ImGui::IsItemClicked())
                        {
                            oldId = -9999;
                            selectedFile = "NULL";
                            memset(searchBuffer, '\0', sizeof(searchBuffer));
                            ImGui::SetScrollY(0.0f);
                            selectedFile = std::filesystem::relative(file.path(), ProjectManager::projectData.path / "Assets").string();
                        }

                        ImGui::TreePop();
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ConsoleLogger::ErrorLog("Failed to iterate project files to find " + type + " files", true);
        memset(searchBuffer, '\0', sizeof(searchBuffer));
        selectedFile = "NULL";
        oldId = -9999;
        ImGui::SetScrollY(0.0f);
    }

    ImGui::EndTable();

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);

    if (!open)
    {
        oldId = -9999;
        open = true;
        selectedFile = "NULL";
        memset(searchBuffer, '\0', sizeof(searchBuffer));
        // Setting the scroll here does not work.
        ImGui::SetScrollY(0.0f);
    }

    oldId = id;
    return selectedFile;
}

void Editor::RenderAnimationGraph()
{
    if (!animationGraphWinOpen) return;
    //ImGui::SetNextWindowSize(ImVec2(180, 180));
    //ImGui::SetNextWindowPos(ImVec2((RaylibWrapper::GetScreenWidth() - 180) / 2, (RaylibWrapper::GetScreenHeight() - 180) / 2));

    animationGraphHovered = true;
    static nlohmann::json* selectedNode = nullptr;

    // Todo: Check model to see if it has new animations (or if animations were moved/renamed)

    if (ImGui::Begin((ICON_FA_PERSON_RUNNING + std::string(" Animation Graph")).c_str(), &animationGraphWinOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        bool update = false;
        ImNodes::BeginNodeEditor();
        if (animationGraphData.is_null())
        {
            // Todo: Fade background and make it so user can't move grid
            ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
            ImVec2 textSize = ImGui::CalcTextSize("No Animation Graph selected. Select or create one in the Content Browser.");
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowWidth() - textSize.x) * 0.5f, (ImGui::GetWindowHeight() - textSize.y) * 0.5f));
            ImGui::Text("No Animation Graph selected. Select or create one in the Content Browser.");
            ImGui::PopFont();
            ImNodes::EndNodeEditor();
            ImGui::End();
            selectedNode = nullptr;
            return;
        }

        // Todo: Add "Any State" node
        // Todo: Ability to create multiple "Any State" nodes to cleanup the graph. Make it so ALL any state nodes can be removed including the last one
        // Todo: Save user position in grid to load it. (It doesn't seem to be implemented into ImNodes)
        // Todo: Make all nodes fixed sizes
        // Todo: Add zooming
        // Todo: Support external animations (from other model files).
        // Todo: If a model is reimported, make sure to check the animations and make sure the animations index values are correct with the actual index
        // Todo: Make sure it doesn't update when dragging a node, only once its dropped
        // Todo: Use node titles to let the user know if theres any warnings/errors
        // Todo: If the user selects any default nodes like Start and switches to a new graph while its still selected, it will move the position of the node in the new graph to where it was in the old graph

        for (auto& node : animationGraphData["nodes"])
        {
            int id = node["id"];
            std::string name = node["name"];
            if (id == -5) // Start node
            {
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImNodes::IsNodeSelected(-5))
                {
                    node["x"] = ImNodes::GetNodeGridSpacePos(id).x;
                    node["y"] = ImNodes::GetNodeGridSpacePos(id).y;
                    update = true;
                }
                if (!ImNodes::IsNodeSelected(id) && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    ImNodes::SetNodeGridSpacePos(-5, ImVec2(node["x"], node["y"]));
                ImNodes::BeginNode(-5);
                ImNodes::BeginOutputAttribute(-5);
                ImGui::Text("Start");
                ImNodes::EndOutputAttribute();
                ImNodes::EndNode();
            }
            else
            {
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImNodes::IsNodeSelected(id))
                {
                    node["x"] = ImNodes::GetNodeGridSpacePos(id).x;
                    node["y"] = ImNodes::GetNodeGridSpacePos(id).y;
                    update = true;
                }
                if (!ImNodes::IsNodeSelected(id) && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    ImNodes::SetNodeGridSpacePos(id, ImVec2(node["x"], node["y"]));
                ImNodes::BeginNode(id);
                ImNodes::BeginInputAttribute(abs(node["id"].get<int>())); // Using the positive ID for input ID
                ImNodes::EndInputAttribute();
                ImNodes::BeginOutputAttribute(node["id"].get<int>() * -1); // Using the negative ID for output ID
                ImGui::Text(node["name"].get<std::string>().c_str());
                ImNodes::EndOutputAttribute();
                ImNodes::EndNode();
            }
        }

        for (int i = 0; i < animationGraphData["links"].size(); ++i)
            ImNodes::Link(i, animationGraphData["links"][i][0], animationGraphData["links"][i][1]);

        static int selectNode = 0;
        int previousSelectNode = selectNode;
        static int openAddSpriteWin = -1; // -2 = Don't open, -1 = open for a new sprite, any else opens to change the sprite at the index (openAddSpriteWin holds the index)
        if (selectedNode == nullptr)
            openAddSpriteWin = -2;

        if (!animationGraphData.is_null())
        {
            if (selectedNode == nullptr || (*selectedNode)["id"] == -5) // Checks if selectedNode is null, or if the selected node is the Start node
            {
                ImGui::SetCursorPos({ ImGui::GetWindowWidth() - 130, 10 });
                if (ImGui::Button("Create Animation", { 120, 30 }))
                {
                    // Todo: Place the node in the center of the current position rather than this hardcoded solution

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<int> distribution(999999, 99999999);

                    int id = 0;
                    while (id == 0)
                    {
                        id = distribution(gen);
                        for (auto& node : animationGraphData["nodes"])
                        {
                            if (id == node["id"])
                            {
                                id = 0;
                                break;
                            }
                        }
                    }

                    nlohmann::json node = {
                        {"id", id},
                        {"name", "New Animation"}, // Todo: It may be best to add numbers at the end of the name if there are others with this name
                        {"index", 0}, // Todo: Not sure why node's have the index element, see if they are needed
                        {"x", 350},
                        {"y", 350},
                        {"loop", true},
                        {"speed", 1.0f},
                        {"sprites", {}}
                    };
                    animationGraphData["nodes"].push_back(node);
                    
                    selectNode = id;
                    update = true;
                }
            }
            else
            {
                ImGui::SetCursorPos({ImGui::GetWindowWidth() - 160, 10});
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.13f, 0.14f, 0.15f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

                float height = 90;
                if (selectedNode->contains("sprites"))
                {
                    height = 170 + (*selectedNode)["sprites"].size() * 100;
                    if (height > ImGui::GetWindowHeight() - 20)
                        height = ImGui::GetWindowHeight() - 20;
                }

                ImGui::BeginChild("##Animation Graph Properties", {150, height });

                char nameBuffer[256];
                strcpy_s(nameBuffer, (*selectedNode)["name"].get<std::string>().c_str());
                ImGui::SetCursorPos({10, 10});
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 20);
                if (ImGui::InputText("##AnimationName", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    std::string temp = nameBuffer;
                    temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
                    if (!temp.empty())
                    {
                        // Todo: Consider checking if an animation with the name already exists and if it does then add a number to the end of it
                        (*selectedNode)["name"] = nameBuffer;
                        update = true;
                    }
                }

                ImGui::SetCursorPos({ 10, 40 });
                ImGui::Text("Loop");
                bool loop = (*selectedNode)["loop"].get<bool>();
                ImGui::SetCursorPos({ 60, 40 });
                if (ImGui::Checkbox("##AnimationLoop", &loop))
                {
                    (*selectedNode)["loop"] = loop;
                    update = true;
                }

                ImGui::SetCursorPos({ 10, 70 });
                ImGui::Text("Speed");
                float speed = (*selectedNode)["speed"].get<float>();
                ImGui::SetNextItemWidth(40);
                ImGui::SetCursorPos({ 60, 70 });
                if (ImGui::InputFloat("##AnimationSpeed", &speed, 0, 0, "%.05g"))
                {
                    (*selectedNode)["speed"] = speed;
                    update = true;
                }
                if (selectedNode->contains("sprites"))
                {
                    // Todo: Add support to re-order sprites.
                    // Todo: Preview button and preview at the bottom. When preview button is clicked, it will expand the window even more to fit it if needed.
                    // When hovering over sprite add X button top right to remove the sprite
                    // Todo: Add sprite drag and drop support

                    ImGui::GetWindowDrawList()->AddLine(ImVec2(ImGui::GetWindowPos().x + 10, ImGui::GetWindowPos().y + 100), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 10, ImGui::GetWindowPos().y + 100), IM_COL32(25, 25, 25, 255), 2);

                    ImGui::SetCursorPos({ 50, 105 });
                    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 15, false));
                    ImGui::Text("Sprites");
                    ImGui::PopFont();

                    ImGui::SetCursorPos({ 10, 123 });
                    ImGui::BeginChild("#AnimationSprites", { ImGui::GetWindowWidth() - 10, ImGui::GetWindowHeight() - 133 });

                    static ImVec2 spriteWinPos;

                    float nextY = 10;
                    int index = 0;
                    for (auto& sprite : (*selectedNode)["sprites"])
                    {
                        // Todo: Check if path exists, if it doesn't then try to find the file by filename + extension in other folders. If it can't find it, then remove it or set it to a NULL/"File Not Found" image
                        tempTextures.push_back(new RaylibWrapper::Texture2D(RaylibWrapper::LoadTexture((ProjectManager::projectData.path / "Assets" / sprite.get<std::string>()).string().c_str())));
                        float aspectRatio = (float)tempTextures.back()->width / (float)tempTextures.back()->height;
                        ImVec2 imageSize = ImVec2(0, 0);
                        imageSize.x = aspectRatio > 1.0f ? 90 : 90 * aspectRatio;
                        imageSize.y = aspectRatio > 1.0f ? 90 / aspectRatio : 90;
                        int buttonSize = 90 + ImGui::GetStyle().FramePadding.x * 2;

                        ImGui::SetCursorPos({17, nextY});
                        ImVec2 cursorPos = ImGui::GetCursorPos();

                        ImVec2 tempPos = ImGui::GetCursorScreenPos();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
                        if (ImGui::Button(("##" + std::to_string(index)).c_str(), ImVec2(buttonSize, buttonSize)))
                        {
                            openAddSpriteWin = index;
                            spriteWinPos = tempPos;
                        }
                        ImGui::PopStyleColor();

                        //if (ImGui::IsItemHovered())
                        //{
                        //    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                        //    {

                        //    }
                        //}

                        ImGui::SetCursorPos(ImVec2(cursorPos.x + (buttonSize - imageSize.x) / 2, cursorPos.y + (buttonSize - imageSize.y) / 2)); // Sets the position to the button position and centers it
                        RaylibWrapper::rlImGuiImageSize(tempTextures.back(), imageSize.x, imageSize.y);
                        nextY += 100;
                        index++;
                    }

                    // Todo: Iterate the sprites, create a background of a lighter grey, then add the sprite inside of it (look at content browser for aspect ratio)

                    if (ImGui::GetCursorPosY() > ImGui::GetWindowHeight())
                        ImGui::SetCursorPos({ 15, nextY });
                    else
                        ImGui::SetCursorPos({ 15, ImGui::GetWindowHeight() - 20});
                    ImVec2 tempPos = ImGui::GetCursorScreenPos();
                    if (ImGui::Button("Add Sprite", { 100, 20 }))
                    {
                        openAddSpriteWin = -1;
                        spriteWinPos = tempPos;
                    }

                    ImGui::EndChild();

                    // Todo: Check if the position + height > window and if it is, then move it up
                    // Todo: Consider using a random number as the RenderFileSelector ID so it fixes the issue with the scroll not always being at the top.
                    // Checks if the Sprite File Selector window is open
                    if (openAddSpriteWin != -2)
                    {
                        std::string selectedFile = RenderFileSelector((*selectedNode)["id"] + openAddSpriteWin, "Sprite", "", { ".png", ".jpeg", ".jpg" }, { spriteWinPos.x - 235, spriteWinPos.y - 40});
                        // If NULL was returned, or if None was returned and its adding a new sprite, then close the window with no changes
                        if (selectedFile == "NULL" || (openAddSpriteWin == -1 && selectedFile == "None"))
                            openAddSpriteWin = -2;
                        else if (!selectedFile.empty())
                        {
                            // If its replacing a sprite instead of adding a new one, either remove it, or replace it
                            if (openAddSpriteWin != -1)
                            {
                                if (selectedFile == "None")
                                    (*selectedNode)["sprites"].erase((*selectedNode)["sprites"].begin() + openAddSpriteWin);
                                else
                                    (*selectedNode)["sprites"][openAddSpriteWin] = selectedFile;
                            }
                            else
                                (*selectedNode)["sprites"].push_back(selectedFile);
                            openAddSpriteWin = -2;
                        }
                    }
                }

                ImGui::EndChild();
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor();
            }
        }

        ImNodes::EndNodeEditor();

        // This uses previousSelectNode to make it wait until the next iteration to select the node incase the node isn't created yet (Like when clicking "Create Animation")
        if (selectNode != 0 && previousSelectNode == selectNode) // Todo: It may be good to check if the node actually exists since this waits an iteration and by that time something could happen and delete the node. Maybe make SelectNode() return true or false since it checks if the node exists. if it doesnt exist, set selectNode to 0
        {
            ImNodes::SelectNode(selectNode);
            selectNode = 0;
        }

        if (ImNodes::NumSelectedNodes() > 0)
        {
            std::vector<int> selectedNodes;
            selectedNodes.resize(ImNodes::NumSelectedNodes());
            ImNodes::GetSelectedNodes(selectedNodes.data());
            if (selectedNode == nullptr || (*selectedNode)["id"] != selectedNodes.back())
            {
                openAddSpriteWin = -2;
                for (auto& node : animationGraphData["nodes"])
                {
                    if (node["id"] == selectedNodes.back())
                    {
                        selectedNode = &node;
                        break;
                    }
                }
            }
        }
        else
            selectedNode = nullptr;

        int start_attr, end_attr;
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
        {
            update = true;
            animationGraphData["links"].push_back({ start_attr, end_attr });
        }

        if (update)
        {
            std::ofstream file(animationGraphData["path"].get<std::filesystem::path>());
            if (file.is_open())
            {
                file << std::setw(4) << animationGraphData << std::endl;
                file.close();
                update = false;
            }
        }

        // Todo: When saving, pop up saying the graph file was deleted or moved. Asked to recreate it or delete it
    }
    ImGui::End();
}

void Editor::RenderProjectSettings()
{
    if (!projectSettingsWinOpen) return;
    if (resetProjectSettings)
    {
        float xSize = RaylibWrapper::GetScreenWidth() * 0.50;
        float ySize = RaylibWrapper::GetScreenHeight() * 0.75;
        ImGui::SetNextWindowSize(ImVec2(xSize, ySize));
        ImGui::SetNextWindowPos(ImVec2((RaylibWrapper::GetScreenWidth() - xSize) / 2, (RaylibWrapper::GetScreenHeight() - ySize) / 2));
        resetProjectSettings = false;
    }
    if (ImGui::Begin((ICON_FA_GEARS + std::string(" Project Properties")).c_str(), &projectSettingsWinOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        // General
        ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
        ImGui::Text("General");
        ImGui::PopFont();

        char nameBuffer[256];
        char versionBuffer[256];
        char authorBuffer[256];
        char iconPathBuffer[256];

        strcpy_s(nameBuffer, ProjectManager::projectData.name.c_str());
        strcpy_s(versionBuffer, ProjectManager::projectData.version.c_str());
        strcpy_s(authorBuffer, ProjectManager::projectData.author.c_str());
        strcpy_s(iconPathBuffer, ProjectManager::projectData.iconPath.c_str());

        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
        {
            ProjectManager::projectData.name = nameBuffer;
            ProjectManager::SaveProjectData(ProjectManager::projectData);
        }

        ImGui::Text("Author");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputText("##Author", authorBuffer, sizeof(authorBuffer)))
        {
            ProjectManager::projectData.author = authorBuffer;
            ProjectManager::SaveProjectData(ProjectManager::projectData);
        }

        ImGui::Text("Version");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(50);
        if (ImGui::InputText("##Version", versionBuffer, sizeof(versionBuffer)))
        {
            ProjectManager::projectData.version = versionBuffer;
            ProjectManager::SaveProjectData(ProjectManager::projectData);
        }

        ImGui::Text("Icon Path");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("##IconPath", iconPathBuffer, sizeof(iconPathBuffer))) // Todo: Add drag & drop support and file select dialog
        {
            ProjectManager::projectData.iconPath = iconPathBuffer;
            ProjectManager::SaveProjectData(ProjectManager::projectData);
        }

        ImGui::NewLine();

        // Window Settings
        ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
        ImGui::Text("Window Settings");
        ImGui::PopFont();

        ImGui::Text("Display Mode");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        const char* displayModes[] = { "Borderless", "Fullscreen", "Windowed" };
        if (ImGui::Combo("##DisplayMode", &ProjectManager::projectData.displayMode, displayModes, IM_ARRAYSIZE(displayModes)))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("Resolution");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        int windowResolutionInt[2] = { static_cast<int>(ProjectManager::projectData.windowResolution.x), static_cast<int>(ProjectManager::projectData.windowResolution.y) };
        if (ImGui::InputInt2("##WindowResolution", windowResolutionInt))
            ProjectManager::projectData.windowResolution = { static_cast<float>(windowResolutionInt[0]), static_cast<float>(windowResolutionInt[1]) };

        ImGui::Text("Minimum Resolution");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        int minimumResolutionInt[2] = { static_cast<int>(ProjectManager::projectData.minimumResolution.x), static_cast<int>(ProjectManager::projectData.minimumResolution.y) };
        if (ImGui::InputInt2("##MinimumResolution", minimumResolutionInt))
            ProjectManager::projectData.minimumResolution = { static_cast<float>(minimumResolutionInt[0]), static_cast<float>(minimumResolutionInt[1]) };

        ImGui::Text("Resizable");
        ImGui::SameLine();
        if (ImGui::Checkbox("##ResizableWindow", &ProjectManager::projectData.resizableWindow))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("Max FPS");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(25);
        if (ImGui::InputInt("##MaxFPS", &ProjectManager::projectData.maxFPS, 0, 0))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("Run In Background");
        ImGui::SameLine();
        if (ImGui::Checkbox("##RunInBackground", &ProjectManager::projectData.runInBackground))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("VSync");
        ImGui::SameLine();
        if (ImGui::Checkbox("##VSync", &ProjectManager::projectData.vsync))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::NewLine();

        // Graphics
        ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
        ImGui::Text("Graphics");
        ImGui::PopFont();

        ImGui::Text("Anti-aliasing");
        ImGui::SameLine();
        if (ImGui::Checkbox("##Antialiasing", &ProjectManager::projectData.antialiasing))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("High DPI");
        ImGui::SameLine();
        if (ImGui::Checkbox("##HighDPI", &ProjectManager::projectData.highDPI))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::NewLine();

        // Physics
        ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
        ImGui::Text("Physics");
        ImGui::PopFont();

        ImGui::Text("Timestep");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(70);
        if (ImGui::InputFloat2("##PhysicsTimeStep", &ProjectManager::projectData.physicsTimeStep.x, "%.05g"))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("Velocity Iterations");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(25);
        if (ImGui::InputInt("##VelocityIterations", &ProjectManager::projectData.velocityIterations, 0, 0))
            ProjectManager::SaveProjectData(ProjectManager::projectData);

        ImGui::Text("Position Iterations");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(25);
        if (ImGui::InputInt("##PositionIterations", &ProjectManager::projectData.positionIterations, 0, 0))
            ProjectManager::SaveProjectData(ProjectManager::projectData);
    }
    ImGui::End();
}

void Editor::RenderScriptCreateWin()
{
    if (!scriptCreateWinOpen) return;
    ImGui::SetNextWindowSize(ImVec2(180, 180));
    ImGui::SetNextWindowPos(ImVec2((RaylibWrapper::GetScreenWidth() - 180) / 2, (RaylibWrapper::GetScreenHeight() - 180) / 2));
    if (ImGui::Begin("Create Script", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Script Name");
        static char behaviourName[36] = "";
        ImGui::SetNextItemWidth(163);
        ImGui::InputTextWithHint("##Name", "Script Name...", behaviourName, IM_ARRAYSIZE(behaviourName));
        ImGui::NewLine();
        if (ImGui::Button("Cancel", ImVec2(163, 0)))
        {
            strcpy_s(behaviourName, sizeof(behaviourName), "");
            scriptCreateWinOpen = false;
        }

        ImGui::NewLine();

        std::string name = behaviourName;
        std::string newName = name;
        newName.erase(std::remove_if(newName.begin(), newName.end(), [](unsigned char c) {
            return std::isspace(c);
            }), newName.end());
        bool canCreate = true;
        if (newName == "") canCreate = false;

        if (!canCreate) ImGui::BeginDisabled();
        if (ImGui::Button("Create", ImVec2(163, 0)))
        {
            if (std::filesystem::exists(fileExplorerPath / (name + ".h")) || std::filesystem::exists(fileExplorerPath / (name + ".cpp")))
            {
                // Todo: Popup saying script with this name already exists in this location
                ImGui::End();
                return;
            }
            std::filesystem::path miscPath = std::filesystem::path(__FILE__).parent_path() / "resources" / "misc";
            std::filesystem::copy_file(miscPath / "ScriptPreset.h", fileExplorerPath / (name + ".h"));
            std::filesystem::copy_file(miscPath / "ScriptPreset.cpp", fileExplorerPath / (name + ".cpp"));
            scriptCreateWinOpen = false;
        }
        if (!canCreate) ImGui::EndDisabled();
    }
    ImGui::End();
}

void Editor::RenderComponentsWin()
{
    if (!componentsWindowOpen || !std::holds_alternative<GameObject*>(objectInProperties)) return;

    if (resetComponentsWin)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 400));
        ImGui::SetNextWindowPos(ImVec2((RaylibWrapper::GetScreenWidth() - 300) / 2, (RaylibWrapper::GetScreenHeight() - 400) / 2));
        ImGui::SetNextWindowScroll({0,0});
        resetComponentsWin = false;
    }
    if (ImGui::Begin("Add Component", &componentsWindowOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoCollapse))
    {
        // Internal Components
        float buttonWidth = ImGui::GetWindowWidth() - 28;
        // Todo: Add MeshRenderer
        if (ImGui::Button("Camera", ImVec2(buttonWidth, 0)))
        {
            std::get<GameObject*>(objectInProperties)->AddComponent<CameraComponent>();
            componentsWindowOpen = false;
            resetComponentsWin = true;
        }
        else if (ImGui::Button("Light", ImVec2(buttonWidth, 0)))
        {
            std::get<GameObject*>(objectInProperties)->AddComponent<Lighting>();
            componentsWindowOpen = false;
            resetComponentsWin = true;
        }
        else if (ImGui::Button("Collider2D", ImVec2(buttonWidth, 0)))
        {
            std::get<GameObject*>(objectInProperties)->AddComponent<Collider2D>();
            componentsWindowOpen = false;
            resetComponentsWin = true;
        }
        else if (ImGui::Button("Rigidbody2D", ImVec2(buttonWidth, 0)))
        {
            std::get<GameObject*>(objectInProperties)->AddComponent<Rigidbody2D>();
            componentsWindowOpen = false;
            resetComponentsWin = true;
        }
        else if (ImGui::Button("Animation Player", ImVec2(buttonWidth, 0)))
        {
            std::get<GameObject*>(objectInProperties)->AddComponent<AnimationPlayer>();
            componentsWindowOpen = false;
            resetComponentsWin = true;
        }
        //else if (ImGui::Button("Audio Player", ImVec2(buttonWidth, 0)))
        //{
        //    std::get<GameObject*>(objectInProperties)->AddComponent<AudioPlayer>();
        //    componentsWindowOpen = false;
        //    resetComponentsWin = true;
        //}
        ImGui::Separator();
        // External Components
        for (const auto& file : std::filesystem::recursive_directory_iterator(ProjectManager::projectData.path / "Assets"))
        {
            if (!std::filesystem::is_regular_file(file) || file.path().extension() != ".h") continue;
            if (ImGui::Button(file.path().stem().string().c_str(), ImVec2(buttonWidth, 0)))
            {
                std::filesystem::path path = std::filesystem::relative(file.path(), ProjectManager::projectData.path / "Assets");

                // Todo: First search same folder for .cpp if its not there, then search all sub folders, then all previous folders.
                std::filesystem::path cppPath = path;
                cppPath.replace_extension(".cpp");
                ScriptComponent* scriptComponent = &std::get<GameObject*>(objectInProperties)->AddComponent<ScriptComponent>();
                scriptComponent->SetHeaderPath(path.string());
                scriptComponent->SetCppPath(cppPath);
                scriptComponent->SetName(path.stem().string());
                //scriptComponent->name = file.path().stem().string();

                componentsWindowOpen = false;
                resetComponentsWin = true;
            }
        }
    }
    ImGui::End();
}

void Editor::RenderCameraView()
{
    if (!cameraSelected) return;

    if (resetCameraView)
    {
        cameraRenderTexture = RaylibWrapper::LoadRenderTexture(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight());
        ImGui::SetNextWindowSize(ImVec2(200, 112));
        if (!viewportOpened)
            ImGui::SetNextWindowPos(ImVec2((RaylibWrapper::GetScreenWidth() - 200) / 2, (RaylibWrapper::GetScreenHeight() - 112) / 2));
        else
            ImGui::SetNextWindowPos(ImVec2(viewportPosition.z - 200, viewportPosition.w - 112));
        resetCameraView = false;
    }

    RaylibWrapper::BeginTextureMode(cameraRenderTexture);
    if (ProjectManager::projectData.is3D)
    {
        RaylibWrapper::ClearBackground({ 135, 206, 235, 255 });
    }
    else
        RaylibWrapper::ClearBackground({128, 128, 128, 255});

    selectedObject->GetComponent<CameraComponent>()->raylibCamera.BeginMode3D();

    //if (ProjectManager::projectData.is3D)
    //    DrawGrid(100, 10.0f);

    for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects()) // Todo: This is different from the main editor camera. Check if this needs to be updated.
    {
        if (!gameObject->IsActive()) continue;
        for (Component* component : gameObject->GetComponents())
        {
            if (!component->IsActive() || !component->runInEditor) continue;
            component->Update(RaylibWrapper::GetFrameTime());
        }
    }

    RaylibWrapper::EndMode3D();

    RaylibWrapper::EndTextureMode();

    if (ImGui::Begin("Camera View", &componentsWindowOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
    {
        rlImGuiImageRenderTextureFit(&cameraRenderTexture, true);
    }
    ImGui::End();
}

int Editor::RenderColorPicker(std::string name, ImVec2 position, ImVec4& selectedColor, ImVec4& previousColor) // Todo: Does this really need to be in the Editor.h? Also Add Undo/Redo
{
    // return 0 = do nothing, 1 = update color, 2 = update color and close
    static bool open = true;
    int action = 0;
    ImGui::SetNextWindowSize(ImVec2(250, 250));
    ImGui::SetNextWindowPos(position);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.12f, 0.12f, 0.12f, 1.00f));
    ImGui::Begin(name.c_str(), &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
    if (ImGui::ColorPicker4(("##ColorPicker" + name).c_str(), (float*)&selectedColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoLabel, (float*)&previousColor))
        action = 1;
    if (!open)
    {
        open = true;
        action = 2;
    }
    ImGui::End();
    ImGui::PopStyleColor(3);
    return action;
}

void Editor::RenderProperties()
{
    static bool show = true;

    //if (resetPropertiesWin)
    //{
    //    ImGui::SetNextWindowSize(ImVec2(300, 736));
    //    ImGui::SetNextWindowPos(ImVec2(1643, 52));
    //    resetPropertiesWin = false;
    //}
    ImGuiWindowFlags windowFlags =  ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin((ICON_FA_GEARS + std::string(" Properties")).c_str(), nullptr, windowFlags))
    {
        ImGui::BeginGroup();
        if (std::holds_alternative<GameObject*>(objectInProperties) && std::find_if(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [&](const auto& obj) { return obj == std::get<GameObject*>(objectInProperties); }) != SceneManager::GetActiveScene()->GetGameObjects().end())
        {
            // Active checkbox
            ImGui::Checkbox("##Active" , &std::get<GameObject*>(objectInProperties)->active);
            // Name
            ImGui::SameLine();
            char nameBuffer[256] = {};
            strcpy_s(nameBuffer, std::get<GameObject*>(objectInProperties)->GetName().c_str());
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 45);
            if (ImGui::InputText("##ObjectNameText", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::get<GameObject*>(objectInProperties)->SetName(std::string(nameBuffer));
            }
            // Layers & Tags
            ImGui::NewLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10);
            int width = (ImGui::GetWindowWidth() - 125) / 2;
            ImGui::Text("Layers:");
            ImGui::SameLine();
            static const char* layers[] = { "Default", "UI" };
            static int selectedLayer = 0;
            ImGui::SetNextItemWidth(width);
            if (ImGui::BeginCombo("##Layers", layers[selectedLayer]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(layers); i++)
                {
                    const bool isSelected = (selectedLayer == i);
                    if (ImGui::Selectable(layers[i], isSelected))
                        selectedLayer = i;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            ImGui::Text("Tag:");
            ImGui::SameLine();
            static const char* tags[] = { "None" };
            static int selectedTag = 0;
            ImGui::SetNextItemWidth(width);
            if (ImGui::BeginCombo("##Tags", tags[selectedTag]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(tags); i++)
                {
                    const bool isSelected = (selectedTag == i);
                    if (ImGui::Selectable(tags[i], isSelected))
                        selectedTag = i;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            // Position
            width = (ImGui::GetWindowWidth() - 165) / 3;
            ImGui::NewLine();
            ImGui::Text("Position:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            float xPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().x;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectXPos", &xPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ xPos, std::get<GameObject*>(objectInProperties)->transform.GetPosition().y, std::get<GameObject*>(objectInProperties)->transform.GetPosition().z });
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            float yPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().y;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectYPos", &yPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetPosition().x, yPos, std::get<GameObject*>(objectInProperties)->transform.GetPosition().z });
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            float zPos = std::get<GameObject*>(objectInProperties)->transform.GetPosition().z;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectZPos", &zPos, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetPosition(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetPosition().x, std::get<GameObject*>(objectInProperties)->transform.GetPosition().y, zPos });
            }
            // Scale
            ImGui::NewLine();
            ImGui::Text("Scale:        ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            //float xScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().x / std::get<GameObject*>(objectInProperties)->GetRealSize().x;
            float xScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().x;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectXScale", &xScale, 0, 0, "%.10g")) {
                //std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ xScale * std::get<GameObject*>(objectInProperties)->GetRealSize().x, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ xScale, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            //float yScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().y / std::get<GameObject*>(objectInProperties)->GetRealSize().y;
            float yScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().y;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectYScale", &yScale, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetScale().x, yScale, std::get<GameObject*>(objectInProperties)->transform.GetScale().z });
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            //float zScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().z / std::get<GameObject*>(objectInProperties)->GetRealSize().z;
            float zScale = std::get<GameObject*>(objectInProperties)->transform.GetScale().z;
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputFloat("##ObjectZScale", &zScale, 0, 0, "%.10g")) {
                std::get<GameObject*>(objectInProperties)->transform.SetScale(Vector3{ std::get<GameObject*>(objectInProperties)->transform.GetScale().x, std::get<GameObject*>(objectInProperties)->transform.GetScale().y, zScale });
            }
            // Rotation
            Vector3 rot = QuaternionToEuler(std::get<GameObject*>(objectInProperties)->transform.GetRotation()) * DEG;
            ImGui::NewLine();
            ImGui::Text("Rotation:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            int xRot = static_cast<int>(std::round(rot.x));
            ImGui::SetNextItemWidth(width);
            // Todo: Create the bool "rotation" updated and if its updated on X, Y, Z, set it to true, then below check if its true and if it is, update the rotation. This way it reduces duplicate code. DO the same with Position and Scale
            if (ImGui::InputInt("##ObjectXRotation", &xRot, 0, 0))
                std::get<GameObject*>(objectInProperties)->transform.SetRotationEuler({ (float)xRot, rot.y, rot.z });
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            int yRot = static_cast<int>(std::round(rot.y));
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputInt("##ObjectYRotation", &yRot, 0, 0))
                std::get<GameObject*>(objectInProperties)->transform.SetRotationEuler({ rot.x, (float)yRot, rot.z });
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            int zRot = static_cast<int>(std::round(rot.z));
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputInt("##ObjectZRotation", &zRot, 0, 0))
                std::get<GameObject*>(objectInProperties)->transform.SetRotationEuler({ rot.x, rot.y, (float)zRot });

            ImGui::NewLine();
             
            //Components
            static nlohmann::json* colorPopupOpened;
            static ImVec4 selectedColor;
            static ImVec4 previousColor;
            static ImVec2 popupPosition;
            int componentsNum = 0;
            static Component* componentInContextMenu = nullptr;
            for (Component* component : std::get<GameObject*>(objectInProperties)->GetComponents())
            {
                componentsNum++;
                float buttonWidth = ImGui::GetWindowWidth() - 15;
                //ImGui::Separator();
                int deleteYPos = ImGui::GetCursorPosY();
                std::string name;
                if (typeid(*component) == typeid(ScriptComponent))
                    name = ICON_FA_FILE_CODE + std::string(" ") + dynamic_cast<ScriptComponent*>(component)->GetName();
                else
                    name = component->iconUnicode + " " + component->name;

                ImGui::SetNextItemWidth(buttonWidth);
                bool collapsingHeaderExpanded = false;
                if (ImGui::CollapsingHeader((name + "##" + std::to_string(ImGui::GetCursorPosY())).c_str())) // Todo: Figure out why the width isn't being set on this
                {
                    collapsingHeaderExpanded = true;
                    if (ImGui::IsItemClicked(1))
                    {
                        componentInContextMenu = component;
                        ImGui::OpenPopup("ComponentsContextMenu");
                    }
                    int windowHeight = 1;
                    if (component->exposedVariables != nullptr)
                        windowHeight = component->exposedVariables[1].size() * 33;

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.22f, 0.22f, 0.22f, 1.00f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 5));
                    // Todo: Is creating a child window the best thing to do here?
                    ImGui::BeginChild((std::to_string(ImGui::GetCursorPosY())).c_str(), ImVec2(buttonWidth, windowHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding);
                    if (typeid(*component) == typeid(ScriptComponent) && (component->exposedVariables == nullptr || oneSecondDelay <= 0)) // Todo: Do not check them for updates. Also check if its empty // Todo: Threading
                    {
                        if (component->exposedVariables == nullptr || component->exposedVariables[0].empty()) // Todo: Check every ~1 second if the file is modified and if should get exposed variables again. DONOT change values, only remove removed variables
                            component->exposedVariables = Utilities::GetExposedVariables(ProjectManager::projectData.path / "Assets" / dynamic_cast<ScriptComponent*>(component)->GetHeaderPath()); // Todo: Add Threading
                        else
                        {
                            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(std::filesystem::last_write_time(ProjectManager::projectData.path / "Assets" / dynamic_cast<ScriptComponent*>(component)->GetHeaderPath()) - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
                            std::time_t tt = std::chrono::system_clock::to_time_t(sctp);

                            if (component->exposedVariables[0].get<int>() != tt)
                            {
                                nlohmann::json updatedJson = Utilities::GetExposedVariables(ProjectManager::projectData.path / "Assets" / dynamic_cast<ScriptComponent*>(component)->GetHeaderPath()); // Todo: Add Threading

                                for (auto updatedElement = updatedJson[1].begin(); updatedElement != updatedJson[1].end(); ++updatedElement)
                                {
                                    for (auto oldElement = component->exposedVariables[1].begin(); oldElement != component->exposedVariables[1].end(); ++oldElement)
                                    {
                                        if ((*updatedElement)[0] == (*oldElement)[0] && (*updatedElement)[1] == (*oldElement)[1])
                                        {
                                            (*updatedElement)[2] = (*oldElement)[2];
                                            //(*updatedElement)[4] = (*oldElement)[4];
                                            break;
                                        }
                                    }
                                }

                                component->exposedVariables = updatedJson;
                            }
                        }
                    }
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.18f, 1.00f));
                    if (component->exposedVariables != nullptr) // Todo: Add Threading
                    {
                        // Todo: Add vectors, list, and array
                        //ConsoleLogger::InfoLog(component->exposedVariables.dump(4));
                        for (auto it = component->exposedVariables[1].begin(); it != component->exposedVariables[1].end(); ++it)
                        {
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                            std::string name = (*it)[3];
                            //ImGui::NewLine();
                            ImGui::Text(name.c_str());
                            if ((*it)[0] == "bool")
                            {
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                bool value = (*it)[2].get<bool>();
                                if (ImGui::Checkbox(("##" + name).c_str(), &value))
                                    (*it)[2] = value;
                                //std::string value = (*it)[5];
                                //ImGui::SetNextItemWidth(60);
                                //if (ImGui::BeginCombo(("##" + name).c_str(), value.c_str()))
                                //{
                                //    if (ImGui::Selectable("True", value == "true"))
                                //    {
                                //        (*it)[2] = "true";
                                //        (*it)[4] = "True";
                                //    }
                                //    if (ImGui::Selectable("False", value == "false"))
                                //    {
                                //        (*it)[2] = "false";
                                //        (*it)[4] = "False";
                                //    }
                                //    ImGui::EndCombo();
                                //}
                            }
                            else if ((*it)[0] == "string" || (*it)[0] == "char")
                            {
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::SetNextItemWidth(120);
                                std::string inputText = (*it)[2].get<std::string>();
                                char buffer[256];
                                strcpy_s(buffer, sizeof(buffer), inputText.c_str());
                                ImGui::InputText(("##" + name).c_str(), buffer, sizeof(buffer));
                                (*it)[2] = buffer;
                            }
                            else if ((*it)[0] == "float")
                            {
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::SetNextItemWidth(60); 
                                float value = (*it)[2].get<float>();
                                ImGui::InputFloat(("##" + name).c_str(), &value, 0, 0, "%.10g");
                                (*it)[2] = value;
                            }
                            else if ((*it)[0] == "int")
                            {
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::SetNextItemWidth(60);
                                int value = (*it)[2].get<int>();
                                ImGui::InputInt(("##" + name).c_str(), &value, 0, 0);
                                (*it)[2] = value;
                            }
                            else if ((*it)[0] == "Color")
                            {
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                // Todo: Add black outline
                                if (ImGui::ColorButton(("##" + name).c_str(), ImVec4((*it)[2][0], (*it)[2][1], (*it)[2][2], (*it)[2][3]), 0, ImVec2(20, 20)))
                                {
                                    if (colorPopupOpened != nullptr && colorPopupOpened[3] == name)
                                        colorPopupOpened = nullptr;
                                    else
                                    {
                                        colorPopupOpened = &(*it);
                                        previousColor = ImVec4((*it)[2][0] / 255, (*it)[2][1] / 255, (*it)[2][2] / 255, (*it)[2][3] / 255);
                                        selectedColor = previousColor;
                                    }
                                }
                                if (colorPopupOpened != nullptr && (*colorPopupOpened)[3].get<std::string>() == name)
                                    popupPosition = ImVec2(ImGui::GetWindowPos().x - 250, ImGui::GetCursorPosY() + ImGui::GetWindowPos().y - 20);
                            }
                            else if ((*it)[0] == "Vector2")
                            {
                                width = (ImGui::GetWindowWidth() - (65 + ImGui::CalcTextSize(name.c_str()).x)) / 2;
                                //ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::SameLine();
                                ImGui::Text("X");
                                ImGui::SameLine();
                                float x = (*it)[2][0].get<float>();
                                ImGui::SetNextItemWidth(width);
                                ImGui::InputFloat(("##X" + name).c_str(), &x, 0, 0, "%.10g");
                                (*it)[2][0] = x;
                                ImGui::SameLine();
                                ImGui::Text("Y");
                                ImGui::SameLine();
                                float y = (*it)[2][1].get<float>();
                                ImGui::SetNextItemWidth(width);
                                ImGui::InputFloat(("##Y" + name).c_str(), &y, 0, 0, "%.10g");
                                (*it)[2][1] = y;
                            }
                            else if ((*it)[0] == "Vector3")
                            {
                                width = (ImGui::GetWindowWidth() - (65 + ImGui::CalcTextSize(name.c_str()).x)) / 2;
                                //ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::SameLine();
                                ImGui::Text("X");
                                ImGui::SameLine();
                                float x = (*it)[2][0].get<float>();
                                ImGui::SetNextItemWidth(width);
                                ImGui::InputFloat(("##X" + name).c_str(), &x, 0, 0, "%.10g");
                                (*it)[2][0] = x;
                                ImGui::SameLine();
                                ImGui::Text("Y");
                                ImGui::SameLine();
                                float y = (*it)[2][1].get<float>();
                                ImGui::SetNextItemWidth(width);
                                ImGui::InputFloat(("##Y" + name).c_str(), &y, 0, 0, "%.10g");
                                (*it)[2][1] = x;
                                ImGui::SameLine();
                                ImGui::Text("Z");
                                ImGui::SameLine();
                                float z = (*it)[2][2].get<float>();
                                ImGui::SetNextItemWidth(width);
                                ImGui::InputFloat(("##Z" + name).c_str(), &z, 0, 0, "%.10g");
                                (*it)[2][2] = x;
                            }
                            else if ((*it).size() > 4 && (*it)[4].contains("Extensions"))
                            {
                                static bool openSelector = false;
                                // Todo: If the user deselects or closes the component, it will hide the menu, but once it is reopened, or another one with Extensions, it will open the menu. Maybe right here right after the bool, run RenderFIleSelector() with a special parameter to check if the old ID != new ID and if it doesnt equal, then return "NULL" and it will know it shouldn't be open
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.17f, 0.18f, 1.00f));
                                std::string selectedFile = "None Selected";
                                if (!(*it)[2].is_null() && (*it)[2].get<std::string>() != "nullptr")
                                    selectedFile = std::filesystem::path((*it)[2].get<std::string>()).stem().string();
                                if (ImGui::Button(selectedFile.c_str(), { ImGui::GetWindowWidth() - 130, 20 }))
                                    openSelector = true;

                                ImGui::PopStyleColor();

                                if (openSelector)
                                {
                                    // Adds the extensions to the extensions vector so it can be passed into RenderFileSelector()
                                    std::vector<std::string> extensions;
                                    for (const auto& extension : (*it)[4]["Extensions"])
                                        extensions.push_back(extension);

                                    std::string currentlySelected = ((*it)[2] == "nullptr") ? "" : (*it)[2];
                                    std::string selectedFile = RenderFileSelector(componentsNum, name, currentlySelected, extensions, {ImGui::GetCursorScreenPos().x - 100, ImGui::GetCursorScreenPos().y - 40});
                                    if (selectedFile == "NULL")
                                        openSelector = false;
                                    else if (!selectedFile.empty())
                                    {
                                        (*it)[2] = selectedFile;
                                        openSelector = false;
                                    }
                                }
                            }
                            else if ((*it).size() > 4) // This is probably not a good solution for checking if its an enum or not. Maybe give the array a key
                            {
                                // Todo: The enum values won't be capitalized or have spaces, only the selected enum value will be capitalized with spaces
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                std::string value = (*it)[2].get<std::string>();
                                ImGui::SetNextItemWidth(ImGui::CalcTextSize(value.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f + 20);
                                if (ImGui::BeginCombo(("##" + name).c_str(), value.c_str())) // Todo: Resize Combo Box to fit selected value.
                                {
                                    for (const std::string& enumValue : (*it)[4])
                                    {
                                        if (ImGui::Selectable(enumValue.c_str(), value == enumValue))
                                            (*it)[2] = enumValue;
                                    }
                                    ImGui::EndCombo();
                                }
                            }
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                            //ConsoleLogger::ErrorLog("Found Exposed Variable: " + it->dump());
                        }
                    }
                    ImGui::PopStyleColor();
                    ImGui::EndChild();
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor();
                }
                bool canOpenMenu = false;
                if (!collapsingHeaderExpanded && ImGui::IsItemClicked(1))
                {
                    componentInContextMenu = component;
                    ImGui::OpenPopup("ComponentsContextMenu");
                }
                int oldYPos = ImGui::GetCursorPosY();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 50, deleteYPos));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.17f, 0.18f, 1.00f));
                ImGui::Checkbox("##ComponentActive", &component->active); // Todo: This isn't detecting clicks
                ImGui::PopStyleColor();
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 25, deleteYPos));
                if (ImGui::Button(("X##" + std::to_string(componentsNum)).c_str())) // Todo: This isn't detecting clicks
                {
                    std::get<GameObject*>(objectInProperties)->RemoveComponent(component);
                }
                ImGui::PopStyleColor(2);
                ImGui::SetCursorPosY(oldYPos);
            }

            ImGui::NewLine();
            ImGui::SetCursorPos(ImVec2(25, ImGui::GetCursorPosY() - 15));
            if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth() - 50, 0)))
                componentsWindowOpen = true;

            if (colorPopupOpened != nullptr)
            {
                int action = RenderColorPicker("Tint", popupPosition, selectedColor, previousColor);
                if (action != 0)
                {
                    (*colorPopupOpened)[2][0] = selectedColor.x * 255;
                    (*colorPopupOpened)[2][1] = selectedColor.y * 255;
                    (*colorPopupOpened)[2][2] = selectedColor.z * 255;
                    (*colorPopupOpened)[2][3] = selectedColor.w * 255;
                    if (action == 2)
                        colorPopupOpened = nullptr;
                }
            }

            if (ImGui::BeginPopupContextItem("ComponentsContextMenu"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    std::get<GameObject*>(objectInProperties)->RemoveComponent(componentInContextMenu);
                    componentInContextMenu = nullptr;
                }

                ImGui::EndPopup();
            }
        }
        // Data File
        else if (std::holds_alternative<DataFile>(objectInProperties))
        {
            DataFile& dataFile = std::get<DataFile>(objectInProperties);
            if (!std::filesystem::exists(dataFile.path))
            {
                objectInProperties = std::monostate{};
                ImGui::EndGroup();
                ImGui::End();
                return;
            }

            int offset = 0;
            switch (dataFile.type)
            {
            case DataFileTypes::Sound:
                ImGui::SetCursorPos({ 10, 30 });
                RaylibWrapper::rlImGuiImageSizeV(IconManager::imageTextures["SoundIcon"], { 25, 25 });
                offset = 35;
                break;
            }
            std::string fileName = std::filesystem::path(std::get<DataFile>(objectInProperties).path).stem().string();
            ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 20, false));
            ImGui::SetCursorPos({ (ImGui::GetWindowWidth() - offset - ImGui::CalcTextSize(fileName.c_str()).x) / 2 + offset, 30});
            ImGui::Text(fileName.c_str());
            ImGui::PopFont();

            ImGui::SetCursorPosY(60);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.5f);

            ImGui::SetCursorPos({15, 70});
            bool updateJson = false;
            for (auto& [key, value] : std::get<DataFile>(objectInProperties).json["public"].items())
            {
                // Adds spaces before all capital letters, and makes the first letter a capital
                std::string newKey;
                for (char c : key) {
                    if (std::isupper(c)) {
                        newKey += ' ';
                    }
                    newKey += c;
                }
                newKey[0] = std::toupper(newKey[0]);

                if (value.is_boolean())
                {
                    ImGui::Text(newKey.c_str());
                    ImGui::SameLine();
                    bool tempValue = value.get<bool>();
                    if (ImGui::Checkbox(("##" + key).c_str(), &tempValue))
                    {
                        value = tempValue;
                        updateJson = true;
                    }
                }
                else if (value.is_string())
                {
                    ImGui::Text(newKey.c_str());
                    ImGui::SameLine();

                    int inputWidth = ImGui::CalcTextSize(value.get<std::string>().c_str()).x + ImGui::CalcTextSize("a").x + 10;
                    if (inputWidth < 130)
                        inputWidth = 130;
                    else if (inputWidth > (ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45))
                        inputWidth = ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45;

                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::SetCursorPos({ ImGui::CalcTextSize(newKey.c_str()).x + 30, ImGui::GetCursorPosY() - 3 });
                    std::string inputText = value.get<std::string>();
                    char buffer[256];
                    strcpy_s(buffer, sizeof(buffer), inputText.c_str());
                    if (ImGui::InputText(("##" + key).c_str(), buffer, sizeof(buffer)))
                    {
                        value = buffer;
                        updateJson = true;
                    }

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                }
                else if (value.is_number_integer())
                {
                    ImGui::Text(newKey.c_str());
                    ImGui::SameLine();

                    int inputWidth = ImGui::CalcTextSize(std::to_string(value.get<int>()).c_str()).x + ImGui::CalcTextSize("1").x + 10;
                    if (inputWidth < 60)
                        inputWidth = 60;
                    else if (inputWidth > (ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45))
                        inputWidth = ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45;

                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::SetCursorPos({ ImGui::CalcTextSize(newKey.c_str()).x + 30, ImGui::GetCursorPosY() - 3 });
                    int input = value.get<int>();
                    if (ImGui::InputInt(("##" + key).c_str(), &input, 0, 0))
                    {
                        if (input > INT_MAX)
                            input = INT_MAX;
                        else if (input < -INT_MIN)
                            input = INT_MIN;
                        value = input;
                        updateJson = true;
                    }

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                }
                else if (value.is_number_float())
                {
                    ImGui::Text(newKey.c_str());
                    ImGui::SameLine();

                    // Todo: Doesn't work the best when there are a lot of decimals
                    std::string stringValue = std::to_string(value.get<float>());
                    stringValue.erase(stringValue.find_last_not_of('0') + 1, std::string::npos); // Removes trailing 0's

                    int inputWidth = ImGui::CalcTextSize(stringValue.c_str()).x + ImGui::CalcTextSize("1").x + 10;
                    if (inputWidth < 60)
                        inputWidth = 60;
                    else if (inputWidth > (ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45))
                        inputWidth = ImGui::GetWindowWidth() - ImGui::CalcTextSize(newKey.c_str()).x - 45;

                    ImGui::SetNextItemWidth(inputWidth);
                    ImGui::SetCursorPos({ ImGui::CalcTextSize(newKey.c_str()).x + 30, ImGui::GetCursorPosY() - 3 });
                    float input = value.get<float>();
                    if (ImGui::InputFloat(("##" + key).c_str(), &input, 0, 0, "%.10g"))
                    {
                        value = input;
                        updateJson = true;
                    }

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
                }
                else
                    continue;

                ImGui::SetCursorPos({ 15, ImGui::GetCursorPosY() + 15 });
            }

            if (updateJson)
            {
                try
               {
                    std::ofstream file(dataFile.path);
                    file << dataFile.json.dump(4);
                    file.close();
                }
                catch (const std::exception& e)
                {
                    if (ConsoleLogger::showDebugMessages)
                        ConsoleLogger::WarningLog("There was an error saving your changes to " + dataFile.path.substr(0, dataFile.path.size() - 5) + ".", false);
                    else
                        ConsoleLogger::WarningLog("There was an error saving your changes to " + dataFile.path + ". Error: " + e.what(), true);
                }
            }
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}

void Editor::RenderConsole()
{
    if (ImGui::Begin((ICON_FA_CODE + std::string(" Console")).c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text(ConsoleLogger::consoleText.c_str());
    }
    ImGui::End();
}

bool Editor::RenderHierarchyNode(GameObject* gameObject, bool normalColor)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (normalColor)
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(36, 40, 43, 255)); // 53, 53, 53
    else
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(45, 48, 51, 255)); // 62, 62, 62

    //ImGui::PushStyleColor(ImGuiCol_Header, {64, 64, 64, 255});

    ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selectedObject != nullptr && selectedObject->GetId() == gameObject->GetId())
        flags |= ImGuiTreeNodeFlags_Selected;

    if (gameObject->GetChildren().size() <= 0)
        flags |= ImGuiTreeNodeFlags_Leaf;
    else flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    //std::string icon = ICON_FA_CUBE;
    //if (gameObject->GetComponent<CameraComponent>()) // Todo: Not sure if this is a good idea to check if it has teh Camera component every frame.
    //    icon = ICON_FA_CAMERA;

    if (ImGui::TreeNodeEx((ICON_FA_CUBE + std::string(" ") + gameObject->GetName() + "##" + std::to_string(gameObject->GetId())).c_str(), flags))
    {
        if (!ImGui::IsItemToggledOpen())
        {
            if (ImGui::IsItemClicked())
            {
                hierarchyObjectClicked = true;
                //if (selectedObject != nullptr)
                    //EventSystem::Invoke("ObjectDeselected", selectedObject);
                objectInProperties = gameObject;
                selectedObject = gameObject;
                //EventSystem::Invoke("ObjectSelected", selectedObject);

                if (selectedObject->GetComponent<CameraComponent>() != nullptr)
                    cameraSelected = true;
                else if (cameraSelected)
                {
                    cameraSelected = false;
                    resetCameraView = true;
                }
            }
            else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                objectInHierarchyContextMenu = gameObject;


            for (GameObject* child : gameObject->GetChildren())
                normalColor = RenderHierarchyNode(child, normalColor);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                // Todo: If 3D, rotate the camera and set proper distance depending on the mesh's size
                // Todo: For 2D and if its a sprite, set proper distance so entire sprite fits on screen
                // Todo: Lerp camera position to target
                camera.position = { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, camera.position.z };
                camera.target.x = camera.position.x;
                camera.target.y = camera.position.y;
            }
        }

        ImGui::TreePop();
    }
    // For some reason if the game object has children and its not expanded, the if statment above doesn't return true.
    else if (gameObject->GetChildren().size() > 0)
    {
        if (ImGui::IsItemClicked())
        {
            hierarchyObjectClicked = true;
            //if (selectedObject != nullptr)
                //EventSystem::Invoke("ObjectDeselected", selectedObject);
            objectInProperties = gameObject;
            selectedObject = gameObject;
            //EventSystem::Invoke("ObjectSelected", selectedObject);

            if (selectedObject->GetComponent<CameraComponent>() != nullptr)
                cameraSelected = true;
            else if (cameraSelected)
            {
                cameraSelected = false;
                resetCameraView = true;
            }
        }
        else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            objectInHierarchyContextMenu = gameObject;

        // Unlike above, I am not rendering children since the node must be closed for this code to run

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            // Todo: If 3D, rotate the camera and set proper distance depending on the mesh's size
            // Todo: For 2D and if its a sprite, set proper distance so entire sprite fits on screen
            // Todo: Lerp camera position to target
            camera.position = { gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, camera.position.z };
            camera.target.x = camera.position.x;
            camera.target.y = camera.position.y;
        }
    }

    return normalColor; // Todo: If has child and child is open, return the lowest color rather than this node's color
}

void Editor::RenderHierarchy()
{
    //if (resetHierarchy)
    //{
    //    ImGui::SetNextWindowSize(ImVec2(277, 736));
    //    ImGui::SetNextWindowPos(ImVec2(0, 52));
    //    resetHierarchy = false;
    //}
    //ImGui::SetNextWindowDockID(ImGui::GetID("DockSpace"), ImGuiCond_Once | ImGuiDir_Right);
    //ImGui::DockBuilderDockWindow();
    hierarchyObjectClicked = false;
    ImGui::Begin((ICON_FA_SITEMAP + std::string(" Hierarchy")).c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTable("HierarchyTable", 1))
    {
        bool hierarchyRowColor = true;

        for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
            if (gameObject->GetParent() == nullptr)
                hierarchyRowColor = !RenderHierarchyNode(gameObject, hierarchyRowColor);

        hierarchyRowColor = false;
        ImGui::EndTable();
    }

    if (hierarchyContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
    {
        hierarchyContextMenuOpen = true;
        if (ImGui::BeginPopupContextWindow("HierarchyContextMenu"))
        {
            ImGui::Separator();

            std::string objectToCreate = "";

            if (ProjectManager::projectData.is3D)
            {
                if (ImGui::MenuItem("Create Empty"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Empty";
                }

                if (ImGui::MenuItem("Create Cube"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Cube";
                }

                if (ImGui::MenuItem("Create Cylinder"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Cylinder";
                }

                if (ImGui::MenuItem("Create Sphere"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Sphere";
                }

                if (ImGui::MenuItem("Create Plane"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Plane";
                }

                if (ImGui::MenuItem("Create Cone"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Cone";
                }

                if (ImGui::MenuItem("Light"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Light";
                }
            }
            else
            {
                if (ImGui::MenuItem("Create Square"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Square";
                }
                if (ImGui::MenuItem("Create Circle"))
                {
                    hierarchyContextMenuOpen = false;
                    objectToCreate = "Circle";
                }
            }

            if (ImGui::MenuItem("Create Camera"))
            {
                hierarchyContextMenuOpen = false;
                objectToCreate = "Camera";
            }

            if (objectInHierarchyContextMenu != nullptr && ImGui::MenuItem("Delete"))
            {
                hierarchyObjectClicked = true;
                hierarchyContextMenuOpen = false;

                if (std::holds_alternative<GameObject*>(objectInProperties) && std::get<GameObject*>(objectInProperties)->GetId() == objectInHierarchyContextMenu->GetId())
                    objectInProperties = std::monostate{};
                if (selectedObject != nullptr && objectInHierarchyContextMenu->GetId() == selectedObject->GetId())
                {
                    //EventSystem::Invoke("ObjectDeselected", nullptr);
                    selectedObject = nullptr;
                }

                SceneManager::GetActiveScene()->RemoveGameObject(objectInHierarchyContextMenu);
                objectInHierarchyContextMenu = nullptr;
            }

            if (objectToCreate != "")
            {
                hierarchyObjectClicked = true;

                GameObject* gameObject = SceneManager::GetActiveScene()->AddGameObject();
                gameObject->transform.SetPosition({ 0,0,0 });
                gameObject->transform.SetScale({ 1,1,1 });
                gameObject->transform.SetRotation(Quaternion::Identity());
                gameObject->SetName(objectToCreate);
                if (objectToCreate == "Empty")
                    gameObject->SetName("GameObject");
                else if (objectToCreate == "Camera")
                {
                    gameObject->AddComponent<CameraComponent>();
                    if (!ProjectManager::projectData.is3D)
                    {
                        gameObject->transform.SetPosition({ 0,0,0 });
                        gameObject->transform.SetRotationEuler({ 180, 0, 0 });
                    }
                }
                else if (objectToCreate == "Light")
                    gameObject->AddComponent<Lighting>();
                else
                {
                    if (ProjectManager::projectData.is3D)
                    {
                        MeshRenderer& meshRenderer = gameObject->AddComponent<MeshRenderer>();
                        meshRenderer.SetModelPath(objectToCreate);

                        if (objectToCreate == "Cube")
                            meshRenderer.SetModel(Cube, "Cube", LitStandard);
                        else if (objectToCreate == "Plane")
                            meshRenderer.SetModel(Plane, "Plane", LitStandard);
                        else if (objectToCreate == "Sphere")
                            meshRenderer.SetModel(Sphere, "Sphere", LitStandard);
                        else if (objectToCreate == "Cylinder")
                            meshRenderer.SetModel(Cylinder, "Cylinder", LitStandard);
                        else if (objectToCreate == "Cone")
                            meshRenderer.SetModel(Cone, "Cone", LitStandard);
                    }
                    else
                    {
                        SpriteRenderer& spriteRenderer = gameObject->AddComponent<SpriteRenderer>();
                        spriteRenderer.SetTexture(objectToCreate);

                        //gameObject->AddComponent<Collider2D>(); // Todo: Set size and type
                    }

                }

                //SceneManager::GetActiveScene()->AddGameObject(gameObject);

                for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back()->GetComponents())
                    component->gameObject = SceneManager::GetActiveScene()->GetGameObjects().back();

                if (objectInHierarchyContextMenu != nullptr)
                    SceneManager::GetActiveScene()->GetGameObjects().back()->SetParent(objectInHierarchyContextMenu);

                //if (selectedObject != nullptr)
                    //EventSystem::Invoke("ObjectDeselected", selectedObject);
                selectedObject = SceneManager::GetActiveScene()->GetGameObjects().back();
                objectInProperties = SceneManager::GetActiveScene()->GetGameObjects().back();
                //EventSystem::Invoke("ObjectSelected", selectedObject);

                if (selectedObject->GetComponent<CameraComponent>() != nullptr) // Todo: Should this if-else-if statement be moved up into the "else if (objectToCreate == "Camera")" statement?
                    cameraSelected = true;
                else if (cameraSelected)
                {
                    cameraSelected = false;
                    resetCameraView = true;
                }
            }

            ImGui::EndPopup();
        }
        if (hierarchyContextMenuOpen && !ImGui::IsPopupOpen("HierarchyContextMenu"))
        {
            objectInHierarchyContextMenu = nullptr;
            hierarchyContextMenuOpen = false;
        }
    }
    else if (!hierarchyObjectClicked && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
    {
        if (selectedObject)
        {
            //EventSystem::Invoke("ObjectDeselected", selectedObject);
            selectedObject = nullptr;
            cameraSelected = false;
            resetCameraView = true;
        }
        if (std::holds_alternative<GameObject*>(objectInProperties))
            objectInProperties = std::monostate{};
    }
    ImGui::End();
}

//void Editor::RenderHierarchy()
//{
//    if (resetHierarchy)
//    {
//        ImGui::SetNextWindowSize(ImVec2(277, 736));
//        ImGui::SetNextWindowPos(ImVec2(0, 52));
//        resetHierarchy = false;
//    }
//    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;
//    if (ImGui::Begin("Hierachy", nullptr, windowFlags))
//    {
//        for (int i = 0; i < SceneManager::GetActiveScene()->GetGameObjects().size(); i++) // Todo: I am doing this both in ShowProperties and ShowViewport, maybe merge them to use the same loop?
//        {
//            std::string objectName = SceneManager::GetActiveScene()->GetGameObjects()[i].GetName();
//            if (objectName.length() > round(ImGui::GetWindowWidth() / 11)) {
//                objectName = objectName.substr(0, round(ImGui::GetWindowWidth() / 11));
//                objectName = objectName + "...";
//            }
//            if (ImGui::Button((objectName + "##" + std::to_string(SceneManager::GetActiveScene()->GetGameObjects()[i].GetId())).c_str(), ImVec2((ImGui::GetWindowWidth() - 15), 0)))
//            {
//                objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects()[i];
//                selectedObject = &SceneManager::GetActiveScene()->GetGameObjects()[i];
//
//                if (selectedObject->GetComponent<CameraComponent>() != nullptr)
//                    cameraSelected = true;
//                else if (cameraSelected)
//                {
//                    cameraSelected = false;
//                    resetCameraView = true;
//                }
//            }
//            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
//        }
//        // Add a scrollable text box
//        //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
//        //ImGui::Text("Scrollable Text");
//        //ImGui::EndChild();
//
//        if (hierarchyContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
//        {
//            hierarchyContextMenuOpen = true;
//            if (ImGui::BeginPopupContextWindow())
//            {
//                ImGui::Separator();
//
//                std::string objectToCreate = "";
//
//                if (ImGui::MenuItem("Create Empty"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Empty";
//                }
//
//                if (ImGui::MenuItem("Create Cube"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Cube";
//                }
//
//                if (ImGui::MenuItem("Create Cylinder"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Cylinder";
//                }
//
//                if (ImGui::MenuItem("Create Sphere"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Sphere";
//                }
//
//                if (ImGui::MenuItem("Create Plane"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Plane";
//                }
//
//                if (ImGui::MenuItem("Create Cone"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Cone";
//                }
//
//                if (ImGui::MenuItem("Create Camera"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Camera";
//                }
//
//                if (ImGui::MenuItem("Light"))
//                {
//                    hierarchyContextMenuOpen = false;
//                    objectToCreate = "Light";
//                }
//
//                if (objectToCreate != "")
//                {
//                    GameObject gameObject;
//                    gameObject->transform.SetPosition({ 0,0,0 });
//                    gameObject->transform.SetScale({ 1,1,1 });
//                    gameObject->transform.SetRotation(QuaternionIdentity());
//                    gameObject->SetName(objectToCreate);
//                    if (objectToCreate == "Empty")
//                        gameObject->SetName("GameObject");
//                    else if (objectToCreate == "Camera")
//                        gameObject->AddComponent<CameraComponent>();
//                    else if (objectToCreate == "Light")
//                        gameObject->AddComponent<Lighting>();
//                    else
//                    {
//                        MeshRenderer& meshRenderer = gameObject->AddComponent<MeshRenderer>();
//                        meshRenderer.SetModelPath(objectToCreate);
//
//                        if (objectToCreate == "Cube")
//                            meshRenderer.SetModel(LoadModelFromMesh(GenMeshCube(1, 1, 1)));
//                        else if (objectToCreate == "Plane")
//                            meshRenderer.SetModel(LoadModelFromMesh(GenMeshPlane(1, 1, 1, 1)));
//                        else if (objectToCreate == "Sphere")
//                            meshRenderer.SetModel(LoadModelFromMesh(GenMeshSphere(1, 1, 1)));
//                        else if (objectToCreate == "Cylinder")
//                            meshRenderer.SetModel(LoadModelFromMesh(GenMeshCylinder(1, 1, 1)));
//                        else if (objectToCreate == "Cone")
//                            meshRenderer.SetModel(LoadModelFromMesh(GenMeshCone(1, 1, 1)));
//                    }
//
//                    SceneManager::GetActiveScene()->AddGameObject(gameObject);
//
//                    for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back().GetComponents())
//                    {
//                        component->gameObject = &SceneManager::GetActiveScene()->GetGameObjects().back();
//                    }
//
//                    selectedObject = &SceneManager::GetActiveScene()->GetGameObjects().back();
//                    objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects().back();
//
//                    if (selectedObject->GetComponent<CameraComponent>() != nullptr)
//                        cameraSelected = true;
//                    else if (cameraSelected)
//                    {
//                        cameraSelected = false;
//                        resetCameraView = true;
//                    }
//                }
//
//                ImGui::EndPopup();
//            }
//        }
//    }
//    ImGui::End();
//}

void Editor::RenderTopbar()
{
    int currentWidth = RaylibWrapper::GetScreenWidth();
    int currentHeight = RaylibWrapper::GetScreenHeight();
    ImGui::SetNextWindowSize(ImVec2(currentWidth, 15));
    ImGui::SetNextWindowPos(ImVec2(0, 30));
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    if (ImGui::Begin("##TopBar", nullptr, windowFlags))
    {
        // Play Button
        ImTextureID playIconTexture;
        //if (playing) playIconTexture = activePlayIcon;
        //else playIconTexture = playIcon;

        ImGui::SetCursorPos(ImVec2(currentWidth / 2 - 20, 2));

        if (RaylibWrapper::rlImGuiImageButtonSize("##PlayButton", IconManager::imageTextures["PlayIcon"], ImVec2(20, 20)))
        {
            //startStopPlaying = true;
            //paused = false;
        }

        // Pause Button
        ImTextureID pauseIconTexture;
        //if (paused) pauseIconTexture = activePauseIcon;
        //else if (playing) pauseIconTexture = pauseIcon;
        //else pauseIconTexture = grayedPauseIcon;

        ImGui::SetCursorPos(ImVec2(currentWidth / 2 + 20, 2));

        if (RaylibWrapper::rlImGuiImageButtonSize("##PauseButton", IconManager::imageTextures["GrayedPauseIcon"], ImVec2(20, 20)))
        {
            //if (playing) paused = !paused;
        }

    }
    ImGui::End();
}

void Editor::Render(void)
{
    // Todo: Put this in RenderDockSpace()
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowPos(ImVec2(0, 31));
    ImGui::SetNextWindowSize(ImVec2(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight() - 31));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    dockspaceID = ImGui::GetID("DockSpace"); // Todo: Do this in the init so it only sets the ID once
    if (ImGui::DockBuilderGetNode(dockspaceID) == NULL)
    {
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton);
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

        ImGuiID dock_main_id = dockspaceID;
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, NULL, &dock_main_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);

        ImGui::DockBuilderDockWindow((ICON_FA_SITEMAP + std::string(" Hierarchy")).c_str(), dock_id_left);
        ImGui::DockBuilderDockWindow((ICON_FA_CUBES + std::string(" Viewport")).c_str(), dock_main_id);
        ImGui::DockBuilderDockWindow((ICON_FA_PERSON_RUNNING + std::string(" Animation Graph")).c_str(), dock_main_id);
        ImGui::DockBuilderDockWindow((ICON_FA_GEARS + std::string(" Properties")).c_str(), dock_id_right);
        ImGui::DockBuilderDockWindow((ICON_FA_FOLDER_OPEN + std::string(" Content Browser")).c_str(), dock_id_bottom);
        ImGui::DockBuilderDockWindow((ICON_FA_CODE + std::string(" Console")).c_str(), dock_id_bottom);
        ImGui::DockBuilderFinish(dockspaceID);
    }
    ImGui::DockSpace(dockspaceID);
    ImGui::End();
    ImGui::PopStyleVar();

    // Todo: Docksapce and EditorWindow should  be the same

    ImGui::SetNextWindowPos(ImVec2(0, 30));
    ImGui::SetNextWindowSize(ImVec2(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight() - 30));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.14f, 0.14f, 0.14f, 1.00f));
    ImGui::Begin("##EditorWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::End();
    ImGui::PopStyleColor();

    RenderViewport();
    RenderHierarchy();
    RenderProperties();
    RenderFileExplorer();
    RenderConsole();
    RenderCameraView();
    RenderComponentsWin();
    RenderScriptCreateWin();
    RenderAnimationGraph();
    RenderProjectSettings();
    //RenderTopbar();


    // Todo: Move this to RenderTitleBar()
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(190, 30));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("##MenuBar", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::BeginMenuBar())
    {
        ImGui::SetCursorPos(ImVec2(5, 0));
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Project", "Ctrl+S"))
            {
                SceneManager::SaveScene(SceneManager::GetActiveScene());
                ImGuiToast toast(ImGuiToastType::Success, 1500, true);
                toast.setTitle("Project Saved", "");
                toast.setContent("The project has successfully saved.");
                ImGui::InsertNotification(toast);
            }
            if (ImGui::MenuItem("Play", "Ctrl+P"))
            {
                // Todo: Check if changes have been made and it needs to be recompiled
                if (ProjectManager::BuildToWindows(ProjectManager::projectData, true))
                    // Todo: This crashes the game instantly and needs to be fixed
                    Utilities::OpenPathInExplorer(ProjectManager::projectData.path / "Internal" / "Builds" / "Debug" / std::string(ProjectManager::projectData.name + ".exe"));

                    //std::system(("\"" + (ProjectManager::projectData.path / "Internal" / "Builds" / "Debug" / std::string(ProjectManager::projectData.name + ".exe")).string() + "\"").c_str());
            }
            if (ImGui::BeginMenu("Build Project"))
            {
                if (ImGui::MenuItem("Windows", ""))
                {
                    if (ProjectManager::BuildToWindows(ProjectManager::projectData, false))
                    {
                        ImGuiToast toast(ImGuiToastType::Success, 2500, true);
                        toast.setTitle("Successfully built", "");
                        toast.setContent("The project has successfully built.");
                        ImGui::InsertNotification(toast);
                    }
                    else
                    {
                        ImGuiToast toast(ImGuiToastType::Error, 2500, true);
                        toast.setTitle("Build failed", "");
                        toast.setContent("The project failed to build.");
                        ImGui::InsertNotification(toast);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(43, 0));
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Project Settings", ""))
            {
                if (projectSettingsWinOpen)
                {
                    ImGuiWindow* window = ImGui::FindWindowByName((ICON_FA_GEARS + std::string(" Project Properties")).c_str());
                    if (window != NULL && window->DockNode != NULL && window->DockNode->TabBar != NULL)
                        window->DockNode->TabBar->NextSelectedTabId = window->TabId;
                }
                else
                {
                    projectSettingsWinOpen = true;
                    resetProjectSettings = true;
                }
            }
            //if (ImGui::MenuItem("Reload API Files", "")) // Todo: Move this into settings or help
            //{
            //    ConsoleLogger::InfoLog("Reloading API Files");
            //    if (!std::filesystem::exists(ProjectManager::projectData.path / "api"))
            //    {
            //        std::filesystem::create_directories(ProjectManager::projectData.path / "api");
            //        Utilities::HideFile(ProjectManager::projectData.path / "api");
            //    }
            //    std::filesystem::remove_all(ProjectManager::projectData.path / "api");
            //    ProjectManager::CopyApiFiles(std::filesystem::path(__FILE__).parent_path(), ProjectManager::projectData.path / "api");
            //}
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(85, 0));
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Hierarchy", "")) {}
            if (ImGui::MenuItem("Content Browser", "")) {}
            if (ImGui::MenuItem("Properties", "")) {}
            if (ImGui::MenuItem("Sprite Editor", "")) {}
            if (ImGui::MenuItem("Animation Graph", "")) { animationGraphWinOpen = true; }
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(152, 0));
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

    if (ImGui::IsKeyPressed(ImGuiKey_S) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        SceneManager::SaveScene(SceneManager::GetActiveScene());
        ImGuiToast toast(ImGuiToastType::Success, 1500, true);
        toast.setTitle("Project Saved", "");
        toast.setContent("The project has successfully saved.");
        ImGui::InsertNotification(toast);
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(RaylibWrapper::GetScreenWidth(), 30));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.14f, 0.14f, 0.14f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##MenuBar2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Todo: It may be best to calculate the position only in Init and window resize.
    // Todo: Store the version in a variable and use that both in this titlebar, and the Raylib window titlebar 
    std::string text = "Cryonic Engine v0.1 - " + ProjectManager::projectData.name;
    ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text.c_str()).x) * 0.5f, 7.5f));
    ImGui::Text((text).c_str());

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 25, false));
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 60, 0));
    if (ImGui::Button("-", ImVec2(30, 30)))
    {
        RaylibWrapper::MinimizeWindow();
    }
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 30, 0));
    if (ImGui::Button("x", ImVec2(30, 30)))
    {
        // Todo: Popup save warning if the scene has not been saved. Popup options "Save & Quit", "Quit Without Saving", and "Cancel"
        closeEditor = true;
    }
    ImGui::PopFont();

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();


    // Notifications
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));

    ImGui::RenderNotifications();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void Editor::SetupViewport()
{
    ViewTexture = RaylibWrapper::LoadRenderTexture(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight());

    if (ProjectManager::projectData.is3D)
    {
        camera.fovy = 45;
        camera.up.y = 1;
        camera.position.y = 3;
        camera.position.z = -25;
        camera.projection = RaylibWrapper::CAMERA_PERSPECTIVE;
        camera.target = { 0.0f, 0.0f, 0.0f };
    }
    else
    {
        camera.projection = RaylibWrapper::CAMERA_ORTHOGRAPHIC;
        camera.up.y = 1;
        camera.fovy = 45;
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.position = {0, 3, 500};
    }
}

void CloseViewport()
{
    RaylibWrapper::UnloadRenderTexture(ViewTexture);
    RaylibWrapper::UnloadTexture(GridTexture);
}

void Editor::InitFonts()
{
    // First font created is the default folt. This should be Familiar-Pro-Bold 15
    FontManager::LoadFonts("Familiar-Pro-Bold", { 15, 10, 18, 20, 25, 30 });
    FontManager::LoadFont("BoldMarker", 90);
    FontManager::LoadFont("fa-solid-900", 15, true);
    FontManager::LoadFont("Roboto-Bold", 15);
}

void Editor::InitStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1, 1, 1, 1.00f); // ImVec4(0.11f, 0.64f, 0.92f, 1.00f)
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 5;
}

void Editor::InitMisc()
{
    //if (projectData.path.stem().string() != projectData.name)
    //    projectData.path = projectData.path / projectData.name;

    char* userProfile;
    size_t len;
    _dupenv_s(&userProfile, &len, "USERPROFILE");

    std::filesystem::path projectsPath = std::filesystem::path(userProfile) / "Documents" / "Cryonic Engine Projects";
    if (!std::filesystem::exists(projectsPath)) std::filesystem::create_directory(projectsPath);
}

void Editor::InitScenes()
{
    std::filesystem::path scenesPath = ProjectManager::projectData.path / "Assets" / "Scenes";
    std::filesystem::path selectedScenePath;
    if (std::filesystem::exists(scenesPath))
    {
        for (const auto& file : std::filesystem::directory_iterator(scenesPath))
        {
            if (file.is_regular_file() && file.path().extension() == ".scene" && SceneManager::LoadScene(file.path()))
            {
                selectedScenePath = file.path();
                break;
            }
        }
    }

    // No scenes found, create a new one
    if (selectedScenePath.empty())
    {
        if (std::filesystem::exists(ProjectManager::projectData.path / "Assets" / "Scenes" / "Default.scene"))
            SceneManager::LoadScene(ProjectManager::projectData.path / "Assets" / "Scenes" / "Default.scene");
        else
        {
            std::filesystem::create_directories(scenesPath);
            SceneManager::CreateScene(scenesPath / "Default.scene");
            SceneManager::LoadScene(scenesPath / "Default.scene");
            //SceneManager::SetActiveScene(); // LoadScene sets the scene to active
            //SceneManager::SaveScene(SceneManager::GetActiveScene()); // Creates Default.scene. It should not create a scene file automatically

        }
    }
    //else SceneManager::SetActiveScene(&SceneManager::GetScenes()->back()); // The scene is already getting set to active in the LoadScene() above
}

void Editor::Cleanup()
{
    IconManager::Cleanup();
    ShaderManager::Cleanup();

    for (auto& image : tempTextures)
    {
        UnloadTexture(*image);
        //delete image;
    }
    tempTextures.clear();

    for (auto& image : tempRenderTextures)
    {
        UnloadRenderTexture(*image);
        //delete image;
    }
    tempRenderTextures.clear();

    for (GameObject* gameObject : SceneManager::GetActiveScene()->GetGameObjects())
    {
        for (Component* component : gameObject->GetComponents())
        {
            component->Destroy();
        }
    }

    if (cameraSelected)
        UnloadRenderTexture(cameraRenderTexture);

    RaylibWrapper::CloseAudioDevice();

    RaylibWrapper::ImGui_ImplRaylib_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
}

void Editor::Init()
{
    RaylibWrapper::SetConfigFlags(RaylibWrapper::FLAG_WINDOW_UNDECORATED | RaylibWrapper::FLAG_WINDOW_RESIZABLE | RaylibWrapper::FLAG_WINDOW_HIGHDPI);// FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    RaylibWrapper::InitWindow(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight(), ("Cryonic Engine v0.1 - " + ProjectManager::projectData.name).c_str());
    RaylibWrapper::MaximizeWindow();
    RaylibWrapper::SetWindowMinSize(100, 100);
    RaylibWrapper::SetTargetFPS(144); // Todo: Set target FPS to monitor refresh rate and handle editor being moved across monitors or just take the higher refresh rate

    RaylibWrapper::InitAudioDevice();

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enabled Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enabled Multi-Viewports
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    InitStyle();

    // Setup Platform/Renderer backends
    RaylibWrapper::ImGui_ImplRaylib_Init();
    FontManager::InitFontManager();
    InitFonts();
    //InitImages();
    IconManager::Init();
    ShaderManager::Init();
    InitMisc();
    InitScenes(); // Must go after InitMisc() and ShaderManager::Init()

    SetupViewport();

    while (!closeEditor)
    {
        oneSecondDelay -= RaylibWrapper::GetFrameTime();
        FontManager::UpdateFonts();
        if (CameraComponent::main != nullptr)
            ShaderManager::UpdateShaders(CameraComponent::main->gameObject->transform.GetPosition().x, CameraComponent::main->gameObject->transform.GetPosition().y, CameraComponent::main->gameObject->transform.GetPosition().z);

        RaylibWrapper::ImGui_ImplRaylib_ProcessEvents();

        RaylibWrapper::ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();

        UpdateViewport();
        Render();

        // Rendering
        ImGui::Render();

        RaylibWrapper::BeginDrawing();
        RaylibWrapper::ClearBackground({64,64,64,255});
        RaylibWrapper::ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
        RaylibWrapper::EndDrawing();

        for (auto& image : tempTextures)
        {
            RaylibWrapper::UnloadTexture(*image);
            delete image;
        }
        tempTextures.clear();

        for (auto& image : tempRenderTextures)
        {
            RaylibWrapper::UnloadRenderTexture(*image);
        }
        tempRenderTextures.clear();

        if (!cameraSelected)
            RaylibWrapper::UnloadRenderTexture(cameraRenderTexture);

        if (oneSecondDelay <= 0)
            oneSecondDelay = 1;
    }

    Cleanup();
    RaylibWrapper::CloseWindow();
}
