#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Tunti
{
	class TUNTI_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in Client
	Application* CreateApplication();
}

