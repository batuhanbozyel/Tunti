#include "GameLayer.h"

namespace Sandbox
{
	GameLayer::GameLayer()
	{
		ConstructDummyScene();
	}

	void GameLayer::OnUpdate()
	{
		m_Scene.OnUpdate();
	}

	void GameLayer::OnEvent(Doge::Event& e)
	{
		Doge::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Doge::KeyPressedEvent>(BIND_EVENT_FN(OnKeyPress));
		dispatcher.Dispatch<Doge::MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPress));
	}

	void GameLayer::ConstructDummyScene()
	{
#if 0
		Doge::Ref<Doge::Model> sponzaModel = Doge::ModelLibrary::Load("assets/models/sponza/sponza.obj");
		std::vector<Doge::Entity> sponzaEntity(sponzaModel->Meshes.size());
		for (uint32_t i = 0; i < sponzaModel->Meshes.size(); i++)
		{
			sponzaEntity[i] = m_Scene.CreateEntity(std::string("Sponza_") + std::to_string(i));
			sponzaEntity[i].AddComponent<Doge::MeshRendererComponent>(sponzaModel->Meshes[i], sponzaModel->MaterialInstances[i]);
		}
#elif 0
		Doge::Ref<Doge::Model> backpackModel = Doge::ModelLibrary::Load("assets/models/backpack/backpack.obj");
		std::vector<Doge::Entity> backpackEntity(backpackModel->Meshes.size());
		for (uint32_t i = 0; i < backpackModel->Meshes.size(); i++)
		{
			backpackEntity[i] = m_Scene.CreateEntity(std::string("Backpack_" + std::to_string(i)));
			backpackEntity[i].AddComponent<Doge::MeshRendererComponent>(backpackModel->Meshes[i], backpackModel->MaterialInstances[i]);
		}
#else
		{
			Doge::Ref<Doge::Model> model = Doge::ModelLibrary::Load("../TuntiEditor/assets/primitives/cube.fbx");
			std::vector<Doge::Entity> entity(model->Meshes.size());
			for (uint32_t i = 0; i < model->Meshes.size(); i++)
			{
				entity[i] = m_Scene.CreateEntity("Cube");
				entity[i].AddComponent<Doge::MeshRendererComponent>(model->Meshes[i], model->MaterialInstances[i]);
				entity[i].GetComponent<Doge::TransformComponent>().Translation = glm::vec3(-2.0f, -1.25f, 0.0f);
				entity[i].GetComponent<Doge::TransformComponent>().Rotation = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
			}
		}
		{
			Doge::Ref<Doge::Model> model = Doge::ModelLibrary::Load("../TuntiEditor/assets/primitives/sphere.fbx");
			std::vector<Doge::Entity> entity(model->Meshes.size());
			for (uint32_t i = 0; i < model->Meshes.size(); i++)
			{
				entity[i] = m_Scene.CreateEntity("Sphere");
				entity[i].AddComponent<Doge::MeshRendererComponent>(model->Meshes[i], model->MaterialInstances[i]);
				entity[i].GetComponent<Doge::TransformComponent>().Translation = glm::vec3(2.0f, -1.25f, 0.0f);
				entity[i].GetComponent<Doge::TransformComponent>().Rotation = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
			}
		}
		{
			Doge::Ref<Doge::Model> model = Doge::ModelLibrary::Load("../TuntiEditor/assets/primitives/cylinder.fbx");
			std::vector<Doge::Entity> entity(model->Meshes.size());
			for (uint32_t i = 0; i < model->Meshes.size(); i++)
			{
				entity[i] = m_Scene.CreateEntity("Cylinder");
				entity[i].AddComponent<Doge::MeshRendererComponent>(model->Meshes[i], model->MaterialInstances[i]);
				entity[i].GetComponent<Doge::TransformComponent>().Translation = glm::vec3(-2.0f, 1.25f, 0.0f);
				entity[i].GetComponent<Doge::TransformComponent>().Rotation = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
			}
		}
		{
			Doge::Ref<Doge::Model> model = Doge::ModelLibrary::Load("../TuntiEditor/assets/primitives/cone.fbx");
			std::vector<Doge::Entity> entity(model->Meshes.size());
			for (uint32_t i = 0; i < model->Meshes.size(); i++)
			{
				entity[i] = m_Scene.CreateEntity("Cone");
				entity[i].AddComponent<Doge::MeshRendererComponent>(model->Meshes[i], model->MaterialInstances[i]);
				entity[i].GetComponent<Doge::TransformComponent>().Translation = glm::vec3(2.0f, 1.25f, 0.0f);
				entity[i].GetComponent<Doge::TransformComponent>().Rotation = glm::radians(glm::vec3(-90.0f, 0.0f, 0.0f));
			}
		}
#endif
		Doge::Entity sceneCameraEntity = m_Scene.CreateEntity("SceneCamera");
		sceneCameraEntity.AddComponent<Doge::CameraComponent>(
			Doge::PerspectiveCamera(60.0f, Doge::Application::GetActiveWindow()->GetWindowProps().Width,
										   Doge::Application::GetActiveWindow()->GetWindowProps().Height, glm::vec3(0.0f, 0.0f, -8.0f)));
		Doge::CameraComponent& sceneCameraComponent = sceneCameraEntity.GetComponent<Doge::CameraComponent>();
		sceneCameraComponent.Primary = true;

		Doge::Log::Warn("Dummy scene constructed!");
	}

	bool GameLayer::OnKeyPress(Doge::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
			case Doge::Input::Key::Escape:
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
			case Doge::Input::Mouse::ButtonRight:
			{
				m_IsMouseVisible ? Doge::Application::DisableCursor() : Doge::Application::EnableCursor();
				m_IsMouseVisible = !m_IsMouseVisible;
				return true;
			}
		}
		return false;
	}
}