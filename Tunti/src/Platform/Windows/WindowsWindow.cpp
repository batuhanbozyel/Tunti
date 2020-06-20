#include "tpch.h"
#include "WindowsWindow.h"

#include "Tunti/Events/ApplicationEvent.h"
#include "Tunti/Events/MouseEvent.h"
#include "Tunti/Events/KeyEvent.h"

#include "Tunti/Renderer/GraphicsContext.h"

namespace Tunti
{
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		T_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		
		m_Context.reset(GraphicsContext::Create((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr));
		m_Context->Init();

		glfwSetWindowUserPointer((GLFWwindow*)m_Context->GetWindowHandle(), &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, int width, int height)
		{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
		});

		glfwSetWindowCloseCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback((GLFWwindow*)m_Context->GetWindowHandle(), [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow((GLFWwindow*)m_Context->GetWindowHandle());
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled ^ m_Data.VSync)
		{
			m_Data.VSync = enabled;
			glfwSwapInterval(enabled);
			T_WARN(enabled ? "VSync Enabled" : "VSync Disabled");
		}
	}

	bool WindowsWindow::IsVsync() const
	{
		return m_Data.VSync;
	}

}