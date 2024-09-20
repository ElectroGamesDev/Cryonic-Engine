#include "Button.h"
#include "../RaylibWrapper.h"
#include "CameraComponent.h"
#if defined (EDITOR)
#include "../Editor.h"
#else
#include "../imgui.h"
#endif

void Button::Awake()
{
#if defined(EDITOR)
    image = new Sprite(exposedVariables[1][0][2].get<std::string>()); // Todo: Handle if the path no longer exists
#endif
    //SetSprite(sprite);

#if defined(EDITOR)
    if (exposedVariables[1][5][2].get<std::string>() == "None") // Font
        return;

    font = new Font(exposedVariables[1][5][2].get<std::string>()); // Todo: Handle if the path no longer exists
#else
    if (font->GetPath() == "None")
        return;
#endif
    SetFont(font);
}

void Button::RenderGui()
{
    Vector2 position = CameraComponent::main->GetWorldToScreen(gameObject->transform.GetPosition());

#if defined(EDITOR)
    // Divding positions by Raylib window size then multiply it by Viewport window size
    position.x = position.x / RaylibWrapper::GetScreenWidth() * ImGui::GetWindowSize().x;
    position.y = position.y / RaylibWrapper::GetScreenHeight() * ImGui::GetWindowSize().y;
#endif

    ImVec2 size = {100, 40};
    Vector2 textPosition = position;
    bool buttonClicked = false;

    position.x -= size.x * gameObject->transform.GetScale().x / 2;
    position.y -= size.y * gameObject->transform.GetScale().y / 2;

    ImGui::SetCursorPos({ position.x, position.y });

    // Image / Button
#if defined(EDITOR)
    ImGui::BeginDisabled();
#else
    if (disabled)
        ImGui::BeginDisabled();
#endif

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,0});
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    if (image->GetTexture())
    {
        RaylibWrapper::Texture2D* texture = image->GetTexture();
        if (RaylibWrapper::rlImGuiImageButtonSizeTint(("##" + text + std::to_string(id)).c_str(), texture, { size.x * gameObject->transform.GetScale().x, size.y * gameObject->transform.GetScale().y }, { (float)color.r, (float)color.g, (float)color.b, (float)color.a }))
            buttonClicked = true;
    }
    else if (image->GetPath() == "Square")
    {
        ImVec4 newColor = { (float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255 };
        ImGui::PushStyleColor(ImGuiCol_Button, newColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { newColor.x + 0.1f, newColor.y + 0.1f, newColor.z + 0.1f, newColor.w + 0.1f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { newColor.x - 0.1f, newColor.y - 0.1f, newColor.z - 0.1f, newColor.w - 0.1f });
        if (ImGui::Button(("##" + text + std::to_string(id)).c_str(), { size.x * gameObject->transform.GetScale().x, size.y * gameObject->transform.GetScale().y }))
            buttonClicked = true;
        ImGui::PopStyleColor(3);
    }
    //    else if (image->GetPath() == "Circle")
    //    {
    //#if defined(EDITOR)
    //        position.x += Editor::viewportPosition.x;
    //        position.y += Editor::viewportPosition.y;
    //#endif
    //
    //        ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(position.x, position.y), gameObject->transform.GetScale().x * 25, IM_COL32(color.r, color.g, color.b, color.a));
    //    }
    else
        if (ImGui::InvisibleButton(("##" + text + std::to_string(id)).c_str(), { size.x * gameObject->transform.GetScale().x, size.y * gameObject->transform.GetScale().y }))
            buttonClicked = true;

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    hovered = ImGui::IsItemHovered();

#if defined(EDITOR)
    ImGui::EndDisabled();
#else
    if (disabled)
        ImGui::EndDisabled();
#endif

    // Text
    if (font && text != "")
    {
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        ImGui::SetCursorPos({ textPosition.x - textSize.x / 2, textPosition.y - textSize.y / 2 });
        ImGui::PushFont(FontManager::GetFont(font->GetPath(), fontSize, false));
        ImGui::TextColored({ (float)textColor.r, (float)textColor.g, (float)textColor.b, (float)textColor.a }, text.c_str());
        ImGui::PopFont();
    }

    if (buttonClicked)
        for (std::function<void()> callback : clickCallbacks)
            callback();
}

#if defined(EDITOR)
void Button::EditorUpdate()
{
    if (!setup)
    {
        Awake();
        setup = true;
    }

    color.r = exposedVariables[1][1][2][0].get<int>();
    color.g = exposedVariables[1][1][2][1].get<int>();
    color.b = exposedVariables[1][1][2][2].get<int>();
    color.a = exposedVariables[1][1][2][3].get<int>();

    textColor.r = exposedVariables[1][4][2][0].get<int>();
    textColor.g = exposedVariables[1][4][2][1].get<int>();
    textColor.b = exposedVariables[1][4][2][2].get<int>();
    textColor.a = exposedVariables[1][4][2][3].get<int>();

    disabled = exposedVariables[1][2][2].get<bool>();

    text = exposedVariables[1][3][2].get<std::string>();

    if (image->GetPath() != exposedVariables[1][0][2])
    {
        SetImage(new Sprite(exposedVariables[1][0][2].get<std::string>()));
        // Todo: Should it unload the old sprite?
    }

    if (exposedVariables[1][5][2].get<std::string>() != "None") // Font
    {
        if (fontSize != exposedVariables[1][6][2].get<int>())
            SetFontSize(exposedVariables[1][6][2].get<int>());

        if (!font || font->GetPath() != exposedVariables[1][5][2])
        {
            SetFont(new Font(exposedVariables[1][5][2].get<std::string>()));
            // Todo: Should it unload the old font?
        }
    }
    else
        font = nullptr; // Todo: Should it unload the font if the font != nullptr?
}
#endif

void Button::SetImage(Sprite* image)
{
    this->image = image;
}

void Button::SetColor(Color color)
{
    this->color = color;
}

Color Button::GetColor() const
{
    return color;
}

void Button::SetDisabled(bool disable)
{
    disabled = disable;
}

bool Button::IsDisabled()
{
    return disabled;
}

void Button::SetText(std::string text)
{
    this->text = text;
}
std::string Button::GetText() const
{
    return text;
}
void Button::SetFontSize(int size)
{
    fontSize = size;
    if (font)
        FontManager::LoadFont(font->GetPath(), fontSize, false, true);
}
int Button::GetFontSize() const
{
    return fontSize;
}

void Button::SetFont(Font* font)
{
    this->font = font;
    SetFontSize(fontSize);
}

void Button::SetTextColor(Color color)
{
    this->textColor = color;
}

Color Button::GetTextColor() const
{
    return textColor;
}

bool Button::IsHovered() const
{
    return hovered;
}

void Button::AddClickListener(std::function<void()> callback)
{
    clickCallbacks.push_back(callback);
}