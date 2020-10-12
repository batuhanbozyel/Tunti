#pragma once
#include <entt.hpp>

namespace Doge
{
	class Entity;
	class Time;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string("Entity"));

		void OnUpdate();
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}