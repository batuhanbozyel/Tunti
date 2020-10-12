#pragma once
#include "Scene.h"

namespace Doge
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene)
			: m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			LOG_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			LOG_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LOG_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}
	private:
		entt::entity m_EntityHandle;
		Scene* m_Scene;
	};
}