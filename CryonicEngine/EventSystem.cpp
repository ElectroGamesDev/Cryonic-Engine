#include "EventSystem.h"
#include <unordered_map>

namespace EventSystem
{
    static std::unordered_map<std::string, Event> events;

    void Subscribe(std::string eventName, std::function<void()>& callback)
    {
        auto it = events.find(eventName);

        if (it == events.end())
        {
            Event newEvent;
            newEvent.Subscribe(callback);
            events[eventName] = std::move(newEvent);
        }
        else
            it->second.Subscribe(callback);

    }

    void Unsubscribe(std::string eventName, std::function<void()>& callback)
    {
        auto it = events.find(eventName);
        if (it != events.end())
        {
            it->second.Unsubscribe(callback);
            if (it->second.CallbackCount() < 1)
                events.erase(it);
        }

    }

    void Invoke(std::string eventName)
    {
        auto it = events.find(eventName);
        if (it != events.end())
            it->second.Invoke();

        events[eventName].Invoke();
    }
}