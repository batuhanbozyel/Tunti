#pragma once
#include <GLFW/glfw3.h>

namespace Tunti
{
	class Time
	{
	public:
		Time()
		{
			if (!s_Instance)
				s_Instance = this;
		}
		~Time()
		{
			s_Instance = nullptr;
		}

		static void OnTick()
		{
			double time = glfwGetTime();
			s_Instance->m_DeltaTime = (time - s_Instance->m_LastFrameTime) * 1000.0;
			s_Instance->m_LastFrameTime = time;
		}

		static double DeltaTime()
		{
			return s_Instance->m_DeltaTime;
		}

		Time(const Time& other) = delete;
		Time& operator=(const Time& other) = delete;
	private:
		double m_LastFrameTime = 0.0f;
		double m_DeltaTime = 0.0f;

		static inline Time* s_Instance = nullptr;
	};
}