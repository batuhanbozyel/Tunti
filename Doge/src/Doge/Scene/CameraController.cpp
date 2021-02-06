#include "pch.h"
#include "CameraController.h"
#include "Doge/Core/Input.h"

namespace Doge
{
	// Orthographic Camera

	OrthographicCameraController::OrthographicCameraController(float width, float height, const glm::vec3& position)
		: m_Camera(width, height, position)
	{

	}

	void OrthographicCameraController::OnEvent(Event& e)
	{

	}

	void OrthographicCameraController::OnUpdate()
	{

	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}

	// Perspective Camera

	PerspectiveCameraController::PerspectiveCameraController(float fov, float width, float height, const glm::vec3& position)
		: m_Camera(fov, width, height, position),
		m_LastMousePos(width / 2.0f, height / 2.0f)
	{
		m_Camera.SetFrontVector(m_Yaw, m_Pitch);
		m_Camera.RecalculateViewMatrix();
	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
	}

	void PerspectiveCameraController::OnUpdate()
	{
		// Camera movement
		if (Input::IsKeyPressed(Input::Key::W))
			m_Camera.SetPosition(m_Camera.GetPosition() + (Time::DeltaTime() * m_Speed * m_Camera.GetFrontVector()));
			
		if (Input::IsKeyPressed(Input::Key::S))
			m_Camera.SetPosition(m_Camera.GetPosition() - (Time::DeltaTime() * m_Speed * m_Camera.GetFrontVector()));

		if (Input::IsKeyPressed(Input::Key::A))
			m_Camera.SetPosition(m_Camera.GetPosition() - (Time::DeltaTime() * m_Speed * glm::normalize(glm::cross(m_Camera.GetFrontVector(), m_Camera.GetUpVector()))));

		if (Input::IsKeyPressed(Input::Key::D))
			m_Camera.SetPosition(m_Camera.GetPosition() + (Time::DeltaTime() * m_Speed * glm::normalize(glm::cross(m_Camera.GetFrontVector(), m_Camera.GetUpVector()))));

		// Camera rotation
		std::pair<float, float> mousePos = Input::GetMousePos();
		m_Yaw += (mousePos.first - m_LastMousePos.first) * m_Sensitivity;
		m_Pitch += (m_LastMousePos.second - mousePos.second) * m_Sensitivity;

		m_LastMousePos = mousePos;

		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		m_Camera.SetFrontVector(m_Yaw, m_Pitch);

		m_Camera.RecalculateViewMatrix();
	}

	bool PerspectiveCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		m_Camera.SetProjection(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
		return false;
	}
}