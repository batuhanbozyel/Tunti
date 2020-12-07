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
	private:
		GLFWwindow* m_Window;
	};
}