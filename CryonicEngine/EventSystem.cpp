#include "EventSystem.h"

namespace EventSystem {
    std::unordered_map<std::string, std::vector<EventHandler>> eventHandlers;

    void Subscribe(const std::string& eventName, EventHandler handler) {
        eventHandlers[eventName].push_back(handler);
    }

    void Invoke(const std::string& eventName, void* eventData) {
        for (auto& handler : eventHandlers[eventName])
            handler(eventData);
    }
}