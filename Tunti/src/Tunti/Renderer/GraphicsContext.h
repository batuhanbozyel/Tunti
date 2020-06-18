#pragma once
#include "tpch.h"

namespace Tunti
{
	class TUNTI_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void* GetWindowHandle() const = 0;
	private:
	};
}