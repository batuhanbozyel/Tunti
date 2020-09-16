#include "pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include "stb_image/stb_image.h"

namespace Doge
{
	// OpenGLTexture

	OpenGLTexture::OpenGLTexture(const std::string& texturePath)
	{
		stbi_set_flip_vertically_on_load(1);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		int width, height, channels;
		unsigned char* buffer = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
		if (buffer)
		{
			glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureStorage2D(m_ID, 1, GL_RGBA8, width, height);
			glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

			uint64_t handle = glGetTextureHandleARB(m_ID);
			glMakeTextureHandleResidentARB(handle);
		}
		stbi_image_free(buffer);
	}

	OpenGLTexture::OpenGLTexture()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		unsigned char buffer[4] = { 255, 255, 255, 255 };

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_ID, 1, GL_RGBA8, 1, 1);
		glTextureSubImage2D(m_ID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		uint64_t handle = glGetTextureHandleARB(m_ID);
		glMakeTextureHandleResidentARB(handle);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		uint64_t handle = glGetTextureHandleARB(m_ID);
		glMakeTextureHandleNonResidentARB(handle);

		glDeleteTextures(1, &m_ID);
	}

	uint64_t OpenGLTexture::GetTextureHandle()
	{
		return glGetTextureHandleARB(m_ID);
	}
}