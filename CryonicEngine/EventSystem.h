#pragma once

#include "Event.h"
#include <functional>
#include <string>

namespace EventSystem
{
    void Subscribe(std::string eventName, std::function<void()>& callback);
    void Unsubscribe(std::string eventName, std::function<void()>& callback);
    void Invoke(std::string eventName);
};