#pragma once

#include <stack>
#include "GameObject.h"
#include "EventSystem.h"

namespace MenuManager
{
	struct Menu
	{
		GameObject* gameObject;
		std::function<void()> closeCallback;
	};

	void Init();
	void OpenMenu(GameObject* menu, std::function<void()> closeCallback = nullptr);
	void CloseTopMenu();
	void CloseAllMenus();
	int GetOpenMenusCount();

	// Only runs when the first menu is opened
	void OnFirstMenuOpened(std::function<void()>& callback);
	void RemoveOnFirstOpen(std::function<void()>& callback);

	// Only runs when the last menu is closed
	void OnLastMenuClosed(std::function<void()>& callback);
	void RemoveOnLastClose(std::function<void()>& callback);
};