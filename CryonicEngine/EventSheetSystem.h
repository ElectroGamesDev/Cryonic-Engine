#pragma once

#include <string>
#include <vector>
#include <any>

namespace EventSheetSystem
{
	// I'm using custom ints so if something is removed, it doesn't break event sheets saved data since they use ints to store the enums.
	enum class Conditions {
		None = 0,

		// Input
		AnyKeyPressed = 1,
		AnyKeyReleased = 2,
		KeyPressed = 3,
		KeyReleased = 4,
		KeyDown = 5,

		MouseButtonPressed = 6,
		MouseButtonReleased = 7,
		MouseButtonDown = 8,
		MouseMoved = 9,
		MouseInZone = 10,

		GamepadButtonPressed = 11,
		GamepadButtonReleased = 12,
		GamepadButtonDown = 13,
		GamepadAxisMoved = 14
	};

	enum class Actions {
		None = 0,

		// Position
		MovePosition = 1,
		XPosition = 2,
		YPosition = 3,
		Position = 4,
	};

	struct Param {
		std::string name;
		std::string type;
		std::any value;
	};

	struct Condition {
		Conditions type;
		std::string stringType;
		std::string category;
		std::vector<Param> params;
	};

	struct Action {
		Actions type;
		std::string stringType;
		std::string category;
		std::vector<Param> params;
	};

	struct Event {
		std::vector<Condition> conditions;
		std::vector<Action> actions;
	};
}