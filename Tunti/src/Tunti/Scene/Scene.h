#pragma once
#include <entt.hpp>

namespace Tunti
{
	class Entity;
	class Time;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string("Entity"));
		void DestroyEntity(Entity entity);

		void OnUpdate();

		entt::registry& GetSceneRegistry() { return m_Registry; }
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}