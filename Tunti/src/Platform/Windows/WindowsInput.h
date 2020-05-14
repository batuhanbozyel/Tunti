#pragma once

#include "Tunti/Input.h"

namespace Tunti
{
	class WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(int keycode) override;

		bool IsMouseButtonPressedImpl(int button) override;
		std::pair<float, float> GetMousePosImpl() override;
	};
}