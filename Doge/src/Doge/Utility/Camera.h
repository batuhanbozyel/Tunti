#pragma once
#include <glm/glm.hpp>

namespace Doge
{
	class Camera
	{
	public:
		virtual ~Camera() = default;

		virtual void Move(int keyCode, float dt) {}
		virtual void Rotate(const std::pair<float, float>& mousePos) {}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		inline void SetPosition(const glm::vec3& position) { m_Position = position; }

		// To be defined in Inherited Camera classes
		virtual void SetProjection(float left, float right, float bottom, float top) {}
		virtual void SetProjection(float width, float height) {}

		virtual const glm::vec3 GetViewDirection() const { return m_Position; }
	protected:
		Camera(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position);
		virtual void Update() = 0;

	protected:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position;
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float width, float height,
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));
		virtual ~OrthographicCamera() = default;

		virtual void Move(int keyCode, float dt) override;

		virtual void SetProjection(float width, float height) override;
	private:
		virtual void Update() override;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float width, float height,
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 2.0f),
			const glm::vec3& front = glm::vec3(0.0f, 0.0f, -1.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
		virtual ~PerspectiveCamera() = default;

		virtual void Move(int keyCode, float dt) override;
		virtual void Rotate(const std::pair<float, float>& mousePos) override;

		virtual void SetProjection(float width, float height) override;

		virtual const glm::vec3 GetViewDirection() const override;
	private:
		virtual void Update() override;
	private:
		glm::vec3 m_Front;
		glm::vec3 m_Up;

		float m_Yaw;
		float m_Pitch;

		float m_Fov;
		float m_AspectRatio;

		std::pair<float, float> m_LastMousePos;
	};
}