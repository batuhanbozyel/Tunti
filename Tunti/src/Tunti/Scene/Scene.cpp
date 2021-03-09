#include "pch.h"
#include "Scene.h"
#include "Entity.h"

#include "Tunti/Renderer/Renderer.h"
#include "Tunti/Utility/EditorCamera.h"

#include "Components.h"

namespace Tunti
{
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(double dt)
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
					cameraView = glm::lookAt(
						transform.Position,
						transform.Position + transform.GetFrontDirection(),
						transform.GetUpDirection());
					cameraPosition = transform.Position;
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

					Renderer::SubmitLight(light, transform.Position, cameraView * glm::vec4(-transform.GetFrontDirection(), 0.0f));
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

	void Scene::OnUpdateEditor(double dt, const EditorCamera& camera)
	{
		const glm::mat4& cameraView = camera.GetViewMatrix();
		Renderer::BeginScene(camera, cameraView, camera.GetPosition());
		{
			auto view = m_Registry.view<TransformComponent, LightComponent>();
			for (auto entity : view)
			{
				auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

				Renderer::SubmitLight(light, transform.Position, cameraView * glm::vec4(-transform.GetFrontDirection(), 0.0f));
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