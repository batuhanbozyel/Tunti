#pragma once

namespace Doge
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
		static constexpr uint8_t VertexBufferShaderStorageBuffer = 1;
		static constexpr uint8_t IndexBufferShaderStorageBuffer = 2;
		static constexpr uint8_t TextureMapsShaderStorageBuffer = 3;
		static constexpr uint8_t LightsUniformBuffer = 4;
	}

	namespace RendererShaders
	{
		static constexpr char* GeometryPass = "../Doge/shaders/GeometryPass.glsl";
		static constexpr char* LightingPass = "../Doge/shaders/LightingPass.glsl";

		static constexpr char* Skybox = "../Doge/shaders/Skybox.glsl";
		static constexpr char* TexturedQuad = "../Doge/shaders/TexturedQuad.glsl";
		static constexpr char* ObjectOutlining = "../Doge/shaders/ObjectOutlining.glsl";
	}
}