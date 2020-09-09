#include "GameLayer.h"

namespace Sandbox
{
	GameLayer::GameLayer()
		: m_CameraController(60.0f, Doge::Application::GetActiveWindow()->GetWindowProps().Width, Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		std::shared_ptr<Doge::Shader> shader = Doge::ShaderLibrary::CreateShader("assets/shaders/PhongLighting.glsl");

		std::shared_ptr<Doge::Material> material = std::make_shared<Doge::Material>(*shader);
		material->SetBaseColor(glm::vec3(1.0f));
		material->SetBaseShininess(32.0f);

		Doge::Model backpack("assets/models/backpack/backpack.obj");
		Doge::Model handgun("assets/models/gun/M1911_01.obj");

		Doge::RenderData backpackData = Doge::RenderDataManager::ConstructBatched(backpack.GetMeshes(), material, glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));
		for (uint32_t i = 0; i < 10; i++)
			for (uint32_t j = 0; j < 10; j++)
			{
				backpackData.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(i * 4.0f, j * 4.0f, 0.0f));
				m_RenderDatas.push_back(backpackData);
			}

		Doge::Application::DisableCursor();
	}

	void GameLayer::OnUpdate(float dt)
	{
		Doge::RendererCommands::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Doge::RendererCommands::Clear();

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
			Doge::Application::GetInstance()->Shutdown();
			return true;
		}
		}
		return false;
	}

	bool GameLayer::OnMouseButtonPress(Doge::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Doge::Mouse::Button2:
		{
			m_IsMouseVisible ? Doge::Application::DisableCursor() : Doge::Application::EnableCursor();
			m_IsMouseVisible = !m_IsMouseVisible;
			return true;
		}
		}
		return false;
	}
}