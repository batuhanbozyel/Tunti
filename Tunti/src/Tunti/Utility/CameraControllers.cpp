#include "pch.h"

#include "Tunti/Core/Input.h"
#include "Tunti/Core/Application.h"

#include "Tunti/Scene/Entity.h"
#include "Tunti/Scene/Components.h"

#include "CameraControllers.h"

namespace Tunti
{
	void FirstPersonCameraController::OnStart(Scene& scene, float verticalFov, float nearClip, float farClip)
	{
		uint32_t width = Application::GetWindow()->GetWindowProps().Width;
		uint32_t height = Application::GetWindow()->GetWindowProps().Height;

		m_FpsCameraEntity = scene.CreateEntity("First Person Camera");
		m_FpsCameraEntity.AddComponent<CameraComponent>(width, height).Camera.SetPerspective(verticalFov, nearClip, farClip);
		m_Transform = &m_FpsCameraEntity.GetComponent<TransformComponent>();
		m_Transform->Position = glm::vec3(0.0f, 0.0f, 8.0f);

		m_MouseLastX = (float)width / 2.0f;
		m_MouseLastY = (float)height / 2.0f;

		Application::SetCursorPos(m_MouseLastX, m_MouseLastY);
		Application::DisableCursor();
	}

	void FirstPersonCameraController::OnUpdate(double dt)
	{
		/* Camera Rotation */
		auto[mouseX, mouseY] = Input::GetMousePos();

		m_Yaw += (m_MouseLastX - mouseX) * m_MouseSensitivity;
		m_Pitch += (m_MouseLastY - mouseY) * m_MouseSensitivity;

		m_MouseLastX = mouseX;
		m_MouseLastY = mouseY;

		m_Pitch = glm::clamp(m_Pitch, -80.0f, 80.0f);

		m_Transform->Rotation = glm::vec3(m_Pitch, m_Yaw, 0.0f);
		/* Camera Rotation */
		
		/* Camera Movement */
		glm::quat orientation = m_Transform->GetOrientation();
		glm::vec3 forward = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 right = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));

		float velocity = dt * m_MovementSpeed;

		if (Input::IsKeyPressed(Input::Key::LeftShift))
			velocity *= 2.5f;

		if (Input::IsKeyPressed(Input::Key::LeftControl))
			velocity /= 2.5f;

		if (Input::IsKeyPressed(Input::Key::W))
			m_Transform->Position += forward * velocity;

		if (Input::IsKeyPressed(Input::Key::S))
			m_Transform->Position -= forward * velocity;

		if (Input::IsKeyPressed(Input::Key::A))
			m_Transform->Position -= right * velocity;

		if (Input::IsKeyPressed(Input::Key::D))
			m_Transform->Position += right * velocity;
		/* Camera Movement */
	}

	void FirstPersonCameraController::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_FpsCameraEntity.GetComponent<CameraComponent>().Camera.SetViewportSize(width, height);
	}
}