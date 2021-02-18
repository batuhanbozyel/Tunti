#include "pch.h"
#include "Tunti.h"
#include "Scene.h"

namespace Tunti
{
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	void Scene::OnUpdate()
	{
		const Camera* mainCamera = nullptr;
		glm::mat4 cameraView;
		glm::vec3 cameraPosition;
		{
			auto view = m_Registry.view<CameraComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraView = glm::inverse(transform.GetTransform());
					cameraPosition = transform.Translation;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraView, cameraPosition);
			{
				auto view = m_Registry.view<TransformComponent, LightComponent>();
				for (auto entity : view)
				{
					auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

					Renderer::SubmitLight(light, transform.Translation, transform.GetDirection());
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
				for (auto entity : view)
				{
					auto& [meshRenderer, transform] = view.get<MeshRendererComponent, TransformComponent>(entity);

					Renderer::DrawMesh(meshRenderer.MeshBuffer, meshRenderer.MaterialInstanceRef, transform);
				}
			}
			Renderer::EndScene();
		}
	}
}