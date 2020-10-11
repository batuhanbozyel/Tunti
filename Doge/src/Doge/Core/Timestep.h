#pragma once
#include <GLFW/glfw3.h>

namespace Doge
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time) {}

		// Returns in seconds
		operator float() const { return m_Time; }

		inline float GetMilliseconds() const { return m_Time * 1000.0f; }
		inline float GetSeconds() const { return m_Time; }
	private:
		float m_Time = 0.0f;
	};
}