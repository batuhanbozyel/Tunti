#pragma once
#include "Doge/Scene/Entity.h"

namespace Doge
{
	class Scene;

	class FirstPersonCameraController
	{
	public:
		FirstPersonCameraController() = default;

		void OnStart(Scene& scene, float verticalFov = 60.0f, float nearClip = 0.1f, float farClip = 1000.0f);
		void OnUpdate();

		void SetViewportSize(uint32_t width, uint32_t height);
	private:
	private:
		float m_MouseSensitivity = 0.05f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_MouseLastX = 0.0f, m_MouseLastY = 0.0f;

		Entity m_FpsCameraEntity;
	};
}