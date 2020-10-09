#include "GameLayer.h"

namespace Sandbox
{
	GameLayer::GameLayer()
		: m_CameraController(60.0f, Doge::Application::GetActiveWindow()->GetWindowProps().Width,
									Doge::Application::GetActiveWindow()->GetWindowProps().Height)
	{
		Doge::Ref<Doge::CubemapTexture> dummyCubemap = Doge::TextureManager::LoadCubemap("assets/cubemaps/learnopengl/",
			"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg");

		Doge::Renderer::SetSkybox(dummyCubemap);

		Doge::Ref<Doge::Shader> shader = Doge::LightingShader::PhongLighting();

		Doge::Ref<Doge::Material> material = Doge::CreateRef<Doge::Material>(shader);
		material->SetBaseColor({ 1.0f, 1.0f, 1.0f });
		material->SetBaseShininess(32.0f);
		material->SetModifiable(Doge::MaterialProperty::Color);

		Doge::Ref<Doge::MaterialInstance> defaultMaterialInstance = Doge::CreateRef<Doge::MaterialInstance>(material);

		Doge::Ref<Doge::MaterialInstance> blueMaterialInstance = Doge::CreateRef<Doge::MaterialInstance>(material);
		blueMaterialInstance->SetColor({ 0.2f, 0.3f, 0.8f });

		Doge::Ref<Doge::MaterialInstance> greenMaterialInstance = Doge::CreateRef<Doge::MaterialInstance>(material);
		greenMaterialInstance->SetColor({ 0.3f, 0.8f, 0.2f });

		Doge::Cube cube(glm::vec3(1.0f));
		Doge::RenderData blueCubeData = Doge::RenderDataManager::Construct(cube.GetMesh(), blueMaterialInstance);
		blueCubeData.Selected = true;
		m_RenderDatas.push_back(blueCubeData);

		Doge::RenderData greenCubeData = Doge::RenderDataManager::Construct(cube.GetMesh(), greenMaterialInstance);
		greenCubeData.ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
		m_RenderDatas.push_back(greenCubeData);

		Doge::Model backpack("assets/models/backpack/backpack.obj");
		Doge::RenderData backpackData = Doge::RenderDataManager::ConstructBatched(backpack.GetMeshes(), defaultMaterialInstance);
		backpackData.ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
		m_RenderDatas.push_back(backpackData);

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