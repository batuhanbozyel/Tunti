#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Time
	{
	public:
		static void Init()
		{
			if(!s_Instance)
				s_Instance = CreateScope<Time>();
		}

		static void Tick()
		{
			float time = static_cast<float>(glfwGetTime());
			s_Instance->m_DeltaTime = (time - s_Instance->m_LastFrameTime) * 1000.0f;
			s_Instance->m_LastFrameTime = time;
		}

		static float DeltaTime() { return s_Instance->m_DeltaTime; }
	private:
		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;

		static Scope<Time> s_Instance;
	};
}