#pragma once

namespace Doge
{
	enum class LightType
	{
		DirectionalLight = 0,
		PointLight = 1,
		SpotLight = 2,
		AreaLight = 3
	};
	
	struct LightData
	{
		glm::vec3 Color{ 1.0f };
		float Intensity = 1.0f;
		float Constant = 1.0f;
		float Linear = 0.09f;
		float Quadratic = 0.032f;

		const LightType Type = LightType::DirectionalLight;

		LightData() = default;
		LightData(LightType type)
			: Type(type) {}
	};
}