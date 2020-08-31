#include "pch.h"
#include "Input.h"
#include "Application.h"

#include <GLFW/glfw3.h>

namespace Doge
{
	bool Input::IsKeyPressed(int keycode)
	{
		int state = glfwGetKey(Application::GetInstance()->GetActiveWindow()->GetNativeWindow(), keycode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyReleased(int keycode)
	{
		int state = glfwGetKey(Application::GetInstance()->GetActiveWindow()->GetNativeWindow(), keycode);
		return state == GLFW_RELEASE;
	}

	bool Input::IsKeyRepeated(int keycode)
	{
		int state = glfwGetKey(Application::GetInstance()->GetActiveWindow()->GetNativeWindow(), keycode);
		return state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		int state = glfwGetMouseButton(Application::GetInstance()->GetActiveWindow()->GetNativeWindow(), button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		double xpos, ypos;
		glfwGetCursorPos(Application::GetInstance()->GetActiveWindow()->GetNativeWindow(), &xpos, &ypos);
		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}
}