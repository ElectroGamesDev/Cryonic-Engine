#include "CryonicEngine.h"
#include "raylib.h"
#include <iostream>
#include <fstream>
#include <variant>
#include "Editor.h"
#include "FontManager.h"
#include "ConsoleLogger.h"
#include <imgui_impl_raylib.h>
#include <fstream>
#include "Utilities.h"
#include <variant>

ProjectData projectData;
std::filesystem::path projectsPath;
bool openEditor = false;

enum WindowOpened
{
    MainWin,
    ProjectCreateWin
}; WindowOpened windowOpened;

void RenderMainWin()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##MainWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::SetCursorPos(ImVec2(155, 30));
    ImGui::PushFont(FontManager::GetFont("BoldMarker", 90, false));
    ImGui::Text("Cryonic Engine");
    ImGui::PopFont();

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 30, false));
    ImGui::SetCursorPos(ImVec2(150, 150));
    if (ImGui::Button("Create Project", ImVec2(200, 75)))
    {
        windowOpened = ProjectCreateWin;
    }

    ImGui::SetCursorPos(ImVec2(440, 150));
    if (ImGui::Button("Open Project", ImVec2(200, 75)))
    {
        std::string path = Utilities::SelectFolderDialog(projectsPath);
        // Todo: Check if path is really a Cryonic Engine project path
        if (path != "")
        {
            openEditor = true;
            projectData.path = path;
            projectData.name = projectData.path.stem().string();
        }
    }
    ImGui::PopFont();

    ImGui::SetCursorPos(ImVec2(275, 250));
    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 40, false));
    ImGui::Text("Recent Projects");
    ImGui::PopFont();

    ImGui::End();
}

void RenderProjectCreateWin() // Todo: Add grayed out button
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##CreationWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 30, false));
    ImGui::SetCursorPos(ImVec2(35, 9));
    ImGui::Text("Templates");
    ImGui::SetCursorPos(ImVec2(10, 50));
    if (ImGui::Button("Blank 3D", ImVec2(175, 50)))
    {
        
    }
    ImGui::SetCursorPos(ImVec2(10, 120));
    if (ImGui::Button("Blank 2D", ImVec2(175, 50)))
    {

    }
    ImGui::PopFont();
    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 20, false));
    ImGui::SetCursorPos(ImVec2(10, 190));
    if (ImGui::Button("2.5D Side-Scroller", ImVec2(175, 50)))
    {

    }
    ImGui::PopFont();

    ImGui::GetWindowDrawList()->AddLine(ImVec2(200, 0), ImVec2(200, 600), IM_COL32(45, 45, 45, 255), 3);

    // Replace this with a sample image, and create a boarder for the image with rounding with AddRect()
    ImGui::SetCursorPos(ImVec2(150, 150));
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(325, 50), ImVec2(675, 250), IM_COL32(255,0,0,255));

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 30, false));
    ImGui::SetCursorPos(ImVec2(325, 265));
    ImGui::Text("            Template Name");
    ImGui::PopFont();

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 20, false));
    ImGui::SetCursorPos(ImVec2(325, 300));
    ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1), "                    Template Description");

    ImGui::SetCursorPos(ImVec2(350, 350));
    ImGui::Text("Project Name");
    ImGui::SetCursorPos(ImVec2(350, 375));
    ImGui::SetNextItemWidth(300);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    static char nameBuffer[26] = "";
    ImGui::InputText("##ProjectName", nameBuffer, IM_ARRAYSIZE(nameBuffer));

    ImGui::SetCursorPos(ImVec2(350, 425));
    ImGui::Text("Project Location");
    ImGui::SetCursorPos(ImVec2(350, 450));
    if (ImGui::Button(projectData.path.string().c_str(), ImVec2(300, 25)))
    {
        std::filesystem::path path = Utilities::SelectFolderDialog(projectData.path);
        if (path != "")
        {
            projectData.path = path;
        }
    }
    ImGui::PopFont();

    ImGui::PopStyleVar();

    ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 20, false));
    ImGui::SetCursorPos(ImVec2(325, 525));
    if (ImGui::Button("Cancel", ImVec2(150, 35)))
    {
        windowOpened = MainWin;
    }

    std::string newName = nameBuffer;
    newName.erase(std::remove_if(newName.begin(), newName.end(), [](unsigned char c) {
        return std::isspace(c);
        }), newName.end());

    bool canCreate = true;
    // Todo: Popup message saying the user must enter a project name
    if (newName == "") canCreate = false;

    if (!canCreate) ImGui::BeginDisabled();
    ImGui::SetCursorPos(ImVec2(525, 525));
    if (ImGui::Button("Create", ImVec2(150, 35)))
    {
        projectData.name = nameBuffer;
        if (!std::filesystem::exists(projectData.path / projectData.name))
        {
            int error = ProjectManager::CreateProject(projectData);
            switch (error)
            {
            case 0:
                // No error, project creation was a success
                openEditor = true;
                break;
            }
        }
        else
        {
            // Todo: Popup message saying a project with that name already exists at location
        }
    }
    if (!canCreate) ImGui::EndDisabled();
    ImGui::PopFont();

    ImGui::End();
}

void InitFonts()
{
    FontManager::CreateFonts("Familiar-Pro-Bold", { 15, 18, 20, 25, 30, 40 });
    FontManager::CreateFont("BoldMarker", 90);
}

void InitStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
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
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
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

void InitMisc()
{
    windowOpened = MainWin;

    char* userProfile;
    size_t len;
    _dupenv_s(&userProfile, &len, "USERPROFILE");

    projectsPath = std::filesystem::path(userProfile) / "Documents" / "Cryonic Engine Projects";
    if (!std::filesystem::exists(projectsPath)) std::filesystem::create_directory(projectsPath);

    projectData.path = projectsPath;
    projectData.templateData._template = Templates::Blank3D;
}

void Cleanup()
{
    ImGui_ImplRaylib_Shutdown();
    ImGui::DestroyContext();
}

void main()
{
    InitWindow(800, 600, ("Cryonic Engine - v0.1-ALPHA"));
    SetWindowMinSize(800, 600);
    SetTargetFPS(60);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    InitStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplRaylib_Init();
    FontManager::InitFontManager();
    InitFonts();
    InitMisc();


    while (!WindowShouldClose() && !openEditor)
    {
        FontManager::UpdateFonts();

        ImGui_ImplRaylib_ProcessEvents();

        ImGui_ImplRaylib_NewFrame();
        ImGui::NewFrame();

        if (windowOpened == MainWin) RenderMainWin();
        else RenderProjectCreateWin();

        ImGui::Render();

        BeginDrawing(); 
        ClearBackground(Color{35,35,35,255});

        ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
        EndDrawing();
    }

    Cleanup();
    CloseWindow();

    if (openEditor)
    {
        Editor editor;
        editor.Init(projectData);
    }
}