#include "pch.h"
#include "Tunti.h"
#include "CameraControllers.h"

namespace Tunti
{
	void FirstPersonCameraController::OnStart(Scene& scene, float verticalFov, float nearClip, float farClip)
	{
		uint32_t width = Application::GetActiveWindow()->GetWindowProps().Width;
		uint32_t height = Application::GetActiveWindow()->GetWindowProps().Height;

		m_FpsCameraEntity = scene.CreateEntity("First Person Camera");
		m_FpsCameraEntity.AddComponent<CameraComponent>(width, height).Camera.SetPerspective(verticalFov, nearClip, farClip);
		m_Transform = &m_FpsCameraEntity.GetComponent<TransformComponent>();
		m_Transform->Translation = glm::vec3(0.0f, 0.0f, 8.0f);

		m_MouseLastX = (float)width / 2.0f;
		m_MouseLastY = (float)height / 2.0f;

		Application::DisableCursor();
	}

	void FirstPersonCameraController::OnUpdate()
	{
		/* Camera Rotation */
		auto[mouseX, mouseY] = Input::GetMousePos();

		m_Yaw += (m_MouseLastX - mouseX) * m_MouseSensitivity;
		m_Pitch += (m_MouseLastY - mouseY) * m_MouseSensitivity;

		m_MouseLastX = mouseX;
		m_MouseLastY = mouseY;

		m_Pitch = glm::clamp(m_Pitch, -80.0f, 80.0f);

		m_Transform->Rotation = glm::rotate(glm::quat(), glm::radians(glm::vec3(m_Pitch, m_Yaw, 0.0f)));
		/* Camera Rotation */
		
		/* Camera Movement */
		glm::quat orientation = glm::quat(m_Transform->Rotation);
		glm::vec3 forward = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 right = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));

		float velocity = Time::DeltaTime() * m_MovementSpeed;

		if (Input::IsKeyPressed(Input::Key::LeftShift))
			velocity *= 2.5f;

		if (Input::IsKeyPressed(Input::Key::W))
			m_Transform->Translation += forward * velocity;

		if (Input::IsKeyPressed(Input::Key::S))
			m_Transform->Translation -= forward * velocity;

		if (Input::IsKeyPressed(Input::Key::A))
			m_Transform->Translation -= right * velocity;

		if (Input::IsKeyPressed(Input::Key::D))
			m_Transform->Translation += right * velocity;
		/* Camera Movement */
	}

	void FirstPersonCameraController::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_FpsCameraEntity.GetComponent<CameraComponent>().Camera.SetViewportSize(width, height);
	}
}