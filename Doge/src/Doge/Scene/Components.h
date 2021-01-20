#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Doge/Renderer/BufferManager.h"

namespace Doge
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 & translation)
			: Translation(translation) {}

		operator glm::mat4 () const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct MeshRendererComponent
	{
		MeshData MeshBuffer;
		Ref<MaterialInstance> MaterialInstanceRef;

		MeshRendererComponent(const Mesh& mesh, const Ref<MaterialInstance>& materialInstance)
			: MeshBuffer(BufferManager::AllocateGraphicsBuffer(mesh, std::hash<Ref<MaterialInstance>>{}(materialInstance))),
			MaterialInstanceRef(materialInstance) {}
	};

	struct CameraComponent
	{
		bool Primary = false;
		Scope<Camera> SceneCamera;

		operator const Camera* () const { return SceneCamera.get(); }
	};

	struct LightComponent
	{
		Light LightObject;

		LightComponent() = default;
		LightComponent(const glm::vec3& position)
			: LightObject(position) {}
 	};
}
