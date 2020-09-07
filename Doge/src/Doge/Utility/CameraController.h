#pragma once
#include "Camera.h"
#include "Doge/Events/Event.h"
#include "Doge/Events/WindowEvent.h"

namespace Doge
{
	class CameraController
	{
	public:
		virtual ~CameraController() = default;

		virtual void OnEvent(Event& e) = 0;
		virtual void OnUpdate(float dt) = 0;

		virtual const Camera& GetCamera() const = 0;
	protected:
		virtual bool OnWindowResize(WindowResizeEvent& e) = 0;
	};

	class OrthographicCameraController : public CameraController
	{
	public:
		OrthographicCameraController(float width, float height,
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f));
		~OrthographicCameraController() = default;

		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(float dt) override;

		virtual const OrthographicCamera& GetCamera() const override { return m_Camera; }
	private:
		virtual bool OnWindowResize(WindowResizeEvent& e) override;
	private:
		OrthographicCamera m_Camera;
	};

	class PerspectiveCameraController : public CameraController
	{
	public:
		PerspectiveCameraController(float fov, float width, float height,
									const glm::vec3& position = glm::vec3(0.0f, 0.0f, 2.0f));
		~PerspectiveCameraController() = default;

		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(float dt) override;

		virtual const PerspectiveCamera& GetCamera() const override { return m_Camera; }
	private:
		virtual bool OnWindowResize(WindowResizeEvent& e) override;
	private:
		PerspectiveCamera m_Camera;

		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;

		float m_Speed = 0.004f;
		float m_Sensitivity = 0.05f;
		std::pair<float, float> m_LastMousePos;
	};
}