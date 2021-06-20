#pragma once

namespace Tunti
{
	namespace SceneSettings
	{
		struct ShadowMap
		{
			static inline uint32_t Resolution = 1024;
			static inline float MaxShadowDistance = 100.0f;
			static inline float CascadeNearPlaneOffset = -15.0f;
			static inline float CascadeFarPlaneOffset = 15.0f;
		};

		struct Lighting
		{
			static inline float EnvironmentMapIntensity = 0.5f;
		};
	}
}