#pragma once
#include <glm/glm.hpp>

namespace Doge
{
	class Camera
	{
	public:
		virtual ~Camera() = default;

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		virtual void RecalculateViewMatrix() = 0;

		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		// To be defined in Inherited Camera classes
		virtual void SetProjection(float width, float height) = 0;

		virtual inline glm::vec3 CalculateViewDirection() const { return m_Position; }
	protected:
		Camera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position);
	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position;
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float width, float height,
			const glm::vec3& position);
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

		virtual inline glm::vec3 CalculateViewDirection() const override { return glm::vec3(-m_Front.x, -m_Front.y, m_Front.z); };

		inline const glm::vec3& GetFrontVector() const { return m_Front; }
		inline const glm::vec3& GetUpVector() const { return m_Up; }
	private:
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		float m_Fov;
		float m_AspectRatio;
	};
}