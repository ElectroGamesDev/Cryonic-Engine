#pragma once

#include <vector>
#include <functional>

class Event
{
public:
	void Subscribe(std::function<void()> callback);
	void Unsubscribe(std::function<void()> callback);
	void Invoke();
	int CallbackCount();

private:
	std::vector<std::function<void()>> callbacks;
};