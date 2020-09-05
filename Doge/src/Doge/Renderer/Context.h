#pragma once

namespace Doge
{
	class Context
	{
	public:
		static std::unique_ptr<Context> Create(void* window);

		virtual void SwapBuffers() const = 0;
		virtual inline void* GetNativeWindow() const = 0;
	};
}