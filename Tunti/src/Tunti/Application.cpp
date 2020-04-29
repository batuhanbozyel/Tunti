#include "tpch.h"
#include "Application.h"

#include "Tunti/Events/ApplicationEvent.h"
#include "Tunti/Log.h"

namespace Tunti
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running) 
		{
			m_Window->OnUpdate();
		}
	}
}

