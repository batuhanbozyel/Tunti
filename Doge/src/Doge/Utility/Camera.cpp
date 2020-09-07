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

	void OrthographicCamera::RecalculateViewMatrix()
	{

	}

	void OrthographicCamera::SetProjection(float width, float height)
	{
		m_ProjectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f);
	}

	// Perspective Camera

	PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, const glm::vec3& position)
		:
		Camera(glm::perspective(glm::radians(fov), width / height, 0.01f, 100.0f), glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)), position),
		m_Fov(fov),	m_Up(glm::vec3(0.0f, 1.0f, 0.0f)), m_Front(glm::vec3(0.0f, 0.0f, 1.0f)), m_AspectRatio(width / height)
	{

	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void PerspectiveCamera::SetProjection(float width, float height)
	{
		if (height == 0.0f) height = 0.0001f;
		m_AspectRatio = width / height;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.01f, 100.0f);
	}

	void PerspectiveCamera::SetFrontVector(float yaw, float pitch)
	{
		m_Front = glm::normalize(glm::vec3(
			glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
			glm::sin(glm::radians(pitch)),
			glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
		));
	}

	void PerspectiveCamera::SetFieldOfView(float fov)
	{
		m_Fov = fov;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.01f, 100.0f);
	}
}