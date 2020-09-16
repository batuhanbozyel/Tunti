#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	class Context
	{
	public:
		static Scope<Context> Create(void* window);

		virtual void SwapBuffers() const = 0;
		virtual inline void* GetNativeWindow() const = 0;
	};
}