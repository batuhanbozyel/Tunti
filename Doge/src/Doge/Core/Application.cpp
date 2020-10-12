#include "pch.h"
#include "Application.h"

#include "Doge/Renderer/Renderer.h"
#include "Doge/Renderer/Framebuffer.h"
#include "Doge/Renderer/RendererCommands.h"
#include "Doge/Utility/Camera.h"

namespace Doge
{
	Scope<Time> Time::s_Instance = nullptr;

	Application* Application::s_Instance = nullptr;
	Application::Application(const std::string& appName, const WindowFlag& flag)
	{
		if (!s_Instance)
		{
			s_Instance = this;
			s_Running = true;
			Log::Init();
			Time::Init();

			Renderer::SetAPI(Doge::RendererAPI::OpenGL);
			Window* window = new Window(WindowProps(appName), flag);
			window->SetEventCallbackFn(BIND_EVENT_FN(OnEvent));
			s_ActiveWindow.reset(window);

			Renderer::Init(s_ActiveWindow->GetWindowProps());
			SetCursorPos(s_ActiveWindow->GetWindowProps().Width / 2.0f, s_ActiveWindow->GetWindowProps().Height / 2.0f);

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
		while (s_Running)
		{
			Time::Tick();

			OnUpdate();

			m_LayerStack.OnUpdate();

			s_ActiveWindow->OnUpdate();
		}
	}

	void Application::Shutdown()
	{
		s_Instance->s_Running = false;
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
		s_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		s_ActiveWindow->OnWindowResize(e);
		Renderer::OnWindowResize(e);
		return true;
	}

	void Application::DisableCursor()
	{
		s_Instance->s_ActiveWindow->HideCursor();
	}

	void Application::EnableCursor()
	{
		s_Instance->s_ActiveWindow->ShowCursor();
	}

	void Application::SetCursorPos(float x, float y)
	{
		s_Instance->s_ActiveWindow->SetCursorPos(x, y);
	}
}