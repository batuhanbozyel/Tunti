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
		glm::vec3 Position = glm::vec3(0.0f);
		glm::quat Rotation = glm::quat();
		glm::vec3 Scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Position(translation) {}

		operator glm::mat4() const
		{
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetDirection() const
		{
			return glm::normalize(Rotation * glm::vec3(0.0f, 1.0f, 0.0f));
		}

		void Rotate(const glm::vec3& degrees)
		{
			Rotation = glm::rotate(Rotation, glm::radians(degrees));
		}

		void SetRotation(const glm::vec3& degrees)
		{
			Rotation = glm::rotate(glm::quat(), glm::radians(degrees));
		}

		void SetPosition(const glm::vec3& position)
		{
			Position = position;
		}
	};

	struct MeshRendererComponent
	{
		MeshData MeshBuffer;
		Ref<MaterialInstance> MaterialInstanceRef;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Mesh& mesh, const Ref<MaterialInstance>& materialInstance)
			: MeshBuffer(BufferManager::AllocateGraphicsBuffer(mesh, std::hash<Ref<MaterialInstance>>{}(materialInstance))),
			MaterialInstanceRef(materialInstance) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(uint32_t width, uint32_t height)
			: Camera(width, height) {}
	};

	struct LightComponent
	{
		Light _Light;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(LightType type)
			: _Light(type) {}

		operator const Light& () const
		{
			return _Light;
		}
	};
}
