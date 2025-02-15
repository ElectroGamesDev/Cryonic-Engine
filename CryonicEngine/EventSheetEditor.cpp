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
				ImGui::PushID(i);
				ImGui::SetCursorPosX(53);
				if (ImGui::Button((event.conditions[i].stringType + "##" + std::to_string(eventIndex) + "-" + std::to_string(i)).c_str()))
				{
					eventSelected = eventIndex;
					conditionSelected = i;
				}
				ImGui::SameLine();
				if (ImGui::Button("X"))
					event.conditions.erase(event.conditions.begin() + i);
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
				ImGui::PushID(i);
				ImGui::SetCursorPosX(53);
				if (ImGui::Button((event.actions[i].stringType + "##" + std::to_string(eventIndex) + "-" + std::to_string(i)).c_str()))
				{
					eventSelected = eventIndex;
					actionSelected = i;
				}
				ImGui::SameLine();
				if (ImGui::Button("X"))
					event.actions.erase(event.actions.begin() + i);
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
		if (ImGui::Button("X"))
		{
			events.erase(events.begin() + eventIndex);
			ImGui::PopID();
			continue;
		}

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
		{ EventSheetSystem::Conditions::AnyKeyPressed, "Any Key Pressed", "Input", {} },
		{ EventSheetSystem::Conditions::KeyPressed, "Key Pressed", "Input", { {"Key", "string", std::string("")}}}, // ----------------------------------  Change this to key. Maybe I could instead make it like an array so its more universal usage?
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
		// Todo: Action selector code
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
			ImGui::SetCursorPos( {10, yPos } );
			ImGui::Text(param.name.c_str());
			if (param.type == "string")
			{
				ImGui::SetCursorPos({ ImGui::CalcTextSize(param.name.c_str()).x + 20, yPos });
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize(param.name.c_str()).x - 50);
				static char inputBuffer[50] = "";
				strcpy_s(inputBuffer, std::any_cast<std::string>(param.value).c_str());
				if (ImGui::InputText(("##param" + param.name).c_str(), inputBuffer, sizeof(inputBuffer)))
					param.value = std::string(inputBuffer);
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