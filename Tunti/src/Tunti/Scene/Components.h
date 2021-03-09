#pragma once
#include "SceneCamera.h"

#include "Tunti/Renderer/Light.h"
#include "Tunti/Renderer/Material.h"
#include "Tunti/Renderer/BufferManager.h"

#include "Tunti/Utility/ModelLibrary.h"

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
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Position(translation) {}

		operator glm::mat4() const
		{
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(glm::quat(glm::radians(Rotation)))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(glm::quat(glm::radians(Rotation)))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::quat GetOrientation() const
		{
			return glm::quat(glm::radians(Rotation));
		}

		glm::vec3 GetUpDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::vec3 GetRightDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glm::vec3 GetFrontDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
		}
	};

	struct MeshRendererComponent
	{
		MeshRenderer MeshBuffer;
		Ref<MaterialInstance> MaterialInstanceRef = Material::DefaulMaterialInstance();
		Ref<Model> ModelRef = nullptr;

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
