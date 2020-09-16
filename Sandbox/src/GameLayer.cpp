#include "GameLayer.h"

namespace Sandbox
{
	GameLayer::GameLayer()
		: m_CameraController(60.0f, Doge::Application::GetActiveWindow()->GetWindowProps().Width, Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		Doge::Ref<Doge::Shader> shader = Doge::ShaderLibrary::CreateShader("assets/shaders/PhongLighting.glsl");

		Doge::Ref<Doge::Material> material = Doge::CreateRef<Doge::Material>(*shader);
		material->SetBaseColor(glm::vec3(1.0f));
		material->SetBaseShininess(32.0f);

		Doge::Cuboid cube(glm::vec3(1.0f));
		Doge::RenderData cubeData = Doge::RenderDataManager::Construct(cube.GetMesh(), material);
		cubeData.Selected = true;
		m_RenderDatas.push_back(cubeData);

		Doge::Application::DisableCursor();
	}

	void GameLayer::OnUpdate(float dt)
	{
		m_CameraController.OnUpdate(dt);

		for (const auto& renderData : m_RenderDatas)
		{
			Doge::Renderer::Submit(renderData);
		}

		Doge::Renderer::RenderIndexed(m_CameraController.GetCamera());
	}

	void GameLayer::OnEvent(Doge::Event& e)
	{
		Doge::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Doge::KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
		dispatcher.Dispatch<Doge::MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPress));
	}

	bool GameLayer::OnKeyPress(Doge::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Doge::Key::Escape:
		{
			Doge::Application::Shutdown();
			return true;
		}
		}
		return false;
	}

	bool GameLayer::OnMouseButtonPress(Doge::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Doge::Mouse::ButtonRight:
		{
			m_IsMouseVisible ? Doge::Application::DisableCursor() : Doge::Application::EnableCursor();
			m_IsMouseVisible = !m_IsMouseVisible;
			return true;
		}
		}
		return false;
	}
}