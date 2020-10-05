#pragma once
#include "Doge/Renderer/Texture.h"

namespace Doge
{
	class OpenGLTexture : public Texture
	{
	public:
		explicit OpenGLTexture();
		explicit OpenGLTexture(const std::string& texturePath);
		~OpenGLTexture();
	private:
		virtual uint64_t GetTextureHandle() override;
	private:
		uint32_t m_ID;
	};

	class OpenGLCubemapTexture : public CubemapTexture
	{
	public:
		explicit OpenGLCubemapTexture(const std::array<std::string, 6>& cubemapTexturePaths);
		~OpenGLCubemapTexture();

		virtual void Bind(uint32_t slot) const override;
	private:
		uint32_t m_ID;
	};
}