#include "EventSystem.h"

void EventSystem::Subscribe(const std::string& eventName, EventHandler handler) {
    eventHandlers[eventName].push_back(handler);
}

void EventSystem::Invoke(const std::string& eventName, void* eventData) {
    for (auto& handler : eventHandlers[eventName])
        handler(eventData);
}