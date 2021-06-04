#pragma once

namespace Tunti
{
	enum class LightType : uint32_t
	{
		DirectionalLight = 0,
		PointLight = 1,
		SpotLight = 2,
		AreaLight = 3
	};

#pragma pack(push, 1)
	struct _DirectionalLight
	{
		glm::mat4 ViewProjection{ 1.0f };
		glm::vec4 DirectionAndIntensity{ 1.0f, 1.0f, 1.0f, 0.0f };
		glm::vec4 Color{ 0.0f };

		_DirectionalLight() = default;
		_DirectionalLight(const glm::mat4& viewProj, const glm::vec3& direction, const glm::vec3& color, float intensity)
			: ViewProjection(viewProj), DirectionAndIntensity(direction, intensity), Color(color, 1.0f) {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct _PointLight
	{
		glm::vec4 Position{ 0.0f };
		glm::vec4 Color{1.0f};
		glm::vec4 ConstantLinearQuadratic{1.0f, 0.09f, 0.032f, 0.0f};

		_PointLight() = default;
		_PointLight(const glm::vec3& position, const glm::vec3& color, float constant, float linear, float quadratic)
			: Position(position, 1.0f), Color(color, 1.0f), ConstantLinearQuadratic(constant, linear, quadratic, 1.0f) {}
	};
#pragma pack(pop)
}