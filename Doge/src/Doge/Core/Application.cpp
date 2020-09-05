#include "pch.h"
#include "Application.h"

#include "Doge/Renderer/Renderer.h"
#include "Doge/Renderer/RendererCommands.h"
#include "Doge/Utility/Camera.h"

namespace Doge
{
	// Application Initialization and Shutdown Methods
	Application* Application::s_Instance = nullptr;
	Window* Application::s_ActiveWindow = nullptr;

	Application::Application(const std::string& appName, const WindowFlag& flag)
	{
		LOG_ASSERT(s_Instance == nullptr, "Application already exists!");
		s_Instance = this;
		Log::Init();

		Renderer::SetAPI(Doge::RendererAPI::OpenGL);
		Window* window = new Window(WindowProps(appName), flag);
		window->SetEventCallbackFn(BIND_EVENT_FN(OnEvent));
		s_ActiveWindow = window;

		Renderer::Init(s_ActiveWindow->GetWindowProps());

		LOG_TRACE("Application started running!");
	}

	Application::~Application()
	{
		LOG_TRACE("Application terminating!");
		delete s_ActiveWindow;
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(s_ActiveWindow->GetNativeWindow()))
		{
			float dt = m_FrameTime.DeltaTime();

			s_Instance->OnUpdate(dt);

			m_LayerStack.OnUpdate(dt);

			s_ActiveWindow->OnUpdate();
		}
	}

	void Application::Shutdown() const
	{
		glfwSetWindowShouldClose(s_ActiveWindow->GetNativeWindow(), GLFW_TRUE);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
	}

	// Application Event Handling Methods

	void Application::OnEvent(Event& e)
	{
		for (auto it = m_LayerStack.GetOverlays().rbegin(); it != m_LayerStack.GetOverlays().rend(); it++)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}

		for (auto it = m_LayerStack.GetLayers().rbegin(); it != m_LayerStack.GetLayers().rend(); it++)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		glfwSetWindowShouldClose(s_ActiveWindow->GetNativeWindow(), GLFW_TRUE);
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		glfwSetWindowSize(s_ActiveWindow->GetNativeWindow(), e.GetWidth(), e.GetHeight());
		s_ActiveWindow->OnWindowResize(e);

		RendererCommands::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

		LOG_TRACE(e.ToString());
		return true;
	}

	void Application::DisableCursor()
	{
		glfwSetInputMode(s_ActiveWindow->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(s_ActiveWindow->GetNativeWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	void Application::EnableCursor()
	{
		glfwSetInputMode(s_ActiveWindow->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(s_ActiveWindow->GetNativeWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

}