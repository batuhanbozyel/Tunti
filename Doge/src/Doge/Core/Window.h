#pragma once

#include "Doge/Renderer/Context.h"
#include "Doge/Events/WindowEvent.h"

namespace Doge
{
	enum class WindowFlag
	{
		BorderlessFullscreen,
		ExclusiveFullscreen,
		MaximizedWindow,
		CustomWindow
	};

	struct WindowProps
	{
		WindowProps(const std::string& title = "Doge",
			uint32_t width = 1280, uint32_t height = 720,
			void* monitor = nullptr,
			void* share = nullptr,
			bool vsync = false)
			: Title(title), Width(width), Height(height), Monitor(monitor), Share(share), VSync(vsync)
		{

		}

		std::string Title;
		uint32_t Width, Height;
		void* Monitor;
		void* Share;
		bool VSync;

		std::function<void(Event&)> EventCallback;
	};

	class Window
	{
	public:
		Window(const WindowProps& props = WindowProps(), const WindowFlag& flag = WindowFlag::CustomWindow);
		~Window();

		void OnUpdate();
		void OnWindowResize(WindowResizeEvent& e);

		void SetVSync(bool enabled);

		void HideCursor();
		void ShowCursor();

		inline void SetEventCallbackFn(std::function<void(Event&)> callback) { m_Props.EventCallback = callback; }
		inline bool VSync() const { return m_Props.VSync; }

		inline void* GetNativeWindow() const { return m_Context->GetNativeWindow(); }
		inline const WindowProps& GetWindowProps() const { return m_Props; }
	private:
		void* CreateNativeWindow(const WindowFlag& flag);
	private:
		std::unique_ptr<Context> m_Context;
		WindowProps m_Props;

		static uint8_t s_WindowCount;
	};
}