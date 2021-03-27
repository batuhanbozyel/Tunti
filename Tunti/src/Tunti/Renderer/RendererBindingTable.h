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

		// Buffer Bindings

		static constexpr uint8_t ViewProjectionUniformBuffer = 0;
		static constexpr uint8_t TextureMapIndexShaderStorageBuffer = 1;
		static constexpr uint8_t VertexBufferShaderStorageBuffer = 2;
		static constexpr uint8_t TextureMapsShaderStorageBuffer = 3;
		static constexpr uint8_t LightsUniformBuffer = 4;
	}

	namespace RendererShaders
	{
		static constexpr char* ShadowPass = "../Tunti/shaders/ShadowPass.glsl";
		static constexpr char* GeometryPass = "../Tunti/shaders/GeometryPass.glsl";
		static constexpr char* PBRLightingPass = "../Tunti/shaders/PBRLightingPass.glsl";

		static constexpr char* EquirectangularToCubemap = "../Tunti/shaders/EquirectangularToCubemap_Compute.glsl";
		static constexpr char* PrefilteredSpecularEnvironmentMap = "../Tunti/shaders/PrefilteredSpecularEnvironmentMap_Compute.glsl";
		static constexpr char* IrradianceCubemap = "../Tunti/shaders/IrradianceCubemap_Compute.glsl";
		static constexpr char* BRDFto2DLUTTexture = "../Tunti/shaders/BRDFto2DLUTTexture_Compute.glsl";

		static constexpr char* Skybox = "../Tunti/shaders/Skybox.glsl";
		static constexpr char* TexturedQuad = "../Tunti/shaders/TexturedQuad.glsl";
		static constexpr char* ObjectOutlining = "../Tunti/shaders/ObjectOutlining.glsl";
	}

	namespace RendererConstants
	{
		static constexpr uint32_t MaximumLightNumber = 100;
	}
}