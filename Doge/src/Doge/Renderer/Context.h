#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Window;

	class Context
	{
	public:
		Context(GLFWwindow* window);
		~Context();

		static void GLFWInit();
		static void GLFWTerminate();
		static void MakeCurrent(GLFWwindow* window);
	protected:
		void PollEvents();
		void SwapBuffers();

		inline GLFWwindow* SetWindow(GLFWwindow* window) { m_Window = window; }
		inline GLFWwindow* GetNativeWindow() const { return m_Window; }
	private:
		GLFWwindow* m_Window;
		static bool s_Initialized;
		friend Window;
	};
}