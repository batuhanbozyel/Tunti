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
	struct Light
	{
		// AttenuationFactors
		float Intensity = 2.0f;
		float Constant = 1.0f;
		float Linear = 0.09f;
		float Quadratic = 0.032f;

		glm::vec3 Color{ 1.0f };
		LightType Type = LightType::DirectionalLight;

		Light() = default;
		Light(const Light&) = default;
		Light(LightType type)
			: Type(type) {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct LightData
	{
		Light _Light;
		glm::vec4 Position{ 0.0f };
		glm::vec3 Direction{ 0.0f };
		float _Padding{ 0.0f };

		LightData() = default;

		LightData(const Light& light, const glm::vec3& position, const glm::vec3& direction)
			: _Light(light), Position(glm::vec4(position, 0.0f)), Direction(direction) {}
	};
#pragma pack(pop)
}