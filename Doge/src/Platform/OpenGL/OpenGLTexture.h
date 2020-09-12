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
		virtual uint64_t GetTextureHandle();
	private:
		uint32_t m_ID;
	};
}