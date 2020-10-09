#pragma once

namespace Doge
{
	enum TextureBinding : uint16_t
	{
		QuadFramebufferColorAttachment = 0,
		SkyboxTextureUnit = 1
	};

	enum BufferBinding : uint16_t
	{
		ViewProjectionUniformBuffer = 1,
		LightingUniformBuffer = 2
	};
}