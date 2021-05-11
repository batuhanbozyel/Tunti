#pragma once
#include <entt.hpp>

namespace Tunti
{
	class EditorCamera;
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string("Entity"));
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(double dt);
		void OnUpdateEditor(double dt, const EditorCamera& editorCamera);

		entt::registry& GetSceneRegistry() { return m_Registry; }
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}