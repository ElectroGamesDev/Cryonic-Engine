#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace EventSystem {
    using EventHandler = std::function<void(void*)>;

    extern std::unordered_map<std::string, std::vector<EventHandler>> eventHandlers;

    void Subscribe(const std::string& eventName, EventHandler handler);
    void Invoke(const std::string& eventName, void* eventData);
}