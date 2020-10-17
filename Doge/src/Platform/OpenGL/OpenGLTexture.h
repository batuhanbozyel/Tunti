#pragma once
#include "Doge/Renderer/Texture.h"

namespace Doge
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		explicit OpenGLTexture2D();
		explicit OpenGLTexture2D(const std::string& texturePath);
		~OpenGLTexture2D();
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