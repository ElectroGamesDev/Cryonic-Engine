#include "InputSystem.h"
#include "RaylibInputWrapper.h"

// Keyboard
bool Keyboard::IsKeyPressed(KeyboardKey key)
{
	return IsKeyPressedWrapper(static_cast<int>(key));
}

bool Keyboard::IsKeyReleased(KeyboardKey key)
{
	return IsKeyReleasedWrapper(static_cast<int>(key));
}

bool Keyboard::IsKeyDown(KeyboardKey key)
{
	return IsKeyDownWrapper(static_cast<int>(key));
}


// Mouse
bool Mouse::IsButtonPressed(MouseButton button)
{
	return IsMouseButtonPressedWrapper(static_cast<int>(button));
}

bool Mouse::IsButtonReleased(MouseButton button)
{
	return IsMouseButtonReleasedWrapper(static_cast<int>(button));
}

bool Mouse::IsButtonDown(MouseButton button)
{
	return IsMouseButtonDownWrapper(static_cast<int>(button));
}


// Gamepad
bool Gamepad::IsButtonPressed(GamepadButton button)
{
	return IsGamepadButtonPressedWrapper(static_cast<int>(button));
}

bool Gamepad::IsButtonReleased(GamepadButton button)
{
	return IsGamepadButtonReleasedWrapper(static_cast<int>(button));
}

bool Gamepad::IsButtonDown(GamepadButton button)
{
	return IsGamepadButtonDownWrapper(static_cast<int>(button));
}
