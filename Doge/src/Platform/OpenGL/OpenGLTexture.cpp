#include "pch.h"
#include <glad/glad.h>
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "Doge/Renderer/RendererBindingTable.h"

#include "stb_image/stb_image.h"

namespace Doge
{
	// OpenGLTexture

	OpenGLTexture2D::OpenGLTexture2D(const TextureData& data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, data.width, data.height);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, data.width, data.height, GL_RGBA, GL_UNSIGNED_BYTE, data.buffer);

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
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

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glMakeTextureHandleNonResidentARB(m_TextureHandle);
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLCubemapTexture

	OpenGLCubemapTexture::OpenGLCubemapTexture(const std::array<TextureData, 6>& cubemapData)
	{
		stbi_set_flip_vertically_on_load(0);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureID);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, cubemapData[0].height, cubemapData[0].height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (uint32_t i = 0; i < 6; i++)
			glTextureSubImage3D(m_TextureID, 0, 0, 0, i, cubemapData[i].height, cubemapData[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE, cubemapData[i].buffer);
	}

	OpenGLCubemapTexture::~OpenGLCubemapTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLTextureCache

	OpenGLTextureCache* OpenGLTextureCache::s_Instance = nullptr;

	OpenGLTextureCache::OpenGLTextureCache()
	{
		m_DefaultTexture = CreateScope<OpenGLTexture2D>();

		m_SSBO = CreateScope<OpenGLShaderStorageBuffer>(SizeofTextureMap * MaxTextures);

		// Create default white texture
		m_SSBO->Bind(RendererBindingTable::TextureMapsShaderStorageBuffer);
		m_SSBO->SetData(&m_DefaultTexture->m_TextureHandle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Diffuse), sizeof(GLuint64));
		m_SSBO->SetData(&m_DefaultTexture->m_TextureHandle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Specular), sizeof(GLuint64));
	}

	Texture2D OpenGLTextureCache::AddTexture2D(const std::string& textureFile)
	{
		Texture2D texture;
		GLuint64 handle = 0;
		const auto& textureIt = m_Textures.find(textureFile);

		// if texture already exists
		if (textureIt != m_Textures.end())
			handle = textureIt->second->m_TextureHandle;
		else
		{
			stbi_set_flip_vertically_on_load(1);

			TextureData data;
			data.buffer = stbi_load(textureFile.c_str(), &data.width, &data.height, &data.channel, 4);
			if (data.buffer)
			{
				const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
				handle = insertedPairIt->second->m_TextureHandle;
				stbi_image_free(data.buffer);
			}
		}

		m_SSBO->SetData(&handle, SizeofTextureMap * m_TextureCount + OffsetofTextureType(TextureType::Diffuse), sizeof(GLuint64));
		m_SSBO->SetData(&handle, SizeofTextureMap * m_TextureCount + OffsetofTextureType(TextureType::Specular), sizeof(GLuint64));

		texture.Index = m_TextureCount++;
		return texture;
	}

	Texture2D OpenGLTextureCache::AddTexture2D(const std::vector<std::tuple<std::string, TextureType>>& textureFiles)
	{
		Texture2D texture;

		for (const auto& [textureFile, textureType] : textureFiles)
		{
			const auto& textureIt = m_Textures.find(textureFile);

			// if texture already exists
			if (textureIt != m_Textures.end())
				m_SSBO->SetData(
					&textureIt->second->m_TextureHandle,
					SizeofTextureMap * m_TextureCount + OffsetofTextureType(textureType),
					sizeof(GLuint64));
			else
			{
				stbi_set_flip_vertically_on_load(1);

				TextureData data;
				data.buffer = stbi_load(
					textureFile.c_str(),
					&data.width,
					&data.height,
					&data.channel,
					4);

				if (data.buffer)
				{
					const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });

					m_SSBO->SetData(
						&insertedPairIt->second->m_TextureHandle,
						SizeofTextureMap * m_TextureCount + OffsetofTextureType(textureType),
						sizeof(GLuint64));

					stbi_image_free(data.buffer);
				}
			}
		}

		texture.Index = m_TextureCount++;
		return texture;
	}

	CubemapTexture OpenGLTextureCache::AddCubemap(const std::array<std::string, 6>& cubemapTextures)
	{
		CubemapTexture cubemap;
		const auto& cubemapIt = m_Cubemaps.find(cubemapTextures[0]);

		if (cubemapIt != m_Cubemaps.end())
			cubemap.TextureID = cubemapIt->second->m_TextureID;
		else
		{
			bool loadFailed = false;
			std::array<TextureData, 6> cubemapData;
			for (uint32_t i = 0; i < 6; i++)
			{
				cubemapData[i].buffer = stbi_load(
					cubemapTextures[i].c_str(),
					&cubemapData[i].width,
					&cubemapData[i].height,
					&cubemapData[i].channel,
					4);

				if (cubemapData[i].buffer == nullptr)
				{
					for (uint32_t j = 0; j < i; j++)
						stbi_image_free(cubemapData[j].buffer);

					loadFailed = true;
					break;
				}
			}
			
			if (!loadFailed)
			{
				const auto& insertedPairIt = m_Cubemaps.insert(
					m_Cubemaps.end(),
					{
						std::string(cubemapTextures[0]),
						CreateScope<OpenGLCubemapTexture>(cubemapData)
					});
				cubemap.TextureID = insertedPairIt->second->m_TextureID;

				for (uint32_t i = 0; i < 6; i++)
					stbi_image_free(cubemapData[i].buffer);
			}
		}

		return cubemap;
	}

	void OpenGLTextureCache::Flush()
	{
		// Clear
		uint64_t handle = 0;
		m_SSBO->SetData(
			&handle,
			static_cast<uint16_t>(TextureType::COUNT) * sizeof(GLuint64),
			static_cast<uint16_t>(TextureType::COUNT) * sizeof(GLuint64) * (m_TextureCount - 1));

		m_Textures.clear();
		m_Cubemaps.clear();
		m_TextureCount = 1;
	}
}