#include "pch.h"
#include "Doge/Core/Platform.h"
#include "Doge/Core/Window.h"

#include "Doge/Renderer/Renderer.h"
#include "Doge/Events/KeyEvent.h"
#include "Doge/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Doge
{
#ifdef PLATFORM_WINDOWS
	uint8_t Window::s_WindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		Log::Error("Error: {0}", description);
	}

	Window::Window(const WindowProps& props, const WindowFlag& flag)
		: m_Props(props)
	{
		if (s_WindowCount == 0)
		{
			int glfw = glfwInit();
			LOG_ASSERT(glfw, "GLFW initialization failed!");

			if (Renderer::GetAPI() == RendererAPI::OpenGL)
			{
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

#ifdef DEBUG_ENABLED
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			}
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		GLFWwindow* windowContext = static_cast<GLFWwindow*>(CreateNativeWindow(flag));
		LOG_ASSERT(windowContext, "Window creation failed");
		m_Context = Context::Create(windowContext);

		glfwSetWindowUserPointer(windowContext, &m_Props);
		glfwSwapInterval(static_cast<int>(props.VSync));

		// KeyPressed, KeyReleased Events
		glfwSetKeyCallback(windowContext, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					props.EventCallback(KeyPressedEvent(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					props.EventCallback(KeyReleasedEvent(key));
					break;
				}

				case GLFW_REPEAT:
				{
					props.EventCallback(KeyPressedEvent(key, 1));
					break;
				}
				}
			});

		// KeyTyped Event
		glfwSetCharCallback(windowContext, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				props.EventCallback(KeyTypedEvent(keycode));
			});

		// MouseButtonPressed, MouseButtonReleased Events
		glfwSetMouseButtonCallback(windowContext, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					props.EventCallback(MouseButtonPressedEvent(button));
					break;
				}
				case GLFW_RELEASE:
				{
					props.EventCallback(MouseButtonReleasedEvent(button));
					break;
				}
				}
			});

		// MouseMoved Event
		glfwSetCursorPosCallback(windowContext, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				props.EventCallback(MouseMovedEvent(static_cast<float>(xPos), static_cast<float>(yPos)));
			});

		// MouseScrolled Event
		glfwSetScrollCallback(windowContext, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				props.EventCallback(MouseScrolledEvent(static_cast<float>(xOffset), static_cast<float>(yOffset)));
			});

		// WindowClose Event
		glfwSetWindowCloseCallback(windowContext, [](GLFWwindow* window)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				props.EventCallback(WindowCloseEvent());
			});

		// WindowResize Event
		glfwSetWindowSizeCallback(windowContext, [](GLFWwindow* window, int width, int height)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);
				props.Width = static_cast<uint32_t>(width);
				props.Height = static_cast<uint32_t>(height);

				props.EventCallback(WindowResizeEvent(props.Width, props.Height));
			});

		// WindowFocus and WindowLostFocus Events
		glfwSetWindowFocusCallback(windowContext, [](GLFWwindow* window, int focused)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				static_cast<bool>(focused) == true ? props.EventCallback(WindowFocusEvent()) : props.EventCallback(WindowLostFocusEvent());
			});

		// WindowMoved Event
		glfwSetWindowPosCallback(windowContext, [](GLFWwindow* window, int xPos, int yPos)
			{
				WindowProps& props = *(WindowProps*)glfwGetWindowUserPointer(window);

				props.EventCallback(WindowMovedEvent(xPos, yPos));
			});
	}

	Window::~Window()
	{
		glfwDestroyWindow(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()));
		s_WindowCount--;

		if (s_WindowCount == 0)
			glfwTerminate();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::OnWindowResize(WindowResizeEvent& e)
	{
		glfwSetWindowSize(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), e.GetWidth(), e.GetHeight());
		m_Props.Width = e.GetWidth();
		m_Props.Height = e.GetHeight();
	}

	void Window::SetVSync(bool enabled)
	{
		if (m_Props.VSync ^ enabled)
		{
			m_Props.VSync = enabled;
			glfwSwapInterval(static_cast<int>(enabled));
		}
	}

	void Window::HideCursor()
	{
		glfwSetInputMode(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	void Window::ShowCursor()
	{
		glfwSetInputMode(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

	void Window::SetCursorPos(float x, float y)
	{
		glfwSetCursorPos(static_cast<GLFWwindow*>(m_Context->GetNativeWindow()), x, y);
	}

	void* Window::CreateNativeWindow(const WindowFlag& flag)
	{
		switch (flag)
		{
		case WindowFlag::BorderlessFullscreen:
		{
			s_WindowCount++;
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			m_Props.Width = mode->width;
			m_Props.Height = mode->height;
			m_Props.Monitor = monitor;

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			return glfwCreateWindow(mode->width, mode->height, m_Props.Title.c_str(), monitor, nullptr);
		}
		case WindowFlag::ExclusiveFullscreen:
		{
			s_WindowCount++;
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			m_Props.Width = mode->width;
			m_Props.Height = mode->height;
			m_Props.Monitor = monitor;

			return glfwCreateWindow(mode->width, mode->height, m_Props.Title.c_str(), monitor, nullptr);
		}
		case WindowFlag::MaximizedWindow:
		{
			s_WindowCount++;
			glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
			GLFWwindow* window = glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);

			int width, height;
			glfwGetWindowSize(window, &width, &height);
			m_Props.Width = width;
			m_Props.Height = height;

			return window;
		}
		case WindowFlag::CustomWindow:
		{
			s_WindowCount++;
			return glfwCreateWindow(m_Props.Width, m_Props.Height, m_Props.Title.c_str(), nullptr, nullptr);
		}
		}

		LOG_ASSERT(nullptr, "Native Window initialization failed!");
		return nullptr;
	}
#else
#error Tunti Engine currently supports Windows only!
#endif
}