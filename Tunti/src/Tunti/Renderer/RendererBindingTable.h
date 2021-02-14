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
		static constexpr char* GeometryPass = "../Tunti/shaders/GeometryPass.glsl";
		static constexpr char* PBRDeferredPass = "../Tunti/shaders/PBRDeferredPass.glsl";

		static constexpr char* Skybox = "../Tunti/shaders/Skybox.glsl";
		static constexpr char* TexturedQuad = "../Tunti/shaders/TexturedQuad.glsl";
		static constexpr char* ObjectOutlining = "../Tunti/shaders/ObjectOutlining.glsl";
	}
}