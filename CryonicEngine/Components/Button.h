#pragma once

#include "Component.h"
#include "../CryonicCore.h"
#include "../FontManager.h"
#include "../Sprite.h"
#include "../Font.h"

class Button : public Component
{
public:
    Button(GameObject* obj, int id) : Component(obj, id)
	{
		runInEditor = true;
		name = "Button";

#if defined(EDITOR)
		std::string variables = R"(
        [
            0,
            [
                [
                    "Sprite",
                    "image",
                    "Square",
                    "Image",
                    {
                        "Extensions": [".png", ".jpg", ".jpeg"]
                    }
                ],
                [
                    "Color",
                    "color",
                    [ 200, 200, 200, 255 ],
                    "Color"
                ],
                [
                    "bool",
                    "disabled",
                    false,
                    "Disabled"
                ],
                [
                    "string",
                    "text",
                    "New Button",
                    "Text"
                ],
                [
                    "Color",
                    "textColor",
                    [ 0, 0, 0, 255 ],
                    "Text Color"
                ],
                [
                    "Font",
                    "font",
                    "None",
                    "Font",
                    {
                        "Extensions": [".ttf", ".otf"]
                    }
                ],
                [
                    "int",
                    "fontSize",
                    16,
                    "Font Size"
                ]
            ]
        ]
    )";
		exposedVariables = nlohmann::json::parse(variables);
#endif
	}
    Button* Clone() override
	{
		return new Button(gameObject, -1);
	}
	// Hide everything from API
	void Awake() override;
	void RenderGui() override;
#if defined(EDITOR)
	void EditorUpdate() override;
#endif

    void SetImage(Sprite* sprite);
    void SetColor(Color color);
    Color GetColor() const;
    void SetDisabled(bool disable);
    bool IsDisabled();
    void SetText(std::string text);
    std::string GetText() const;
    void SetFontSize(int size);
    int GetFontSize() const;
    void SetFont(Font* font);
    void SetTextColor(Color color);
    Color GetTextColor() const;
    bool IsHovered() const;

    // This only works for static functions and lambda
    void AddClickListener(std::function<void()> callback);

    template <typename T, typename U>
    void AddClickListener(T* obj, void (U::* func)()) {
        clickCallbacks.emplace_back([obj, func] { (obj->*func)(); });
    }

private:
    Sprite* image = nullptr;
    Color color = { 255, 255, 255, 255 };
    bool disabled;
    std::string text;
    Color textColor = { 255, 255, 255, 255 };
    Font* font = nullptr;
    int fontSize = 16;
    bool hovered = false;
    std::vector<std::function<void()>> clickCallbacks;
    bool setup = false;
};