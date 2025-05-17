#include "EventSheetEditor.h"
#include "ConsoleLogger.h"
#include "FontManager.h"
#include <fstream>
#include <unordered_set>
#include <IconsFontAwesome6.h>

void EventSheetEditor::Render()
{
	// Check if it has focus and CTRL S clciked and hovered
	// There is a ImGuiTableFlags_Reorderable, ImGuiTableFlags_Hideable, and ImGuiTableFlags_Sortable flag

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));

	int eventIndex = 0;
	for (EventSheetSystem::Event& event : events)
	{
		ImGui::PushID(eventIndex); // Ensure unique IDs per event

		float height = std::max(event.conditions.size(), event.actions.size()) * 30.0f;
		ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg;
 
		ImVec2 currentPos = ImGui::GetCursorPos();
		ImGui::SetCursorPosX(50);
		if (ImGui::BeginTable(("eventTable" + std::to_string(eventIndex)).c_str(), 2, tableFlags, { ImGui::GetWindowWidth() - 100, height }))
		{
			ImGui::TableNextRow();
			// Conditions
			ImGui::TableNextColumn();
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(48, 50, 52, 255));
			//ImGui::Text("Conditions");
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.19f, 0.20f, 1.00f));
			//ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
			//ImGui::BeginChild(("conditions" + std::to_string(eventIndex)).c_str(), { ImGui::GetWindowWidth() * 0.48f, childHeight }, true);
			for (size_t i = 0; i < event.conditions.size(); ++i)
			{
				std::string parameters = "";

				// Show the parameters in the condition name
				for (EventSheetSystem::Param param : event.conditions[i].params)
				{
					if (!parameters.empty())
						parameters += ", ";

					if (param.type == "string")
						parameters += param.name + " = " + std::any_cast<std::string>(param.value);
					else if (param.type == "int")
						parameters += param.name + " = " + std::to_string(std::any_cast<int>(param.value));
					else if (param.type == "float")
						parameters += param.name + " = " + std::to_string(std::any_cast<float>(param.value));
					else
						parameters.erase(parameters.length() - 2); // Removes the ", " if the parameter type is not supported
				}

				if (event.conditions[i].params.size() > 0)
					parameters = " (" + parameters + ")";

				ImGui::PushID(i);
				ImGui::SetCursorPosX(53);
				if (ImGui::Button((event.conditions[i].stringType + parameters + "##" + std::to_string(eventIndex) + "-" + std::to_string(i)).c_str()))
				{
					eventSelected = eventIndex;
					conditionSelected = i;
				}

				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

				// Changes the color of the "X" to red
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

				if (ImGui::Button("X"))
					event.actions.erase(event.actions.begin() + i);

				ImGui::PopStyleColor();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

				ImGui::PopID();
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
			if (ImGui::Button("Add Condition"))
			{
				eventSelected = eventIndex;
				conditionSelected = -2;
				//event.conditions.push_back({ "New Condition", {} });
			}
			ImGui::PopStyleColor();
			//ImGui::EndChild();
			//ImGui::PopStyleColor();

			// Actions
			ImGui::TableNextColumn();
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(41, 43, 45, 255), 1);
			//ImGui::Text("Actions");
			//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.17f, 0.18f, 0.19f, 1.00f));
			//ImGui::BeginChild(("actions" + std::to_string(eventIndex)).c_str(), { ImGui::GetWindowWidth() * 0.48f, childHeight }, true);
			for (size_t i = 0; i < event.actions.size(); ++i)
			{
				std::string parameters = "";

				// Show the parameters in the action name
				for (EventSheetSystem::Param param : event.actions[i].params)
				{
					if (!parameters.empty())
						parameters += ", ";

					if (param.type == "string")
						parameters += param.name + " = " + std::any_cast<std::string>(param.value);
					else if (param.type == "int")
						parameters += param.name + " = " + std::to_string(std::any_cast<int>(param.value));
					else if (param.type == "float")
						parameters += param.name + " = " + std::to_string(std::any_cast<float>(param.value));
					else
						parameters.erase(parameters.length() - 2); // Removes the ", " if the parameter type is not supported
				}

				if (event.actions[i].params.size() > 0)
					parameters = " (" + parameters + ")";

				ImGui::PushID(i);
				//ImGui::SetCursorPosX(53);
				if (ImGui::Button((event.actions[i].stringType + parameters + "##" + std::to_string(eventIndex) + "-" + std::to_string(i)).c_str()))
				{
					eventSelected = eventIndex;
					actionSelected = i;
				}

				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

				// Changes the color of the "X" to red
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

				if (ImGui::Button("X"))
					event.actions.erase(event.actions.begin() + i);

				ImGui::PopStyleColor();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

				ImGui::PopID();
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
			if (ImGui::Button("Add Action"))
			{
				eventSelected = eventIndex;
				actionSelected = -2;
				//event.actions.push_back({ "New Action", {} });
			}
			ImGui::PopStyleColor();
			//ImGui::EndChild();
			//ImGui::PopStyleColor();
			//ImGui::PopStyleVar();

			ImGui::EndTable();
		}

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() - 45, currentPos.y });

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("X"))
		{
			events.erase(events.begin() + eventIndex);
			ImGui::PopID();
			continue;
		}
		ImGui::PopStyleColor();

		ImGui::SetCursorPos(cursorPos);

		ImGui::PopID();
		eventIndex++;
	}
	ImGui::PopStyleColor(3);

	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Add Event").x) / 2);
	if (ImGui::Button("Add Event"))
	{
		events.push_back({});
	}

	RenderSelector();
}

void EventSheetEditor::RenderSelector()
{
	if (eventSelected == -1)
		return;

	static const std::vector<EventSheetSystem::Condition> conditions = {
		// Todo: Move to "Keyboard" sub group
		{ EventSheetSystem::Conditions::AnyKeyPressed, "Any Key Pressed", "Input", {} },
		{ EventSheetSystem::Conditions::KeyPressed, "Key Pressed", "Input", { {"Key", "string", std::string("")}}}, // ----------------------------------  Change this to key. Maybe I could instead make it like an array so its more universal usage?
			{ EventSheetSystem::Conditions::KeyReleased, "Key Released", "Input", { {"Key", "string", std::string("")} }},
		{ EventSheetSystem::Conditions::KeyDown, "Key Down", "Input", { {"Key", "string", std::string("")} }},

		// Todo: Move to "Mouse" sub group
		{ EventSheetSystem::Conditions::MouseButtonPressed, "Mouse Button Pressed", "Input", { {"Button", "int", 0} }},
		{ EventSheetSystem::Conditions::MouseButtonReleased, "Mouse Button Released", "Input", { {"Button", "int", 0} }},
		{ EventSheetSystem::Conditions::MouseButtonDown, "Mouse Button Down", "Input", { {"Button", "int", 0} }},
		{ EventSheetSystem::Conditions::MouseMoved, "Mouse Moved", "Input", {} },
		{ EventSheetSystem::Conditions::MouseInZone, "Mouse In Zone", "Input", {
			{"X", "float", 0.0f},
			{"Y", "float", 0.0f},
			{"Width", "float", 100.0f},
			{"Height", "float", 100.0f}
		}},

		// Todo: Move to "Gamepad" sub group
		{ EventSheetSystem::Conditions::GamepadButtonPressed, "Gamepad Button Pressed", "Input", {
			{"Gamepad", "int", 0},
			{"Button", "int", 0}
		}},
		{ EventSheetSystem::Conditions::GamepadButtonReleased, "Gamepad Button Released", "Input", {
			{"Gamepad", "int", 0},
			{"Button", "int", 0}
		}},
		{ EventSheetSystem::Conditions::GamepadButtonDown, "Gamepad Button Down", "Input", {
			{"Gamepad", "int", 0},
			{"Button", "int", 0}
		}},
		{ EventSheetSystem::Conditions::GamepadAxisMoved, "Gamepad Axis Moved", "Input", {
			{"Gamepad", "int", 0},
			{"Axis", "int", 0},
			{"MinValue", "float", -1.0f},
			{"MaxValue", "float", 1.0f}
		}},
	};

	static EventSheetSystem::Condition conditionData;
	static EventSheetSystem::Action actionData;

	static bool firstFrame = true;
	if (firstFrame)
	{
		if (conditionSelected != -1 && conditionSelected != -2) // Checks if a condition is being selected and if its not a new condition (meaning it should set the selected and params)
			conditionData = events[eventSelected].conditions[conditionSelected];

		if (actionSelected != -1 && actionSelected != -2)
			actionData = events[eventSelected].actions[actionSelected];

		firstFrame = false;
	}

	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
	ImGui::SetCursorPos({0, 0});
	ImGui::BeginChild("BlockOverlay", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), 0, ImGuiWindowFlags_NoScrollbar);
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::SetCursorPos({25, 50});
	ImGui::BeginChild("Selector", { ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() - 70 });

	ImGui::SetCursorPos({10, 10});
	ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 30, false));
	ImGui::Text(conditionSelected != -1 ? "Condition" : "Action");
	ImGui::PopFont();

	// Left side (Condition/Action selecting)
	ImGui::BeginChild("SelectingWin", { ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0, 0.5f });
	static std::unordered_set<std::string> expandedCategories;
	std::string lastCategory = "";
	bool isCategoryExpanded = false;
	if (conditionSelected != -1)
	{
		for (const EventSheetSystem::Condition& condition : conditions)
		{
			if (condition.category != lastCategory)
			{
				isCategoryExpanded = expandedCategories.find(condition.category) != expandedCategories.end();
				if (ImGui::Button(isCategoryExpanded ? (ICON_FA_ANGLE_DOWN + std::string(" ") + condition.category).c_str() : (ICON_FA_ANGLE_RIGHT + std::string(" ") + condition.category).c_str(), {ImGui::GetWindowWidth(), 25}))
				{
					if (isCategoryExpanded)
					{
						isCategoryExpanded = false;
						expandedCategories.erase(condition.category);
					}
					else
					{
						isCategoryExpanded = true;
						expandedCategories.insert(condition.category);
					}
				}
				lastCategory = condition.category;
			}

			ImGui::SetCursorPosX(20);
			if (isCategoryExpanded && ImGui::Button(condition.stringType.c_str(), { ImGui::GetWindowWidth() - 20, 25 }))
				conditionData = condition;
		}
	}
	else
	{
		static const std::vector<EventSheetSystem::Action> actions = {
			{ EventSheetSystem::Actions::Position, "Set Position", "Position", {
				{"GameObject", "string", std::string("self")},
				{"X", "float", 0.0f},
				{"Y", "float", 0.0f}
			}},
			{ EventSheetSystem::Actions::MovePosition, "Move Position", "Position", {
				{"GameObject", "string", std::string("self")},
				{"X", "float", 0.0f},
				{"Y", "float", 0.0f}
			}},
			{ EventSheetSystem::Actions::XPosition, "Set X Position", "Position", {
				{"GameObject", "string", std::string("self")},
				{"X", "float", 0.0f}
			}},
			{ EventSheetSystem::Actions::YPosition, "Set Y Position", "Position", {
				{"GameObject", "string", std::string("self")},
				{"Y", "float", 0.0f}
			}},
		};

		static std::unordered_set<std::string> expandedActionCategories;
		std::string lastActionCategory = "";
		bool isActionCategoryExpanded = false;

		for (const EventSheetSystem::Action& action : actions)
		{
			if (action.category != lastActionCategory)
			{
				isActionCategoryExpanded = expandedActionCategories.find(action.category) != expandedActionCategories.end();
				if (ImGui::Button(isActionCategoryExpanded ? (ICON_FA_ANGLE_DOWN + std::string(" ") + action.category).c_str() : (ICON_FA_ANGLE_RIGHT + std::string(" ") + action.category).c_str(), { ImGui::GetWindowWidth(), 25 }))
				{
					if (isActionCategoryExpanded)
					{
						isActionCategoryExpanded = false;
						expandedActionCategories.erase(action.category);
					}
					else
					{
						isActionCategoryExpanded = true;
						expandedActionCategories.insert(action.category);
					}
				}
				lastActionCategory = action.category;
			}

			ImGui::SetCursorPosX(20);
			if (isActionCategoryExpanded && ImGui::Button(action.stringType.c_str(), { ImGui::GetWindowWidth() - 20, 25 }))
				actionData = action;
		}
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
	ImGui::EndChild();

	// Right side (Parameters)
	if (!conditionData.stringType.empty() || !actionData.stringType.empty())
	{
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() / 2, 0 });
		ImGui::BeginChild("ParameterWin", { ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() - 60 });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.38f, 0.38f, 0.38f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0, 0.5f });
		ImGui::PushFont(FontManager::GetFont("Familiar-Pro-Bold", 30, false));
		ImGui::SetCursorPos({ 10, 10 });
		ImGui::Text(!conditionData.stringType.empty() ? conditionData.stringType.c_str() : actionData.stringType.c_str());
		ImGui::PopFont();

		std::vector<EventSheetSystem::Param>* params = !conditionData.stringType.empty() ? &conditionData.params : &actionData.params;

		float yPos = 60;
		for (EventSheetSystem::Param& param : *params)
		{
			// Todo: This has a lot of repetitive code

			ImGui::SetCursorPos( {10, yPos } );
			ImGui::Text((param.name + ":").c_str());

			// Handle Keys
			if (param.name == "Key" && param.type == "string")
			{
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
				if (ImGui::Button(!std::any_cast<std::string>(param.value).empty() ? (std::any_cast<std::string>(param.value) + " - Click To Change").c_str() : "Select Key"))
					ImGui::OpenPopup("KeySelector");

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

				if (ImGui::BeginPopup("KeySelector"))
				{
					auto AddKeyMenu = [&](const char* label, const std::vector<std::string>& keys)
					{
						if (ImGui::BeginMenu(label))
						{
							for (const auto& key : keys)
							{
								if (ImGui::Selectable(key.c_str()))
									param.value = key;
							}
							ImGui::EndMenu();
						}
					};

					AddKeyMenu("Letters", [] {
						std::vector<std::string> letters;
						for (char c = 'A'; c <= 'Z'; ++c) letters.push_back(std::string(1, c));
						return letters;
					}());

					AddKeyMenu("Numbers", [] {
						std::vector<std::string> numbers;
						for (char c = '0'; c <= '9'; ++c) numbers.push_back(std::string(1, c));
						return numbers;
					}());

					AddKeyMenu("Function Keys", [] {
						std::vector<std::string> fkeys;
						for (int i = 1; i <= 12; ++i) fkeys.push_back("F" + std::to_string(i));
						return fkeys;
					}());

					AddKeyMenu("Modifier Keys", {
						"SHIFT", "LEFT_SHIFT", "RIGHT_SHIFT",
						"CTRL", "LEFT_CTRL", "RIGHT_CTRL",
						"ALT", "LEFT_ALT", "RIGHT_ALT",
						"CAPSLOCK", "TAB"
					});

					AddKeyMenu("Arrow Keys", {
						"UP", "DOWN", "LEFT", "RIGHT"
					});

					AddKeyMenu("Control Keys", {
						"ENTER", "ESCAPE", "SPACE", "BACKSPACE", "DELETE",
						"INSERT", "HOME", "END", "PAGEUP", "PAGEDOWN",
						"PRINTSCREEN", "PAUSE", "NUMLOCK", "SCROLLLOCK"
					});

					AddKeyMenu("Numpad", {
						"NUMPAD_0", "NUMPAD_1", "NUMPAD_2", "NUMPAD_3", "NUMPAD_4",
						"NUMPAD_5", "NUMPAD_6", "NUMPAD_7", "NUMPAD_8", "NUMPAD_9",
						"NUMPAD_ADD", "NUMPAD_SUBTRACT", "NUMPAD_MULTIPLY", "NUMPAD_DIVIDE",
						"NUMPAD_DECIMAL", "NUMPAD_ENTER"
					});

					AddKeyMenu("Symbols", {
						"`", "-", "=", "[", "]", "\\", ";", "'", ",", ".", "/"
					});

					ImGui::EndPopup();
				}
			}

			// Mouse Buttons
			else if (param.name == "Button" && param.type == "int" &&
				(conditionData.category == "Mouse" || actionData.category == "Mouse"))
			{
				ImGui::SameLine();
				if (ImGui::Button(param.value.has_value() ? (std::any_cast<std::string>(param.value) + " - Click To Change").c_str() : "Select Button"))
					ImGui::OpenPopup("MouseButtonSelector");

				if (ImGui::BeginPopup("MouseButtonSelector"))
				{
					// Add mouse buttons
					static const char* buttonOptions[] = { "Left (0)", "Right (1)", "Middle (2)" };
					static const int buttonValues[] = { 0, 1, 2 };

					for (int i = 0; i < 3; i++)
					{
						if (ImGui::Selectable(buttonOptions[i]))
						{
							param.value = buttonValues[i];
						}
					}
					ImGui::EndPopup();
				}
			}

			// Gamepad Buttons
			else if (param.name == "Button" && param.type == "int" &&
				(conditionData.category == "Gamepad" || actionData.category == "Gamepad"))
			{
				ImGui::SameLine();
				if (ImGui::Button(param.value.has_value() ? (std::any_cast<std::string>(param.value) + " - Click To Change").c_str() : "Select Button"))
					ImGui::OpenPopup("GamepadButtonSelector");

				if (ImGui::BeginPopup("GamepadButtonSelector"))
				{
					// Common gamepad buttons
					static const char* gpButtonOptions[] = {
						"A/Cross (0)", "B/Circle (1)", "X/Square (2)", "Y/Triangle (3)",
						"Left Shoulder (4)", "Right Shoulder (5)",
						"Back/Select (6)", "Start (7)", "Left Thumb (8)", "Right Thumb (9)"
					};
					static const int gpButtonValues[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

					for (int i = 0; i < 10; i++)
					{
						if (ImGui::Selectable(gpButtonOptions[i]))
						{
							param.value = gpButtonValues[i];
						}
					}
					ImGui::EndPopup();
				}
			}

			// Gamepad Axis
			else if (param.name == "Axis" && param.type == "int" &&
				(conditionData.category == "Gamepad" || actionData.category == "Gamepad"))
			{
				ImGui::SameLine();
				if (ImGui::Button(param.value.has_value() ? (std::any_cast<std::string>(param.value) + " - Click To Change").c_str() : "Select Axis"))
					ImGui::OpenPopup("GamepadAxisSelector");

				if (ImGui::BeginPopup("GamepadAxisSelector"))
				{
					static const char* axisOptions[] = {
						"Left X (0)", "Left Y (1)", "Right X (2)", "Right Y (3)",
						"Left Trigger (4)", "Right Trigger (5)"
					};
					static const int axisValues[] = { 0, 1, 2, 3, 4, 5 };

					for (int i = 0; i < 6; i++)
					{
						if (ImGui::Selectable(axisOptions[i]))
						{
							param.value = axisValues[i];
						}
					}
					ImGui::EndPopup();
				}
			}

			// GameObject parameter
			else if (param.name == "GameObject" && param.type == "string")
			{
				ImGui::SameLine();
				if (ImGui::Button(param.value.has_value() ? (std::any_cast<std::string>(param.value) + " - Click To Change").c_str() : "Select GameObject"))
					ImGui::OpenPopup("GameObjectSelector");

				if (ImGui::BeginPopup("GameObjectSelector"))
				{
					if (ImGui::Selectable("self"))
						param.value = std::string("self");

					ImGui::EndPopup();
				}
}

			// Normal parameters
			else if (param.type == "string")
			{
				ImGui::SetCursorPos({ ImGui::CalcTextSize(param.name.c_str()).x + 20, yPos });
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize(param.name.c_str()).x - 50);
				static char inputBuffer[50] = "";
				strcpy_s(inputBuffer, std::any_cast<std::string>(param.value).c_str());
				if (ImGui::InputText(("##param" + param.name).c_str(), inputBuffer, sizeof(inputBuffer)))
					param.value = std::string(inputBuffer);
			}
			else if (param.type == "int")
			{
				ImGui::SetCursorPos({ ImGui::CalcTextSize(param.name.c_str()).x + 20, yPos });
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize(param.name.c_str()).x - 50);
				int value = std::any_cast<int>(param.value);
				if (ImGui::InputInt(("##param" + param.name).c_str(), &value))
					param.value = value;
			}
			else if (param.type == "float")
			{
				ImGui::SetCursorPos({ ImGui::CalcTextSize(param.name.c_str()).x + 20, yPos });
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize(param.name.c_str()).x - 50);
				float value = std::any_cast<float>(param.value);
				if (ImGui::InputFloat(("##param" + param.name).c_str(), &value, 0.1f, 1.0f, "%.2f"))
					param.value = value;
			}

			yPos += 20;
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		ImGui::EndChild();
	}

	ImGui::SetCursorPos({ImGui::GetWindowWidth() - 120, ImGui::GetWindowHeight() - 35 });
	if (ImGui::Button("Cancel", { 50, 25 }))
	{
		conditionData.category = "";
		conditionData.params.clear();
		conditionData.stringType = "";
		conditionData.type = EventSheetSystem::Conditions::None;

		actionData.category = "";
		actionData.params.clear();
		actionData.stringType = "";
		actionData.type = EventSheetSystem::Actions::None;

		expandedCategories.clear();
		eventSelected = -1;
		conditionSelected = -1;
		actionSelected = -1;
		firstFrame = true;
	}

	if ((conditionSelected != -1 && conditionData.stringType == "") || (actionSelected == -1 && actionData.stringType != ""))
		ImGui::BeginDisabled();

	ImGui::SetCursorPos({ ImGui::GetWindowWidth() - 60, ImGui::GetWindowHeight() - 35 });
	if (ImGui::Button("Apply", { 50, 25 }))
	{
		if (conditionSelected != -1)
		{
			if (conditionSelected == -2)
				events[eventSelected].conditions.push_back(conditionData);
			else
				events[eventSelected].conditions[conditionSelected] = conditionData;
		}
		else
		{
			if (actionSelected == -2)
				events[eventSelected].actions.push_back(actionData);
			else
				events[eventSelected].actions[actionSelected] = actionData;
		}

		conditionData.category = "";
		conditionData.params.clear();
		conditionData.stringType = "";
		conditionData.type = EventSheetSystem::Conditions::None;
		actionData.category = "";
		actionData.params.clear();
		actionData.stringType = "";
		actionData.type = EventSheetSystem::Actions::None;
		expandedCategories.clear();
		eventSelected = -1;
		conditionSelected = -1;
		actionSelected = -1;
		firstFrame = true;
	}
	if ((conditionSelected != -1 && conditionData.stringType == "") || (actionSelected == -1 && actionData.stringType != ""))
		ImGui::EndDisabled();

	ImGui::EndChild();
}

void EventSheetEditor::LoadData(nlohmann::json data, std::string path)
{
	filePath = path;

	if (!data.contains("events") || !data["events"].is_array())
		return;

	auto convertFromJson = [](const nlohmann::json& jsonValue) -> std::any {
		if (jsonValue.is_boolean())
			return std::any(jsonValue.get<bool>());
		if (jsonValue.is_number_integer()) 
			return std::any(jsonValue.get<int>());
		if (jsonValue.is_number_float()) 
			return std::any(jsonValue.get<float>());
		if (jsonValue.is_string()) 
			return std::any(jsonValue.get<std::string>());
		return nullptr;
	};

	for (const auto& eventJson : data["events"])
	{
		EventSheetSystem::Event event;
		for (const auto& conditionJson : eventJson["conditions"])
		{
			EventSheetSystem::Condition condition;
			condition.type = static_cast<EventSheetSystem::Conditions>(conditionJson["type"].get<int>());
			condition.stringType = conditionJson["stringType"].get<std::string>();

			for (const auto& paramJson : conditionJson["params"])
				condition.params.push_back({ paramJson["name"], paramJson["type"], convertFromJson(paramJson["value"]) });

			event.conditions.push_back(condition);
		}

		for (const auto& actionJson : eventJson["actions"])
		{
			EventSheetSystem::Action action;
			action.type = static_cast<EventSheetSystem::Actions>(actionJson["type"].get<int>());
			action.stringType = actionJson["stringType"].get<std::string>();

			for (const auto& paramJson : actionJson["params"])
				action.params.push_back({ paramJson["name"], paramJson["type"], convertFromJson(paramJson["value"]) });

			event.actions.push_back(action);
		}

		events.push_back(event);
	}
}

void EventSheetEditor::SaveData()
{
	nlohmann::json json = {
		{"version", 1},
		{"path", filePath},
		{"events", nlohmann::json::array()}
	};

	auto convertToJson = [](const std::any& value) -> nlohmann::json {
		if (value.type() == typeid(bool))
			return std::any_cast<bool>(value);
		if (value.type() == typeid(int))
			return std::any_cast<int>(value);
		if (value.type() == typeid(float))
			return std::any_cast<float>(value);
		if (value.type() == typeid(std::string))
			return std::any_cast<std::string>(value);
		return nullptr;
	};

	for (EventSheetSystem::Event& event : events)
	{
		nlohmann::json eventJson;
		eventJson["conditions"] = nlohmann::json::array();
		eventJson["actions"] = nlohmann::json::array();

		for (EventSheetSystem::Condition& condition : event.conditions)
		{
			nlohmann::json conditionJson = nlohmann::json::object();
			conditionJson["type"] = static_cast<int>(condition.type);
			conditionJson["stringType"] = static_cast<std::string>(condition.stringType);
			conditionJson["params"] = nlohmann::json::array();

			for (auto& param : condition.params)
			{
				nlohmann::json paramJson = nlohmann::json::object();
				paramJson["name"] = param.name;
				paramJson["type"] = param.type;
				paramJson["value"] = convertToJson(param.value);
				conditionJson["params"].push_back(paramJson);
			}

			eventJson["conditions"].push_back(conditionJson);
		}

		for (EventSheetSystem::Action& action : event.actions)
		{
			nlohmann::json actionJson = nlohmann::json::object();
			actionJson["type"] = static_cast<int>(action.type);
			actionJson["stringType"] = static_cast<std::string>(action.stringType);
			actionJson["params"] = nlohmann::json::array();

			for (auto& param : action.params)
			{
				nlohmann::json paramJson = nlohmann::json::object();
				paramJson["name"] = param.name;
				paramJson["type"] = param.type;
				paramJson["value"] = convertToJson(param.value);
				actionJson["params"].push_back(paramJson);
			}

			eventJson["actions"].push_back(actionJson);
		}

		json["events"].push_back(eventJson);
	}

	std::ofstream file(filePath, std::ofstream::trunc);
	if (file.is_open())
	{
		file << json.dump(4);
		file.close();
	}
	else
	{
		ConsoleLogger::ErrorLog("There was an error saving your " + std::filesystem::path(filePath).stem().string() + " event sheet.");
		// Todo: If its being saved from OnClose(), give a popup saying there was an issue saving it and if they would like to try and save again or force close.
	}
}

void EventSheetEditor::OnClose()
{
	// Todo: Ask if the user would like to save before closing
	SaveData();
}