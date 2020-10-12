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

	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, meshRenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);

			Renderer::Submit(meshRenderer.MeshData, meshRenderer.MaterialInstanceRef, transform);
		}
	}
}