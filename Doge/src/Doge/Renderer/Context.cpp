#include "pch.h"
#include "Context.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Doge
{
	bool Context::s_Initialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_ERROR("Error: {0}", description);
	}

	std::shared_ptr<Context> Context::Create(GLFWwindow* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, ""); return nullptr;
		case RendererAPI::OpenGL: return std::make_shared<OpenGLContext>(window);
		}
		LOG_ASSERT(false, "");
		return nullptr;
	}

	void Context::GLFWInit()
	{
		if (!s_Initialized)
		{
			int glfw = glfwInit();
			LOG_ASSERT(glfw, "GLFW initialization failed!");
			s_Initialized = true;

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef DEBUG_ENABLED
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

			glfwSetErrorCallback(GLFWErrorCallback);
		}
	}

	void Context::GLFWTerminate()
	{
		if (s_Initialized)
		{
			glfwTerminate();
			s_Initialized = false;
		}
	}

	void Context::MakeCurrent(GLFWwindow* window)
	{
		glfwMakeContextCurrent(window);
		LOG_TRACE("Current Context assigned to ID: {0}", static_cast<void*>(window));
	}

	void Context::PollEvents()
	{
		glfwPollEvents();
	}

	void Context::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}
}