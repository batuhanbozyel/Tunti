#include "pch.h"
#include "CameraController.h"

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

	void OrthographicCameraController::OnUpdate(float dt)
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

	}

	void PerspectiveCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
	}

	void PerspectiveCameraController::OnUpdate(float dt)
	{
		// Camera movement
		if (Input::IsKeyPressed(Key::W))
			m_Camera.SetPosition(m_Camera.GetPosition() + (m_Speed * m_Camera.GetFrontVector() * dt));
			
		if (Input::IsKeyPressed(Key::S))
			m_Camera.SetPosition(m_Camera.GetPosition() - (m_Speed * m_Camera.GetFrontVector() * dt));

		if (Input::IsKeyPressed(Key::A))
			m_Camera.SetPosition(m_Camera.GetPosition() - (m_Speed * glm::normalize(glm::cross(m_Camera.GetFrontVector(), m_Camera.GetUpVector())) * dt));

		if (Input::IsKeyPressed(Key::D))
			m_Camera.SetPosition(m_Camera.GetPosition() + (m_Speed * glm::normalize(glm::cross(m_Camera.GetFrontVector(), m_Camera.GetUpVector())) * dt));

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