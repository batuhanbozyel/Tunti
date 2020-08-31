#include "pch.h"
#include <glad/glad.h>
#include "Texture.h"
#include "Buffer.h"

#include "stb_image/stb_image.h"

namespace Doge
{
	constexpr uint32_t max_textures = 1024;

	uint32_t TextureManager::m_Count = 0;
	std::vector<std::unique_ptr<Texture>> TextureManager::s_Textures;
	std::unique_ptr<ShaderStorageBuffer> TextureManager::m_SSBO = nullptr;
	std::unordered_map<std::string, uint32_t> TextureManager::s_TextureMap;

	Texture::Texture(const std::string& texturePath)
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

	Texture::~Texture()
	{
		uint64_t handle = glGetTextureHandleARB(m_ID);
		glMakeTextureHandleNonResidentARB(handle);

		glDeleteTextures(1, &m_ID);
	}

	// TextureManager

	void TextureManager::Init()
	{
		s_Textures.reserve(max_textures);
		m_SSBO.reset(new ShaderStorageBuffer(sizeof(TextureMaps) * max_textures, 0));
	}

	uint32_t TextureManager::LoadTexture(const std::string& path)
	{
		auto& textureIt = s_TextureMap.find(path);
		uint32_t index = 0;
		// Texture does not exist: Create a new Texture
		if (textureIt == s_TextureMap.end())
		{
			index = static_cast<uint32_t>(s_Textures.size());
			s_Textures.emplace_back(new Texture(path));
			s_TextureMap.emplace(std::make_pair(path, index));
		}
		// Texture already exists: Get existing Texture's index
		else
		{
			index = textureIt->second;
		}
		// Get Handle with Texture index and store in SSBO
		m_SSBO->Bind();
		uint64_t handle = glGetTextureHandleARB(s_Textures[index]->m_ID);
		m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(TextureType::Diffuse), sizeof(uint64_t));
		m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(TextureType::Specular), sizeof(uint64_t));

		// Return SSBO storage index
		return m_Count++;
	}

	uint32_t TextureManager::LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths)
	{
		m_SSBO->Bind();
		for (const auto& texture : texturePaths)
		{
			auto& textureIt = s_TextureMap.find(texture.first);
			uint32_t index = 0;
			// Texture does not exist: Create a new Texture
			if (textureIt == s_TextureMap.end())
			{
				index = static_cast<uint32_t>(s_Textures.size());
				s_Textures.emplace_back(new Texture(texture.first));
				s_TextureMap.emplace(std::make_pair(texture.first, index));
			}
			// Texture already exists: Get existing Texture's index
			else
			{
				index = textureIt->second;
			}
			// Get Handle with Texture index and store in SSBO
			uint64_t handle = glGetTextureHandleARB(s_Textures[index]->m_ID);
			m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(texture.second), sizeof(uint64_t));
		}

		// Return SSBO storage index
		return m_Count++;
	}
}