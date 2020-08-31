#pragma once
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Doge
{
	class Input
	{
	public:
		// Keyboard Input
		static bool IsKeyPressed(int keycode);
		static bool IsKeyReleased(int keycode);
		static bool IsKeyRepeated(int keycode);

		// Mouse Input
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePos();
	};
}
