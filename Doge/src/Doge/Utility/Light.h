#pragma once

namespace Doge
{
	struct Light
	{
		static constexpr uint32_t MaxLightsPerType = 100;

		glm::vec3 Position{ 1.0f };
		glm::vec3 Color{ 1.0f };
		float Linear;
		float Quadratic;
		float Radius;

		Light(const glm::vec3& position)
			: Position(position) {}
	};
}