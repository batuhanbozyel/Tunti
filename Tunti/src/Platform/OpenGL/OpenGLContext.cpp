#include "tpch.h"
#include <glad/glad.h>
#include "OpenGLContext.h"

namespace Tunti
{
	bool GLFWContext::s_GLFWInitialized = false;

	void GLFWContext::GLFWErrorCallback(int error, const char* description)
	{
		T_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	void GLFWContext::Init()
	{
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			T_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
	}

	OpenGLContext::OpenGLContext(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
	{
		GLFWContext::Init();
		m_WindowHandle = glfwCreateWindow(width, height, title, monitor, share);
		T_CORE_ASSERT(m_WindowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		T_CORE_ASSERT(status, "Failed to initialize GLAD!");

		T_CORE_INFO("OpenGL Info:");
		T_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		T_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		T_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}