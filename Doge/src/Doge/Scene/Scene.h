#pragma once
#include <entt.hpp>

namespace Doge
{
	class Entity;
	class Timestep;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string("Entity"));

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}