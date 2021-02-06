#include "pch.h"
#include "Doge/Core/Input.h"
#include "Doge/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Doge
{
#ifdef PLATFORM_WINDOWS
	namespace Input
	{
		bool IsKeyPressed(int keycode)
		{
			int state = glfwGetKey(static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow()), keycode);
			return state == GLFW_PRESS;
		}

		bool IsKeyReleased(int keycode)
		{
			int state = glfwGetKey(static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow()), keycode);
			return state == GLFW_RELEASE;
		}

		bool IsKeyRepeated(int keycode)
		{
			int state = glfwGetKey(static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow()), keycode);
			return state == GLFW_REPEAT;
		}

		bool IsMouseButtonPressed(int button)
		{
			int state = glfwGetMouseButton(static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow()), button);
			return state == GLFW_PRESS;
		}

		std::pair<float, float> GetMousePos()
		{
			double xpos, ypos;
			glfwGetCursorPos(static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow()), &xpos, &ypos);
			return { static_cast<float>(xpos), static_cast<float>(ypos) };
		}
	}
#else
#error Tunti Engine currently supports Windows only!
#endif
}