#pragma once
#include <GLFW/glfw3.h>
#include "Doge/Renderer/Context.h"

namespace Doge
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* window);
		~OpenGLContext() = default;

		virtual void SwapBuffers() const override;
		virtual inline void* GetNativeWindow() const { return m_Window; }
	private:
		GLFWwindow* m_Window;
	};
}