#pragma once
#include "Doge/Events/ApplicationEvent.h"
#include "Doge/Events/KeyEvent.h"
#include "Doge/Events/MouseEvent.h"

#include "Base.h"
#include "Window.h"
#include "Timestep.h"
#include "LayerStack.h"

int main(int argc, char** argv);

namespace Doge
{
	class Application
	{
	public:
		Application(const std::string& appName = "Doge", const WindowFlag& flag = WindowFlag::CustomWindow);
		~Application();

		static void Run();
		static void Shutdown();

		// Application Utility Methods
		static void DisableCursor();
		static void EnableCursor();
		static void SetCursorPos(float x, float y);

		inline static const Scope<Window>& GetActiveWindow() { return s_Instance->s_ActiveWindow; }
	protected:
		// To be defined in Client
		virtual void OnUpdate(Timestep ts) {};

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
		Scope<Window> s_ActiveWindow;
		LayerStack m_LayerStack;
		Timestep m_Timestep = 0.0f;
		bool s_Running = false;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}
