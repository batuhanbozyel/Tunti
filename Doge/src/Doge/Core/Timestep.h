#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_LastFrameTime(time) {}

		inline float DeltaTime()
		{
			float time = static_cast<float>(glfwGetTime());
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;
			return m_DeltaTime * 1000.0f;
		}

		inline float GetDeltaTime() { return m_DeltaTime * 1000.0f; }
	private:
		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;
	};
}