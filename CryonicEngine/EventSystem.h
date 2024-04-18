#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class EventSystem {
public:
    using EventHandler = std::function<void(void*)>;

    static void Subscribe(const std::string& eventName, EventHandler handler);
    static void Invoke(const std::string& eventName, void* eventData);

private:
    static std::unordered_map<std::string, std::vector<EventHandler>> eventHandlers;
};