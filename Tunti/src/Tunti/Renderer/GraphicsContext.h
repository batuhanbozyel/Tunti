#pragma once
#include "tpch.h"
#include "Renderer.h"

namespace Tunti
{
	class TUNTI_API GraphicsContext
	{
	public:
		static GraphicsContext* Create(int width, int height, const char* title, void* monitor, void* share);

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void* GetWindowHandle() const = 0;
	private:
	};
}