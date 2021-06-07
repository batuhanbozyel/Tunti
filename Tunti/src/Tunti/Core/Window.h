#pragma once
#include "Tunti/Events/WindowEvent.h"

#ifdef PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
namespace Tunti
{
	struct NativeWindowHandle
	{
		GLFWwindow* Handle;

		NativeWindowHandle() = default;
		NativeWindowHandle(GLFWwindow* handle)
			: Handle(handle)
		{

		}

		operator GLFWwindow* () const
		{
			return Handle;
		}
	};
#else
#error Tunti Engine currently supports Windows only!
#endif

	enum class WindowFlag
	{
		BorderlessFullscreen,
		ExclusiveFullscreen,
		MaximizedWindow,
		CustomWindow
	};

	struct WindowProps
	{
		WindowProps(const std::string& title = "Tunti",
			uint32_t width = 1280, uint32_t height = 720,
			void* monitor = nullptr,
			void* share = nullptr,
			bool vsync = true)
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
		void SetCursorPos(float x, float y);

		void SetEventCallbackFn(std::function<void(Event&)> callback) { m_Props.EventCallback = callback; }
		bool IsVSync() const { return m_Props.VSync; }

		const WindowProps& GetWindowProps() const { return m_Props; }
		const NativeWindowHandle& GetHandle() const;
	private:
		NativeWindowHandle CreateNativeWindow(const WindowFlag& flag);
	private:
		NativeWindowHandle m_Handle;
		WindowProps m_Props;

		static inline uint8_t s_WindowCount = 0;
	};
}