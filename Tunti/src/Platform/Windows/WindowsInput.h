#pragma once

#include "Tunti/Core/Input.h"

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