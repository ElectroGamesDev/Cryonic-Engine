#include "Event.h"
#include <algorithm>

void Event::Subscribe(std::function<void()>& callback)
{
	callbacks.push_back(callback);
}

void Event::Unsubscribe(std::function<void()>& callback)
{
    callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
            [&](const std::function<void()>& c) {
                return c.target_type() == callback.target_type() && c.target<void()>() == callback.target<void()>();
            }),
        callbacks.end());
}

void Event::Invoke()
{
    for (std::function<void()>& callback : callbacks)
    {
        if (callback)
            callback();
    }
}

int Event::CallbackCount()
{
    return callbacks.size();
}
