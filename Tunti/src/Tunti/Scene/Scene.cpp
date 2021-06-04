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
					cameraView = glm::inverse(glm::translate(glm::mat4(1.0f), transform.Position)
						* glm::toMat4(glm::quat(glm::radians(transform.Rotation))));
					cameraPosition = transform.Position;
					break;
				}
			}
		}

		if (mainCamera)
		{
			{
				auto view = m_Registry.view<TransformComponent, LightComponent>();
				for (auto entity : view)
				{
					auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

					if (light.Type == LightType::DirectionalLight)
					{
						glm::vec3 direction = transform.GetForwardDirection();
						glm::mat4 viewProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 1000.0f) * glm::lookAt(
							-direction * 1000.0f,
							glm::vec3(0.0f, 0.0f, 0.0f),
							transform.GetUpDirection()
						);
						Renderer::SubmitDirectionalLight(_DirectionalLight{ viewProjection, direction, light.Color, light.Intensity });
					}
					else if (light.Type == LightType::PointLight)
					{
						Renderer::SubmitPointLight(_PointLight{ transform.Position, light.Color, light.Constant, light.Linear, light.Quadratic });
					}
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
				for (auto entity : view)
				{
					auto& [meshRenderer, transform] = view.get<MeshRendererComponent, TransformComponent>(entity);

					if (meshRenderer._Mesh.IsValid)
					{
						Renderer::DrawMesh(meshRenderer._Mesh, { meshRenderer.Submeshes, meshRenderer.Materials }, transform);
					}
				}
			}
			Renderer::DrawScene(*mainCamera, cameraView, cameraPosition);
		}
	}

	void Scene::OnUpdateEditor(double dt, const EditorCamera& camera)
	{
		const glm::mat4& cameraView = camera.GetViewMatrix();
		{
			auto view = m_Registry.view<TransformComponent, LightComponent>();
			for (auto entity : view)
			{
				auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

				if (light.Type == LightType::DirectionalLight)
				{
					glm::vec3 direction = transform.GetForwardDirection();
					glm::mat4 viewProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 1000.0f) * glm::lookAt(
						-direction * 1000.0f,
						glm::vec3(0.0f, 0.0f, 0.0f),
						transform.GetUpDirection()
					);
					Renderer::SubmitDirectionalLight(_DirectionalLight{ viewProjection, direction, light.Color, light.Intensity });
				}
				else if (light.Type == LightType::PointLight)
				{
					Renderer::SubmitPointLight(_PointLight{ transform.Position, light.Color, light.Constant, light.Linear, light.Quadratic });
				}
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
			for (auto entity : view)
			{
				auto& [meshRenderer, transform] = view.get<MeshRendererComponent, TransformComponent>(entity);

				if (meshRenderer._Mesh.IsValid)
				{
					Renderer::DrawMesh(meshRenderer._Mesh, { meshRenderer.Submeshes, meshRenderer.Materials }, transform);
				}
			}
		}
		Renderer::DrawScene(camera, cameraView, camera.GetPosition());
	}
}