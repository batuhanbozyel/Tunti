#pragma once

#include "Base.h"
#include "Window.h"
#include "LayerStack.h"

#include "Tunti/Events/Event.h"
#include "Tunti/Events/ApplicationEvent.h"

#include "Tunti/ImGui/ImGuiLayer.h"

namespace Tunti
{
	class TUNTI_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();
}

