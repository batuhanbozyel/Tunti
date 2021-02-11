#include "pch.h"
#include "Application.h"

#include "Tunti/Renderer/Renderer.h"

namespace Tunti
{
	Application::Application(const std::string& appName, const WindowFlag& flag)
	{
		if (!s_Instance)
		{
			s_Instance = this;
			m_Running = true;
			Log::Init();
			m_Timer = CreateScope<Time>();

			WindowProps windowProps(appName);
			Window* window = new Window(windowProps, flag);
			window->SetEventCallbackFn(BIND_EVENT_FN(OnEvent));
			m_ActiveWindow.reset(window);
			Renderer::Init(RendererAPI::OpenGL);

			SetCursorPos(m_ActiveWindow->GetWindowProps().Width / 2.0f, m_ActiveWindow->GetWindowProps().Height / 2.0f);

			Log::Trace("Application started running!");
		}
	}

	Application::~Application()
	{
		Log::Trace("Application terminating!");
		s_Instance = nullptr;
	}

	void Application::Run()
	{
		m_LayerStack.OnStart();
		while (m_Running)
		{
			Time::OnTick();
			m_LayerStack.OnUpdate();
			m_ActiveWindow->OnUpdate();
		}
	}

	void Application::Shutdown()
	{
		m_Running = false;
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
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_ActiveWindow->OnWindowResize(e);
		return true;
	}

	void Application::DisableCursor()
	{
		s_Instance->m_ActiveWindow->HideCursor();
	}

	void Application::EnableCursor()
	{
		s_Instance->m_ActiveWindow->ShowCursor();
	}

	void Application::SetCursorPos(float x, float y)
	{
		s_Instance->m_ActiveWindow->SetCursorPos(x, y);
	}

	Application* Application::s_Instance = nullptr;
}