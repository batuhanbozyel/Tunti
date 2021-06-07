#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "SceneSettings.h"

#include "Tunti/Renderer/Renderer.h"
#include "Tunti/Utility/EditorCamera.h"

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
		const SceneCamera* mainCamera = nullptr;
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
						glm::mat4 viewProjection = glm::ortho(
							SceneSettings::ShadowMap::CascadeNearPlaneOffset, SceneSettings::ShadowMap::CascadeFarPlaneOffset,
							SceneSettings::ShadowMap::CascadeNearPlaneOffset, SceneSettings::ShadowMap::CascadeFarPlaneOffset,
							1.0f, SceneSettings::ShadowMap::MaxShadowDistance)
							* glm::lookAt(-direction * 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
						Renderer::GetRenderPipeline()->SubmitDirectionalLight(_DirectionalLight{ viewProjection, direction, light.Color, light.Intensity });
					}
					else if (light.Type == LightType::PointLight)
					{
						Renderer::GetRenderPipeline()->SubmitPointLight(_PointLight{ transform.Position, light.Color, light.Constant, light.Linear, light.Quadratic });
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
						Renderer::GetRenderPipeline()->SubmitMesh(meshRenderer._Mesh, { meshRenderer.Submeshes, meshRenderer.Materials }, transform);
					}
				}
			}
			Texture2D frame = Renderer::GetRenderPipeline()->Execute(ShaderCameraContainer{ cameraView, mainCamera->GetProjection(), cameraPosition });
			Renderer::OutputToScreenFramebuffer(frame);
		}
	}

	Texture2D Scene::OnUpdateEditor(double dt, const EditorCamera& camera)
	{
		{
			auto view = m_Registry.view<TransformComponent, LightComponent>();
			for (auto entity : view)
			{
				auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

				if (light.Type == LightType::DirectionalLight)
				{
					glm::vec3 direction = transform.GetForwardDirection();
					float shadowDistance = SceneSettings::ShadowMap::MaxShadowDistance;
					float halfShadowDistance = shadowDistance * 0.5f;
					glm::mat4 viewProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f)
						* glm::lookAt(-direction * 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					Renderer::GetRenderPipeline()->SubmitDirectionalLight(_DirectionalLight{ viewProjection, direction, light.Color, light.Intensity });
				}
				else if (light.Type == LightType::PointLight)
				{
					Renderer::GetRenderPipeline()->SubmitPointLight(_PointLight{ transform.Position, light.Color, light.Constant, light.Linear, light.Quadratic });
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
					Renderer::GetRenderPipeline()->SubmitMesh(meshRenderer._Mesh, { meshRenderer.Submeshes, meshRenderer.Materials }, transform);
				}
			}
		}
		return Renderer::GetRenderPipeline()->Execute(ShaderCameraContainer{ camera.GetViewMatrix(), camera.GetProjection(), camera.GetPosition() });
	}
}