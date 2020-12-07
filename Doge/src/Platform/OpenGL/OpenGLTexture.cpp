#include "pch.h"
#include <glad/glad.h>
#include "OpenGLTexture.h"
#include "OpenGLConstants.h"

#include "stb_image/stb_image.h"

namespace Doge
{
	// OpenGLTexture

	OpenGLTexture2D::OpenGLTexture2D(const std::string& texturePath)
	{
		stbi_set_flip_vertically_on_load(1);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		int width, height, channels;
		unsigned char* buffer = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
		if (buffer)
		{
			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureStorage2D(m_TextureID, 1, GL_RGBA8, width, height);
			glTextureSubImage2D(m_TextureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

			uint64_t handle = glGetTextureHandleARB(m_TextureID);
			glMakeTextureHandleResidentARB(handle);
		}
		stbi_image_free(buffer);
	}

	OpenGLTexture2D::OpenGLTexture2D()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		unsigned char buffer[4] = { 255, 255, 255, 255 };

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, 1, 1);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		uint64_t handle = glGetTextureHandleARB(m_TextureID);
		glMakeTextureHandleResidentARB(handle);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		uint64_t handle = glGetTextureHandleARB(m_TextureID);
		glMakeTextureHandleNonResidentARB(handle);

		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLCubemapTexture

	OpenGLCubemapTexture::OpenGLCubemapTexture(const std::array<std::string, 6>& cubemapTexturePaths)
	{
		struct CubemapFace
		{
			unsigned char* buffer;
			int width, height, channels;
		} faces[6];

		stbi_set_flip_vertically_on_load(0);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureID);

		for(uint32_t i = 0; i < 6; i++)
			faces[i].buffer = stbi_load(cubemapTexturePaths[i].c_str(), &faces[i].width, &faces[i].height, &faces[i].channels, 4);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, faces[0].height, faces[0].height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (uint32_t i = 0; i < 6; i++)
		{
			CubemapFace face = faces[i];

			if (face.buffer)
				glTextureSubImage3D(m_TextureID, 0, 0, 0, i, face.height, face.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, face.buffer);

			else
				Log::Error("Cubemap Texture failed to load face: {0}", cubemapTexturePaths[i]);

			stbi_image_free(face.buffer);
		}
	}

	OpenGLCubemapTexture::~OpenGLCubemapTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLTextureCache

	OpenGLTextureCache* OpenGLTextureCache::s_Instance = nullptr;

	OpenGLTextureCache::OpenGLTextureCache()
	{
		Scope<OpenGLTexture2D> defaultTexture = CreateScope<OpenGLTexture2D>();
		GLuint64 handle = glGetTextureHandleARB(defaultTexture->m_TextureID);
		m_Textures.insert(std::make_pair("default", defaultTexture));

		m_SSBO = CreateScope<OpenGLShaderStorageBuffer>(SizeofTextureMap * MaxTextures, OpenGLBindings::ShaderStorageBuffer);

		// Create default white texture
		m_SSBO->Bind();
		m_SSBO->SetData(&handle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Diffuse), sizeof(GLuint64));
		m_SSBO->SetData(&handle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Specular), sizeof(GLuint64));
	}

	Texture2D OpenGLTextureCache::AddTexture2D(const std::string& textureFile)
	{
		Texture2D texture;
		GLuint64 handle = 0;
		const auto& textureIt = m_Textures.find(textureFile);

		// if texture already exists
		if (textureIt != m_Textures.end())
			handle = glGetTextureHandleARB(textureIt->second->m_TextureID);
		else
		{
			Scope<OpenGLTexture2D> newTexture = CreateScope<OpenGLTexture2D>(textureFile);
			handle = glGetTextureHandleARB(newTexture->m_TextureID);
			m_Textures.insert(std::make_pair(textureFile, newTexture));
		}

		m_SSBO->Bind();
		m_SSBO->SetData(&handle, SizeofTextureMap * m_TextureCount + OffsetofTextureType(TextureType::Diffuse), sizeof(GLuint64));
		m_SSBO->SetData(&handle, SizeofTextureMap * m_TextureCount + OffsetofTextureType(TextureType::Specular), sizeof(GLuint64));

		texture.Index = m_TextureCount++;
		return texture;
	}

	Texture2D OpenGLTextureCache::AddTexture2D(const std::vector<std::tuple<std::string, TextureType>>& textureFiles)
	{
		Texture2D texture;

		m_SSBO->Bind();
		for (const auto& [textureFile, textureType] : textureFiles)
		{
			GLuint64 handle = 0;
			const auto& textureIt = m_Textures.find(textureFile);

			// if texture already exists
			if (textureIt != m_Textures.end())
				handle = glGetTextureHandleARB(textureIt->second->m_TextureID);
			else
			{
				Scope<OpenGLTexture2D> newTexture = CreateScope<OpenGLTexture2D>(textureFile);
				handle = glGetTextureHandleARB(newTexture->m_TextureID);
				m_Textures.insert(std::make_pair(textureFile, newTexture));
			}

			m_SSBO->SetData(&handle, SizeofTextureMap * m_TextureCount + OffsetofTextureType(textureType), sizeof(GLuint64));
		}

		texture.Index = m_TextureCount++;
		return texture;
	}

	CubemapTexture OpenGLTextureCache::AddCubemap(const std::array<std::string, 6>& cubemapTextures)
	{
		CubemapTexture cubemap;
		const auto& cubemapIt = m_Cubemaps.find(cubemapTextures);

		if (cubemapIt != m_Cubemaps.end())
			cubemap.TextureID = cubemapIt->second->m_TextureID;
		else
		{
			Scope<OpenGLCubemapTexture> newCubemap = CreateScope<OpenGLCubemapTexture>(cubemapTextures);
			m_Cubemaps.insert(std::make_pair(cubemapTextures, newCubemap));
			cubemap.TextureID = newCubemap->m_TextureID;
		}

		return cubemap;
	}
}