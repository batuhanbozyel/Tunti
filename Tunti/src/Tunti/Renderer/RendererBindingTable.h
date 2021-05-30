#pragma once

namespace Tunti
{
	namespace RendererBindingTable
	{
		// Texture Bindings

		static constexpr uint8_t SkyboxTextureUnit = 0;
		static constexpr uint8_t GBufferPositionTextureUnit = 1;
		static constexpr uint8_t GBufferNormalTextureUnit = 2;
		static constexpr uint8_t GBufferAlbedoSpecularTextureUnit = 3;
		static constexpr uint8_t IrradianceCubemapTextureUnit = 4;
		static constexpr uint8_t BRDF_LUTCubemapTextureUnit = 5;

		// Buffer Bindings

		static constexpr uint8_t ViewProjectionUniformBuffer = 0;
		static constexpr uint8_t MaterialIndexBufferShaderStorageBuffer = 1;
		static constexpr uint8_t VertexBufferShaderStorageBuffer = 2;
		static constexpr uint8_t MaterialBufferShaderStorageBuffer = 3;
		static constexpr uint8_t LightsUniformBuffer = 4;
	}

	namespace RendererShaders
	{
		static constexpr char* ShadowPass = "../Tunti/assets/shaders/ShadowPass.glsl";
		static constexpr char* GeometryPass = "../Tunti/assets/shaders/GeometryPass.glsl";
		static constexpr char* PBRLightingPass = "../Tunti/assets/shaders/PBRLightingPass.glsl";

		static constexpr char* EquirectangularToCubemap = "../Tunti/assets/shaders/EquirectangularToCubemap_Compute.glsl";
		static constexpr char* PrefilteredSpecularEnvironmentMap = "../Tunti/assets/shaders/PrefilteredSpecularEnvironmentMap_Compute.glsl";
		static constexpr char* IrradianceCubemap = "../Tunti/assets/shaders/IrradianceCubemap_Compute.glsl";
		static constexpr char* BRDF_LUTTexture = "../Tunti/assets/shaders/BRDF_LUTTexture_Compute.glsl";

		static constexpr char* Skybox = "../Tunti/assets/shaders/Skybox.glsl";
		static constexpr char* FullscreenTriangle = "../Tunti/assets/shaders/FullscreenTriangle.glsl";
		static constexpr char* ObjectOutlining = "../Tunti/assets/shaders/ObjectOutlining.glsl";
	}

	namespace RendererConstants
	{
		static constexpr uint32_t MaximumLightNumber = 100;
		static constexpr char* DefaultEnvironmentMap = "../Tunti/assets/textures/environmentmaps/default.hdr";
	}
}