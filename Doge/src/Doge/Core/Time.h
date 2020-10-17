#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Time
	{
	public:
		inline void OnTick()
		{
			float time = static_cast<float>(glfwGetTime());
			m_DeltaTime = (time - m_LastFrameTime) * 1000.0f;
			m_LastFrameTime = time;
		}

		static inline float DeltaTime()
		{
			return s_Instance->m_DeltaTime;
		}

		static inline Time& GetInstance()
		{
			if (!s_Instance)
				s_Instance = new Time;

			return *s_Instance;
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

		static Time* s_Instance;
	};
}