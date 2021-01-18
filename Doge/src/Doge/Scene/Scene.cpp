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
		{
			auto group = m_Registry.group<TransformComponent, CameraComponent>();
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = camera;
					cameraTransform = transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer::BeginScene(*mainCamera);

			auto group = m_Registry.group<MeshRendererComponent, TransformComponent>();
			for (auto entity : group)
			{
				auto& [meshRenderer, transform] = group.get<MeshRendererComponent, TransformComponent>(entity);

				Renderer::DrawMesh(meshRenderer.MeshData, meshRenderer.MaterialInstanceRef, transform);
			}

			Renderer::EndScene();
		}
	}
}