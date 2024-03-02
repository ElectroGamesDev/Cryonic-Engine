#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <variant>
#include "Editor.h"
#include "FontManager.h"
#include "ConsoleLogger.h"
#include "Scenes/SceneManager.h"
#include <imgui_internal.h>
#include "IconsFontAwesome6.h"
#include <fstream>
#include "Utilities.h"
#include <variant>
#include "Components/MeshRenderer.h"
#include "Components/ScriptComponent.h"
#include "Components/CameraComponent.h"
#include "Components/Lighting.h"
#include "Components/SpriteRenderer.h"
#include "Components/Collider2D.h"
#include "Components/Rigidbody2D.h"
#include "IconManager.h"
#include "ShaderManager.h"
#include "ProjectManager.h"
#include "RaylibModelWrapper.h"

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

std::variant<std::monostate, GameObject*> objectInProperties = std::monostate{}; // Make a struct or something that holds a Path and ifstream String. Not specific to material so prefabs and stuff can use
GameObject* selectedObject = nullptr;
bool movingObjectX = false;
bool movingObjectY = false;
bool movingObjectZ = false;
bool cameraSelected = false;

RaylibWrapper::Vector2 lastMousePosition = { 0 };

bool explorerContextMenuOpen = false;
bool hierarchyContextMenuOpen = false;
GameObject* objectInHierarchyContextMenu = nullptr;
bool hierarchyObjectClicked = false;
bool componentsWindowOpen = false;
bool scriptCreateWinOpen = false;

bool resetComponentsWin = true;
bool resetPropertiesWin = true;
bool resetViewportWin = true;
bool resetFileExplorerWin = true;
bool resetHierarchy = true;
bool resetCameraView = true;

std::filesystem::path fileExplorerPath;

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
    // Load the 3D model

    RaylibModel model;
    model.Create(Custom, modelPath.string().c_str(), LitStandard);

    // Set a basic camera to view the model
    RaylibWrapper::Camera camera = { 0 };
    camera.position = { 0.0f, 0.0f, 6.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = RaylibWrapper::CAMERA_PERSPECTIVE;

    // Create a render texture
    //RenderTexture2D target = LoadRenderTexture(textureSize, textureSize);
    tempRenderTextures.push_back(new RaylibWrapper::RenderTexture2D(RaylibWrapper::LoadRenderTexture(textureSize, textureSize)));

    // Render model to texture
    BeginTextureMode(*tempRenderTextures.back());

    // Clear the render texture
    RaylibWrapper::ClearBackground(RaylibWrapper::Color{ 63, 63, 63, 255 });

    // Set camera position and projection for rendering
    RaylibWrapper::BeginMode3D(camera);

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

    if (ImGui::Begin((ICON_FA_CUBES + std::string(" Viewport")).c_str(), nullptr, ImGuiWindowFlags_NoCollapse))
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


        if (ImGui::IsWindowHovered() && RaylibWrapper::IsMouseButtonDown(RaylibWrapper::MOUSE_BUTTON_RIGHT)) // Todo: Maybe change to viewportFocused instead of IsWindowFocused
        {
            //    // Todo: Add SHIFT to speed up by x2, and scroll weel to change speed
            //rmbDown = true;
            //HideCursor();
            //DisableCursor();

            if (ProjectManager::projectData.is3D)
                RaylibWrapper::UpdateCamera(&camera, RaylibWrapper::CAMERA_PERSPECTIVE);

            else
            {
                RaylibWrapper::Vector2 mousePosition = RaylibWrapper::GetMousePosition();
                RaylibWrapper::Vector2 deltaMouse = RaylibWrapper::Vector2Subtract(mousePosition, lastMousePosition);
                camera.position.x += deltaMouse.x * 0.1f;
                camera.position.y += deltaMouse.y * 0.1f;
                camera.target = RaylibWrapper::Vector3Add(camera.position, {0,0,1});
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
                if (movingObjectX) // Todo: Dividing by 15 is a horrible solution. Fix this
                    x -= deltaMouse.x / 15;
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
    ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse;

    float nextX = 310;
    float nextY = 55;

    if (ImGui::Begin((ICON_FA_FOLDER_OPEN + std::string(" File Explorer")).c_str(), nullptr, windowFlags))
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

        for (const auto& entry : std::filesystem::directory_iterator(fileExplorerPath))
        {
            ImGui::PushID(entry.path().string().c_str());
            std::string id = entry.path().string().c_str();
            std::string fileName = entry.path().filename().string();
            if (fileName.length() > 7) {
                fileName = fileName.substr(0, 8);
                fileName = fileName + "..";
            }
            ImGui::SetCursorPosY(nextY);
            ImGui::SetCursorPosX(nextX);
            ImVec2 pos = ImGui::GetCursorPos();
            if (entry.is_directory())
            {
                if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["FolderIcon"], ImVec2(32, 32)))
                {
                    fileExplorerPath = entry.path();
                    ImGui::PopStyleColor(2);
                    ImGui::PopID();
                    ImGui::End();
                    return;
                }
            }
            else if (entry.is_regular_file())
            {
                std::string extension = entry.path().extension().string();
                if (extension == ".cpp")
                {
                    if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["CppIcon"], ImVec2(32, 32)))
                    {
                        //std::string command = "code " + entry.path().string(); // VSCode
                        std::system(("\"" + entry.path().string() + "\"").c_str()); // Use prefered editor
                    }
                }
                else if (extension == ".h")
                {
                    if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["HeaderIcon"], ImVec2(32, 32)))
                    {
                        //std::string command = "code " + entry.path().string(); // VSCode
                        std::system(("\"" + entry.path().string() + "\"").c_str()); // Use prefered editor
                    }
                }
                else if (extension == ".png" || extension == ".jpg" || extension == ".webp")
                {
                    tempTextures.push_back(new RaylibWrapper::Texture2D(RaylibWrapper::LoadTexture(entry.path().string().c_str())));

                    if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), tempTextures.back(), ImVec2(32, 32)))
                    {
                        std::string command = "start \"" + entry.path().string() + "\"";
                        std::system(command.c_str());
                    }

                    //ImVec2 dropPos = ImGui::GetCursorScreenPos();
                    //ImVec2 dropSize = ImVec2(100, 100);
                    //if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                    //    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    //        draggingObject = true;
                    //        dragData.SetTexture(texture);
                    //        dragData.SetName(entry.path().stem().string());
                    //        dragData.SetPath(entry.path().string());
                    //        objectInProperties = nullptr;
                    //        selectedObject = nullptr;
                    //    }
                    //    else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    //    {
                    //        ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                    //        std::string command = "start " + PathToString(entry.path());
                    //        std::system(command.c_str());
                    //    }
                    //}
                    //if (isDragging && draggingObject) {
                    //    // Set the next window position, position, and background alpha

                    //    int width, height;
                    //    SDL_QueryTexture(folderIconTexture, nullptr, nullptr, &width, &height);
                    //    //ConsoleLogger::InfoLog("Width" + std::to_string(width) + " Height" + std::to_string(height));

                    //    ImGui::SetNextWindowSize(ImVec2(width, height));
                    //    ImGui::SetNextWindowPos(dragPos);
                    //    ImGui::SetNextWindowBgAlpha(0.0f);

                    //    // Set the padding and margins for the button and the window
                    //    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    //    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                    //    // Begin the window and draw the button
                    //    ImGui::Begin("DragSpriteWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
                    //    ImGui::Image((void*)dragData.GetTexture(), ImGui::GetWindowSize());
                    //    ImGui::End();
                    //    ImGui::PopStyleVar(4);
                    //}

                    //if (draggingObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    //    draggingObject = false;
                    //    isDragging = false;
                    //    ConsoleLogger::InfoLog("Dropped on file explorer");
                    //}
                }
                else if (extension == ".obj" || extension == ".gltf" || extension == ".glb" || extension == ".vox" || extension == ".iqm" || extension == ".m3d")
                {
                    if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), &CreateModelPreview(entry.path(), 32)->texture, ImVec2(32, 32)))
                    {
                    }
                }
                else if (extension != ".asset")
                {
                    if (RaylibWrapper::rlImGuiImageButtonSize(("##" + id).c_str(), IconManager::imageTextures["UnknownFile"], ImVec2(32, 32)))
                        std::system(("start \"" + entry.path().string() + "\"").c_str());
                }
                else
                {
                    ImGui::PopID();
                    continue;
                }
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY());
            ImGui::SetCursorPosX(nextX - 2);
            ImGui::Text(fileName.c_str());

            nextX += 60;

            if (nextX > 1900)
            {
                nextX = 310;
                nextY = nextY + 75;
            }
            ImGui::PopID();
        }
        ImGui::PopStyleColor(2);


        // Mini Left File Explorer
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
                if (ImGui::MenuItem("Create Script"))
                {
                    explorerContextMenuOpen = false;
                    scriptCreateWinOpen = true;
                }
                if (ImGui::MenuItem("Create Sprite"))
                {
                    explorerContextMenuOpen = false;
                }
                if (ImGui::MenuItem("Create Material"))
                {
                    explorerContextMenuOpen = false;
                    std::ofstream outFile(fileExplorerPath / "NewMaterial.mat");
                    // Todo: In file explorer, show material as multi-color/rainbow square/sphere 
                    if (outFile.is_open()) {

                        outFile.close();
                    }
                    else {

                    }
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Open Folder In File Explorer"))
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
                try
                {
                    std::filesystem::path path = fileExplorerPath / std::filesystem::path(droppedFiles.paths[i]).filename();
                    if (std::filesystem::exists(path))
                    {
                        // Todo: Popup a window asking if the user would like to replace the file
                        std::filesystem::remove(path);
                    }
                    if (std::filesystem::is_directory(droppedFiles.paths[i]))
                        std::filesystem::copy(droppedFiles.paths[i], path, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                    else std::filesystem::copy_file(droppedFiles.paths[i], path);
                }
                catch (const std::filesystem::filesystem_error& error)
                {
                    // Todo: Popup a window with the error
                    ConsoleLogger::WarningLog("Failed to copy file. Error: " + std::string(error.what())); // Todo: Remove this log once I add a popup
                }
            }

            UnloadDroppedFiles(droppedFiles);
        }
    }
    ImGui::End();
}

void Editor::RenderFileExplorerTreeNode(std::filesystem::path path, bool openOnDefault)
{
    bool hasChildren = false;
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
    if (ImGui::Begin((ICON_FA_GEARS + std::string(" Properties")).c_str(), nullptr, windowFlags) && std::holds_alternative<GameObject*>(objectInProperties) && std::find_if(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [&](const auto& obj) { return obj == std::get<GameObject*>(objectInProperties); }) != SceneManager::GetActiveScene()->GetGameObjects().end())
    {
        ImGui::BeginGroup();
        if (std::holds_alternative<GameObject*>(objectInProperties))
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
            int xRot = static_cast<int>(rot.x);
            ImGui::SetNextItemWidth(width);
            // Todo: Create the bool "rotation" updated and if its updated on X, Y, Z, set it to true, then below check if its true and if it is, update the rotation. This way it reduces duplicate code. DO the same with Position and Scale
            if (ImGui::InputInt("##ObjectXRotation", &xRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(EulerToQuaternion((float)xRot* RAD, rot.y* RAD, rot.z* RAD));
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            int yRot = static_cast<int>(rot.y);
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputInt("##ObjectYRotation", &yRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(EulerToQuaternion((float)xRot* RAD, rot.y* RAD, rot.z* RAD));
            }
            ImGui::SameLine();
            ImGui::Text("Z");
            ImGui::SameLine();
            int zRot = static_cast<int>(rot.z);
            ImGui::SetNextItemWidth(width);
            if (ImGui::InputInt("##ObjectZRotation", &zRot, 0, 0)) {
                std::get<GameObject*>(objectInProperties)->transform.SetRotation(EulerToQuaternion((float)xRot * RAD, rot.y * RAD, rot.z * RAD));
            }

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
                                ImGui::InputFloat(("##" + name).c_str(), &value);
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
                            else if ((*it).size() > 4) // This is probably not a good solution for checking if its an enum or not.
                            {
                                // Todo: The enum values won't be capitalized or have spaces, only the selected enum value will be capitalized with spaces
                                ImGui::SameLine();
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
                                std::string value = (*it)[2].get<std::string>();
                                ImGui::SetNextItemWidth(60);
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
    else flags |= ImGuiTreeNodeFlags_OpenOnArrow;

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
                objectInProperties = gameObject;
                selectedObject = gameObject;

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
        }

        ImGui::TreePop();
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
                    selectedObject = nullptr;

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
                    gameObject->AddComponent<CameraComponent>();
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
                        spriteRenderer.SetTexturePath(objectToCreate);

                        gameObject->AddComponent<Collider2D>(); // Todo: Set size and type
                    }

                }

                //SceneManager::GetActiveScene()->AddGameObject(gameObject);

                for (Component* component : SceneManager::GetActiveScene()->GetGameObjects().back()->GetComponents())
                    component->gameObject = SceneManager::GetActiveScene()->GetGameObjects().back();

                if (objectInHierarchyContextMenu != nullptr)
                    SceneManager::GetActiveScene()->GetGameObjects().back()->SetParent(objectInHierarchyContextMenu);

                selectedObject = SceneManager::GetActiveScene()->GetGameObjects().back();
                objectInProperties = SceneManager::GetActiveScene()->GetGameObjects().back();

                if (selectedObject->GetComponent<CameraComponent>() != nullptr)
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
        ImGui::DockBuilderDockWindow((ICON_FA_GEARS + std::string(" Properties")).c_str(), dock_id_right);
        ImGui::DockBuilderDockWindow((ICON_FA_FOLDER_OPEN + std::string(" File Explorer")).c_str(), dock_id_bottom);
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
            if (ImGui::MenuItem("Open Project", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save Project", "Ctrl+S"))
            {
                ProjectManager::SaveProject();
            }
            if (ImGui::BeginMenu("Build Project"))
            {
                if (ImGui::MenuItem("Windows", ""))
                {
                    ProjectManager::BuildToWindows(ProjectManager::projectData);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(43, 0));
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Project Settings", "")) {}
            if (ImGui::MenuItem("Reload API Files", "")) // Todo: Move this into settings or help
            {
                ConsoleLogger::InfoLog("Reloading API Files");
                if (!std::filesystem::exists(ProjectManager::projectData.path / "api"))
                {
                    std::filesystem::create_directories(ProjectManager::projectData.path / "api");
                    Utilities::HideFile(ProjectManager::projectData.path / "api");
                }
                std::filesystem::remove_all(ProjectManager::projectData.path / "api");
                ProjectManager::CopyApiFiles(std::filesystem::path(__FILE__).parent_path(), ProjectManager::projectData.path / "api");
            }
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(85, 0));
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Hierarchy", "")) {}
            if (ImGui::MenuItem("File Explorer", "")) {}
            if (ImGui::MenuItem("Properties", "")) {}
            if (ImGui::MenuItem("Sprite Editor", "")) {}
            ImGui::EndMenu();
        }
        ImGui::SetCursorPos(ImVec2(152, 0));
        if (ImGui::BeginMenu("Help")) {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

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
        //camera2D.zoom = 1;
        //camera2D.rotation = 0.0f;

        camera.projection = RaylibWrapper::CAMERA_ORTHOGRAPHIC;
        camera.up.y = 1;
        camera.fovy = 45;
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.position = {0, 3, -50};
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
    FontManager::CreateFonts("Familiar-Pro-Bold", { 15, 10, 18, 20, 25, 30 });
    FontManager::CreateFont("BoldMarker", 90);
    FontManager::CreateFont("fa-solid-900", 15, true);
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
    // Loads all scenes in /Assets/Scenes
    std::filesystem::path scenesPath = ProjectManager::projectData.path / "Assets" / "Scenes";
    std::filesystem::path selectedScenePath;
    if (std::filesystem::exists(scenesPath))
    {
        for (const auto& file : std::filesystem::directory_iterator(scenesPath))
        {
            if (file.is_regular_file() && file.path().extension() == ".scene" && SceneManager::LoadScene(file.path()))
            {
                selectedScenePath = file.path();
            }
        }
    }

    // No scenes found, create a new one
    if (selectedScenePath.empty())
    {
        std::filesystem::create_directories(scenesPath);
        SceneManager::SetActiveScene(SceneManager::CreateScene());
        SceneManager::GetActiveScene()->SetPath(scenesPath / "Default.scene");
        SceneManager::SaveScene(SceneManager::GetActiveScene()); // Creates Default.scene
    }
    // Sets the last scene in the vector active
    else SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
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

    RaylibWrapper::ImGui_ImplRaylib_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Init()
{
    RaylibWrapper::SetConfigFlags(RaylibWrapper::FLAG_WINDOW_UNDECORATED | RaylibWrapper::FLAG_WINDOW_RESIZABLE | RaylibWrapper::FLAG_WINDOW_HIGHDPI);// FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT
    RaylibWrapper::InitWindow(RaylibWrapper::GetScreenWidth(), RaylibWrapper::GetScreenHeight(), ("Cryonic Engine v0.1 - " + ProjectManager::projectData.name).c_str());
    RaylibWrapper::MaximizeWindow();
    RaylibWrapper::SetWindowMinSize(100, 100);
    RaylibWrapper::SetTargetFPS(144); // Todo: Set target FPS to monitor refresh rate and handle editor being moved across monitors or just take the higher refresh rate

    // Setup Dear ImGui context
    ImGui::CreateContext();
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
    InitMisc();
    InitScenes(); // Must go after InitMisc()
    ShaderManager::Init();

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
