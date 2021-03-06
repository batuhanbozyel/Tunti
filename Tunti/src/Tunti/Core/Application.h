#pragma once
#include "Tunti/Events/ApplicationEvent.h"
#include "Tunti/Events/KeyEvent.h"
#include "Tunti/Events/MouseEvent.h"

#include "Base.h"
#include "Window.h"
#include "LayerStack.h"

int main(int argc, char** argv);

namespace Tunti
{
	class Application
	{
	public:
		Application(const std::string& appName = "Tunti", const WindowFlag& flag = WindowFlag::CustomWindow);
		~Application();

		void Run();
		void Shutdown();

		// Application Utility Methods
		static void DisableCursor();
		static void EnableCursor();
		static void SetCursorPos(float x, float y);

		static const Scope<Window>& GetWindow() { return s_Instance->m_Window; }
		static Application* GetInstance() { return s_Instance; }
	protected:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);
	private:
		// Application Event Handling Methods
		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;

		bool m_Running = false;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}