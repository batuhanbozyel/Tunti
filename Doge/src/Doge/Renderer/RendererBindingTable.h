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
		static constexpr uint8_t TransformMatrixUniformBuffer = 1;
		static constexpr uint8_t TextureMapsShaderStorageBuffer = 2;
		static constexpr uint8_t LightsUniformBuffer = 3;
	}

	namespace OpenGLRendererShaders
	{
		static constexpr char* GeometryPass = "../Doge/assets/shaders/GeometryPass.glsl";
		static constexpr char* LightingPass = "../Doge/assets/shaders/LightingPass.glsl";

		static constexpr char* Skybox = "../Doge/assets/shaders/Skybox.glsl";
		static constexpr char* TexturedQuad = "../Doge/assets/shaders/TexturedQuad.glsl";
		static constexpr char* ObjectOutlining = "../Doge/assets/shaders/ObjectOutlining.glsl";
	}
}