#include "pch.h"
#include "Camera.h"

namespace Doge
{
	Camera::Camera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position)
		: m_ProjectionMatrix(projection), m_ViewMatrix(view), m_Position(position)
	{

	}

	// Orthographic Camera

	OrthographicCamera::OrthographicCamera(float width, float height, const glm::vec3& position)
		: Camera(glm::ortho(0.0f, width, 0.0f, height), glm::mat4(1.0f), position)
	{

	}

	void OrthographicCamera::Update()
	{

	}

	void OrthographicCamera::Move(int keyCode, float dt)
	{

	}

	void OrthographicCamera::SetProjection(float width, float height)
	{
		m_ProjectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f);
	}

	// Perspective Camera

	PerspectiveCamera::PerspectiveCamera(float fov, float width, float height,
		const glm::vec3& position,
		const glm::vec3& front,
		const glm::vec3& up)
		:
		Camera(glm::perspective(glm::radians(fov), width / height, 0.01f, 100.0f), glm::lookAt(position, position + front, up), position),
		m_Fov(fov), m_AspectRatio(width / height),
		m_Up(up), m_Front(front),
		m_Yaw(-90.0f), m_Pitch(0.0f),
		m_LastMousePos(width / 2.0f, height / 2.0f)
	{

	}

	void PerspectiveCamera::Update()
	{

	}

	void PerspectiveCamera::Move(int keyCode, float dt)
	{
		constexpr float speed = 0.004f;
		switch (keyCode)
		{
		case KEY_W:
			m_Position += speed * m_Front * dt;
			break;
		case KEY_S:
			m_Position -= speed * m_Front * dt;
			break;
		case KEY_A:
			m_Position -= speed * glm::normalize(glm::cross(m_Front, m_Up)) * dt;
			break;
		case KEY_D:
			m_Position += speed * glm::normalize(glm::cross(m_Front, m_Up)) * dt;
			break;
		}
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void PerspectiveCamera::Rotate(const std::pair<float, float>& mousePos)
	{
		constexpr float sensitivity = 0.05f;

		m_Yaw += (mousePos.first - m_LastMousePos.first) * sensitivity;
		m_Pitch += (m_LastMousePos.second - mousePos.second) * sensitivity;

		m_LastMousePos = mousePos;

		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		m_Front = glm::normalize(glm::vec3(
			glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch)),
			glm::sin(glm::radians(m_Pitch)),
			glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch))
		));
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void PerspectiveCamera::SetProjection(float width, float height)
	{
		if (height == 0.0f) height = 0.0001f;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), width / height, 0.01f, 100.0f);
	}

	const glm::vec3 PerspectiveCamera::GetViewDirection() const
	{
		return glm::vec3(-m_Front.x, -m_Front.y, m_Front.z);
	}
}