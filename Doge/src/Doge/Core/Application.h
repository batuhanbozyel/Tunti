#pragma once
#include "Doge/Events/ApplicationEvent.h"
#include "Doge/Events/KeyEvent.h"
#include "Doge/Events/MouseEvent.h"

#include "Window.h"
#include "Timestep.h"
#include "LayerStack.h"

int main(int argc, char** argv);

namespace Doge
{
	class Application
	{
	public:
		Application(const std::string& appName = "Doge");
		~Application();

		void Run();
		void Shutdown() const;
		// To be defined in Client
		virtual void OnUpdate(float dt) {};

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline static const Window* GetActiveWindow() { return s_ActiveWindow; }
		inline static const Application* GetInstance() { return s_Instance; }

		// Application Utility Methods
		static void DisableCursor();
		static void EnableCursor();
	private:
		// Application Event Handling Methods
		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		bool OnKeyPress(KeyPressedEvent& e);
		bool OnMouseButtonPress(MouseButtonPressedEvent& e);
		bool OnMouseMove(MouseMovedEvent& e);
	private:
		static Window* s_ActiveWindow;
		LayerStack m_LayerStack;
		Timestep m_FrameTime = 0.0f;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}
