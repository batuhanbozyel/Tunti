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
		bool Renaming = false;

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
				* glm::toMat4(GetOrientation())
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

		glm::vec3 GetForwardDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
		}
	};

	struct MeshRendererComponent
	{
		MeshBuffer _Mesh;
		std::vector<SubmeshBuffer> Submeshes;
		std::vector<Ref<MaterialInstance>> Materials;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Model>& model)
			: _Mesh(model->_Mesh), Submeshes(model->Submeshes), Materials(model->Materials) {}
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
		LightType Type = LightType::DirectionalLight;

		glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);

		// Directional Light
		float Intensity = 1.0f;

		// Point Light
		float Constant = 1.0f;
		float Linear = 0.09f;
		float Quadratic = 0.032f;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(LightType type)
			: Type(type) {}
	};
}
