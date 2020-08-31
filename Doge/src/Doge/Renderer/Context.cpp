#include "pch.h"
#include <glad/glad.h>
#include "Context.h"

namespace Doge
{
	bool Context::s_Initialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_ERROR("Error: {0}", description);
	}

	static void OpenGLMessageCallback(
		unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         LOG_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          LOG_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_TRACE(message); return;
		}

	}

	Context::Context(GLFWwindow* window)
		: m_Window(window)
	{
		GLFWInit();
		LOG_ASSERT(window, "Window could not found!");

		glfwMakeContextCurrent(m_Window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOG_ASSERT(status, "Glad initialization failed");

#ifdef DEBUG_ENABLED
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
		glEnable(GL_MULTISAMPLE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		LOG_INFO(glGetString(GL_RENDERER));
		LOG_INFO(glGetString(GL_VERSION));

		LOG_TRACE("Context creation succeed!");
	}

	Context::~Context()
	{
		m_Window = nullptr;
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