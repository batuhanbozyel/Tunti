#pragma once
#include <entt.hpp>

namespace Tunti
{
	struct Texture2D;
	struct TransformComponent;

	class Entity;
	class Camera;
	class EditorCamera;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string("Entity"));
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(double dt);
		Texture2D OnUpdateEditor(double dt, const EditorCamera& editorCamera);

		entt::registry& GetSceneRegistry() { return m_Registry; }
	private:
		std::pair<const Camera*, TransformComponent> GetPrimaryCamera();
		void SubmitLights();
		void SubmitMeshes();
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}