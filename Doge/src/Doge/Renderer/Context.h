#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Window;

	class Context
	{
	public:
		static std::shared_ptr<Context> Create(GLFWwindow* window);

		static void GLFWInit();
		static void GLFWTerminate();
		static void MakeCurrent(GLFWwindow* window);
	protected:
		void PollEvents();
		void SwapBuffers();

		inline GLFWwindow* GetNativeWindow() const { return m_Window; }
	protected:
		GLFWwindow* m_Window;
		static bool s_Initialized;
		friend Window;
	};
}