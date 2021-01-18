#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Time
	{
	public:
		static void OnTick()
		{
			float time = static_cast<float>(glfwGetTime());
			s_Instance->m_DeltaTime = (time - s_Instance->m_LastFrameTime) * 1000.0f;
			s_Instance->m_LastFrameTime = time;
		}

		static float DeltaTime()
		{
			return s_Instance->m_DeltaTime;
		}

		Time(const Time& other) = delete;
		Time& operator=(const Time& other) = delete;
	private:
		Time() = default;
		~Time()
		{
			s_Instance = nullptr;
		}
	private:
		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;

		static inline Time* s_Instance = nullptr;
	};
}