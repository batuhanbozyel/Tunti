#pragma once
#include <glm/glm.hpp>

namespace Doge
{
	class Camera
	{
	public:
		Camera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position);
		virtual ~Camera() = default;

		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		virtual void RecalculateViewMatrix() = 0;

		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		// To be defined in Inherited Camera classes
		virtual void SetProjection(float width, float height) = 0;
	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position;
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float width, float height, const glm::vec3& position);
		virtual ~OrthographicCamera() = default;

		virtual void RecalculateViewMatrix() override;

		virtual void SetProjection(float width, float height) override;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float width, float height, const glm::vec3& position);
		virtual ~PerspectiveCamera() = default;

		virtual void SetProjection(float width, float height) override;

		virtual void RecalculateViewMatrix() override;

		void SetFrontVector(float yaw, float pitch);
		void SetFieldOfView(float fov);

		const glm::vec3& GetFrontVector() const { return m_Front; }
		const glm::vec3& GetUpVector() const { return m_Up; }
	private:
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		float m_Fov;
		float m_AspectRatio;
	};
}