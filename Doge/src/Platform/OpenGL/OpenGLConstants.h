#pragma once

namespace Doge
{
	namespace OpenGLBindings
	{
		// Texture Bindings

		static constexpr uint8_t QuadFramebufferColorAttachment = 0;
		static constexpr uint8_t SkyboxTextureUnit				= 1;

		// Buffer Bindings

		static constexpr uint8_t ViewProjectionUniformBuffer	= 0;
		static constexpr uint8_t ShaderStorageBuffer			= 1;
		static constexpr uint8_t LightingUniformBuffer			= 2;
	}

	namespace OpenGLRendererShaders
	{
		static constexpr char* ObjectOutlining	= "../Doge/assets/shaders/ObjectOutlining.glsl";
		static constexpr char* Skybox			= "../Doge/assets/shaders/Skybox.glsl";
		static constexpr char* TexturedQuad		= "../Doge/assets/shaders/TexturedQuad.glsl";
	}
}