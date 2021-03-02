#pragma once
#include "Tunti/Scene/Entity.h"

namespace Tunti
{
	class Scene;
	struct TransformComponent;

	class FirstPersonCameraController
	{
	public:
		FirstPersonCameraController() = default;
		~FirstPersonCameraController() = default;

		void OnStart(Scene& scene, float verticalFov = 60.0f, float nearClip = 0.01f, float farClip = 1000.0f);
		void OnUpdate(double dt);

		void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
		void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
		void SetViewportSize(uint32_t width, uint32_t height);
	private:
		float m_MovementSpeed = 0.01f;
		float m_MouseSensitivity = 0.1f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_MouseLastX = 0.0f, m_MouseLastY = 0.0f;

		TransformComponent* m_Transform = nullptr;
		Entity m_FpsCameraEntity;
	};
}