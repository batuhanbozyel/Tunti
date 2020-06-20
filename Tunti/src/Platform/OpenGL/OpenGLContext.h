#pragma once

#include <GLFW/glfw3.h>
#include "Tunti/Renderer/GraphicsContext.h"

namespace Tunti
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		virtual void Init() override;
		virtual void SwapBuffers() override;

		inline void* GetWindowHandle() const override { return m_WindowHandle; }
	private:
		OpenGLContext(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
	private:
		GLFWwindow* m_WindowHandle;
		static bool s_GLFWInitialized;
		friend GraphicsContext;
	};

	class GLFWContext
	{
	private:
		static void Init();
		static void GLFWErrorCallback(int error, const char* description);
	private:
		static bool s_GLFWInitialized;
		friend OpenGLContext;
	};
}