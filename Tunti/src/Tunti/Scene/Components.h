#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Tunti/Renderer/BufferManager.h"

namespace Tunti
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
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		operator glm::mat4() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 Transform() const
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
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent(uint32_t width, uint32_t height)
			: Camera(width, height) {}
		CameraComponent(const CameraComponent&) = default;
	};

	struct LightComponent
	{
		LightData Light;

		LightComponent() = default;
		LightComponent(LightType type)
			: Light(type) {}
	};
}
