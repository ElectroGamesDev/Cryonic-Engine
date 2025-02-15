#pragma once

#include "EditorWindow.h"
#include <json.hpp>
#include <any>
#include "EventSheetSystem.h"

class EventSheetEditor : public EditorWindow
{
public:
	void Render() override;
	void OnClose() override;

    void RenderSelector();
	void LoadData(nlohmann::json data, std::string path);
    void SaveData();

private:
    std::string filePath;
    std::vector<EventSheetSystem::Event> events;
    int eventSelected = -1; // Index of the event selected. -1 = none.
    int conditionSelected = -1; // Index of the condition selected. -1 = none, -2 = new, anything else is a valid index for events
    int actionSelected = -1; // Index of the action selected. -1 = none, -2 = new, anything else is a valid index for events
};