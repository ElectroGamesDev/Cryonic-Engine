#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class EventSystem {
public:
    using EventHandler = std::function<void(void*)>;

    template<typename Func>
    static void Subscribe(const std::string& eventName, Func&& handler) {
        SubscribeImpl(eventName, std::forward<Func>(handler), std::make_index_sequence<std::tuple_size_v<std::decay_t<Func>>>{});
    }

    static void Subscribe(const std::string& eventName, EventHandler handler);

    static void Subscribe(const std::string& eventName, void(*handler)(void*));

    template<typename... Args>
    static void Invoke(const std::string& eventName, Args&&... args) {
        for (auto& handler : GetHandlers(eventName)) {
            handler(&std::make_tuple(std::forward<Args>(args)...));
        }
    }

    static void Invoke(const std::string& eventName);

private:
    template<typename Func, std::size_t... Is>
    static void SubscribeImpl(const std::string& eventName, Func&& handler, std::index_sequence<Is...>) {
        auto wrapper = [=](void* data) {
            handler(*reinterpret_cast<std::tuple_element_t<Is, std::decay_t<Func>>*>(data)...);
        };
        GetHandlers(eventName).push_back(wrapper);
    }

    static std::vector<EventHandler>& GetHandlers(const std::string& eventName);
};