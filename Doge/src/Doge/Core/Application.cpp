#include "pch.h"
#include "Application.h"

#include "Doge/Renderer/Renderer.h"
#include "Doge/Utility/Camera.h"

namespace Doge
{
	constexpr RendererMode RenderMode = RendererMode::_3D;
	// Application Initialization and Shutdown Methods
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& appName)
	{
		LOG_ASSERT(s_Instance == nullptr, "Application already exists!");
		s_Instance = this;
		Log::Init();

		Window* window = new Window(appName);
		window->SetEventCallbackFn(BIND_EVENT_FN(OnEvent));
		m_ActiveWindow = window;

		Renderer::Init(RenderMode, m_ActiveWindow->GetWindowProps());

		LOG_TRACE("Application started running!");
	}

	Application::~Application()
	{
		LOG_TRACE("Application terminating!");
		delete m_ActiveWindow;
		Context::GLFWTerminate();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_ActiveWindow->GetNativeWindow()))
		{
			float dt = m_FrameTime.DeltaTime();

			s_Instance->OnUpdate(dt);

			m_LayerStack.OnUpdate(dt);

			m_ActiveWindow->OnUpdate();
		}
	}

	void Application::Shutdown() const
	{
		glfwSetWindowShouldClose(m_ActiveWindow->GetNativeWindow(), GLFW_TRUE);
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
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPress));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		glfwSetWindowShouldClose(m_ActiveWindow->GetNativeWindow(), GLFW_TRUE);
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		glfwSetWindowSize(m_ActiveWindow->GetNativeWindow(), e.GetWidth(), e.GetHeight());
		m_ActiveWindow->OnWindowResize(e);

		Renderer::GetCamera()->SetProjection(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
		Renderer::SetViewport(0, 0, e.GetWidth(), e.GetHeight());

		LOG_TRACE(e.ToString());
		return true;
	}

	bool Application::OnKeyPress(KeyPressedEvent& e)
	{
		return true;
	}

	bool Application::OnMouseButtonPress(MouseButtonPressedEvent& e)
	{
		return true;
	}

	bool Application::OnMouseMove(MouseMovedEvent& e)
	{
		return true;
	}

	void Application::DisableCursor()
	{
		glfwSetInputMode(m_ActiveWindow->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_ActiveWindow->GetNativeWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	void Application::EnableCursor()
	{
		glfwSetInputMode(m_ActiveWindow->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_ActiveWindow->GetNativeWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

}