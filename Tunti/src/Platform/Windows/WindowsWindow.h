#pragma once

#include "Tunti/Core/Window.h"

#include <GLFW/glfw3.h>

#include "Tunti/Renderer/GraphicsContext.h"

namespace Tunti
{
	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVsync() const override;

		inline void* GetNativeWindow() const override { return m_Context->GetWindowHandle(); }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title = "Tunti Engine";
			unsigned int Width = 1280, Height = 720;
			bool VSync = true;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}