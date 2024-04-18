#include "EventSystem.h"

std::unordered_map<std::string, std::vector<EventSystem::EventHandler>> eventHandlers;

std::vector<EventSystem::EventHandler>& EventSystem::GetHandlers(const std::string& eventName) {
    return eventHandlers[eventName];
}

void EventSystem::Subscribe(const std::string& eventName, EventHandler handler) {
    GetHandlers(eventName).push_back(handler);
}

void EventSystem::Subscribe(const std::string& eventName, void(*handler)(void*)) {
    EventHandler wrapper = [handler](void* data) {
        handler(data);
    };
    GetHandlers(eventName).push_back(wrapper);
}

void EventSystem::Invoke(const std::string& eventName) {
    for (auto& handler : GetHandlers(eventName)) {
        handler(nullptr);
    }
}