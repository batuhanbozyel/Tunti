#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "SceneSettings.h"

#include "Tunti/Renderer/Renderer.h"
#include "Tunti/Utility/EditorCamera.h"

namespace Tunti
{
	Scene::Scene()
	{
		Entity directionalLight = CreateEntity("Directional Light");
		directionalLight.AddComponent<LightComponent>();
		directionalLight.GetComponent<TransformComponent>().Rotation = glm::vec3{-45.0f, 0.0f, 0.0f};
	}

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
		const auto [mainCamera, transform] = GetPrimaryCamera();

		if (mainCamera)
		{
			SubmitLights();
			SubmitMeshes();

			glm::mat4 cameraView = glm::inverse(glm::translate(glm::mat4(1.0f), transform.Position) * glm::toMat4(glm::quat(glm::radians(transform.Rotation))));
			Texture2D frame = Renderer::GetRenderPipeline()->Execute(ShaderCameraContainer{ cameraView, mainCamera->GetProjection(), transform.Position });
			Renderer::OutputToScreenFramebuffer(frame);
		}
	}

	Texture2D Scene::OnUpdateEditor(double dt, const EditorCamera& camera)
	{
		SubmitLights();
		SubmitMeshes();
		return Renderer::GetRenderPipeline()->Execute(ShaderCameraContainer{ camera.GetViewMatrix(), camera.GetProjection(), camera.GetPosition() });
	}

	std::pair<const Camera*, TransformComponent> Scene::GetPrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);

			if (camera.Primary)
			{
				return std::make_pair(&camera.Camera, transform);
			}
		}
		return std::make_pair(nullptr, TransformComponent());
	}

	void Scene::SubmitLights()
	{
		auto view = m_Registry.view<TransformComponent, LightComponent>();
		for (auto entity : view)
		{
			auto& [light, transform] = view.get<LightComponent, TransformComponent>(entity);

			if (light.Type == LightType::DirectionalLight)
			{
				glm::vec3 direction = transform.GetForwardDirection();
				glm::mat4 viewProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 1.0f, 10000.0f)
					* glm::lookAt(-direction * 8000.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				Renderer::GetRenderPipeline()->SubmitDirectionalLight(_DirectionalLight{ viewProjection, direction, light.Color, light.Intensity });
			}
			else if (light.Type == LightType::PointLight)
			{
				Renderer::GetRenderPipeline()->SubmitPointLight(_PointLight{ transform.Position, light.Color, light.Constant, light.Linear, light.Quadratic });
			}
		}
	}

	void Scene::SubmitMeshes()
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
}