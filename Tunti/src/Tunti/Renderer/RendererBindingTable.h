#pragma once

namespace Tunti
{
	namespace RendererBindingTable
	{
		// Texture Bindings

		static constexpr uint8_t PrefilterSpecularCubemapTextureUnit = 0;
		static constexpr uint8_t IrradianceCubemapTextureUnit = 1;
		static constexpr uint8_t BRDF_LUTCubemapTextureUnit = 2;
		static constexpr uint8_t GBufferDepthTextureUnit = 3;
		static constexpr uint8_t GBufferNormalTextureUnit = 4;
		static constexpr uint8_t GBufferAlbedoTextureUnit = 5;
		static constexpr uint8_t GBufferRoughnessMetalnessAOTextureUnit = 6;
		static constexpr uint8_t DirectionalLightShadowMapTextureUnit = 7;
		static constexpr uint8_t EmptyTextureUnitBase = 8;

		// Buffer Bindings

		static constexpr uint8_t CameraUniformBuffer = 0;
		static constexpr uint8_t MaterialIndexBufferShaderStorageBuffer = 1;
		static constexpr uint8_t VertexBufferShaderStorageBuffer = 2;
		static constexpr uint8_t MaterialBufferShaderStorageBuffer = 3;
		static constexpr uint8_t LightsUniformBuffer = 4;
	}

	namespace RendererShaders
	{
		static constexpr char* FullscreenTriangle = "../Tunti/assets/shaders/FullscreenTriangle.glsl";
		static constexpr char* ShadowPass = "../Tunti/assets/shaders/ShadowPass.glsl";
		static constexpr char* GeometryPass = "../Tunti/assets/shaders/GeometryPass.glsl";
		static constexpr char* PBRLightingPass = "../Tunti/assets/shaders/PBRLightingPass.glsl";
		static constexpr char* SkyboxPass = "../Tunti/assets/shaders/Skybox.glsl";

		static constexpr char* EquirectangularToCubemap = "../Tunti/assets/shaders/EquirectangularToCubemap_Compute.glsl";
		static constexpr char* PrefilteredSpecularEnvironmentMap = "../Tunti/assets/shaders/PrefilteredSpecularEnvironmentMap_Compute.glsl";
		static constexpr char* IrradianceCubemap = "../Tunti/assets/shaders/IrradianceCubemap_Compute.glsl";
		static constexpr char* BRDF_LUTTexture = "../Tunti/assets/shaders/BRDF_LUTTexture_Compute.glsl";
	}

	namespace RendererConstants
	{
		static constexpr uint32_t MaximumLightNumber = 100;
		static constexpr char* DefaultEnvironmentMap = "../Tunti/assets/textures/environmentmaps/default.hdr";
	}
}