#include "pch.h"
#include "Doge.h"
#include "Scene.h"

namespace Doge
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
		glm::mat4 cameraTransform;
		glm::vec3 cameraPosition;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform;
					cameraPosition = transform.Translation;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera, cameraTransform, cameraPosition);

			auto group = m_Registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
			for (auto entity : group)
			{
				auto& [meshRenderer, transform] = group.get<MeshRendererComponent, TransformComponent>(entity);

				Renderer::DrawMesh(meshRenderer.MeshBuffer, meshRenderer.MaterialInstanceRef, transform);
			}

			Renderer::EndScene();
		}
	}
}