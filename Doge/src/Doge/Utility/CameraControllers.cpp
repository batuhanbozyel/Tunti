#include "pch.h"
#include "Doge.h"
#include "CameraControllers.h"

namespace Doge
{
	void FirstPersonCameraController::OnStart(Scene& scene, float verticalFov, float nearClip, float farClip)
	{
		uint32_t width = Application::GetActiveWindow()->GetWindowProps().Width;
		uint32_t height = Application::GetActiveWindow()->GetWindowProps().Height;

		m_FpsCameraEntity = scene.CreateEntity("First Person Camera");
		m_FpsCameraEntity.AddComponent<CameraComponent>(width, height).Camera.SetPerspective(verticalFov, nearClip, farClip);
		m_FpsCameraEntity.GetComponent<TransformComponent>().Translation = glm::vec3(0.0f, 0.0f, 8.0f);

		m_MouseLastX = (float)width / 2.0f;
		m_MouseLastY = (float)height / 2.0f;

		Application::DisableCursor();
	}

	void FirstPersonCameraController::OnUpdate()
	{
		TransformComponent& transform = m_FpsCameraEntity.GetComponent<TransformComponent>();

		/* Mouse Movement */
		auto[mouseX, mouseY] = Input::GetMousePos();

		m_Yaw += (m_MouseLastX - mouseX) * m_MouseSensitivity * Time::DeltaTime();
		m_Pitch += (m_MouseLastY - mouseY) * m_MouseSensitivity * Time::DeltaTime();

		m_MouseLastX = mouseX;
		m_MouseLastY = mouseY;

		m_Pitch = glm::clamp(m_Pitch, -80.0f, 80.0f);

		transform.Rotation = glm::rotate(glm::quat(), glm::radians(glm::vec3(m_Pitch, m_Yaw, 0.0f)));
		/* Mouse Movement */
	}

	void FirstPersonCameraController::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_FpsCameraEntity.GetComponent<CameraComponent>().Camera.SetViewportSize(width, height);
	}
}