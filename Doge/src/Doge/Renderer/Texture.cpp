#include "pch.h"
#include "Texture.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Doge
{
	constexpr uint32_t max_textures = 1024;

	uint32_t TextureManager::m_Count = 0;
	std::vector<std::shared_ptr<Texture>> TextureManager::s_Textures;
	std::shared_ptr<ShaderStorageBuffer> TextureManager::m_SSBO = nullptr;
	std::unordered_map<std::string, uint32_t> TextureManager::s_TextureMap;

	std::shared_ptr<Doge::Texture> Texture::Create(const std::string& texturePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return std::make_shared<OpenGLTexture>(texturePath);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	// TextureManager

	void TextureManager::Init()
	{
		s_Textures.reserve(max_textures);
		m_SSBO = ShaderStorageBuffer::Create(sizeof(TextureMaps) * max_textures, 0);
	}

	uint32_t TextureManager::LoadTexture(const std::string& path)
	{
		auto& textureIt = s_TextureMap.find(path);
		uint32_t index = 0;
		// Texture does not exist: Create a new Texture
		if (textureIt == s_TextureMap.end())
		{
			index = static_cast<uint32_t>(s_Textures.size());
			s_Textures.emplace_back(Texture::Create(path));
			s_TextureMap.emplace(std::make_pair(path, index));
		}
		// Texture already exists: Get existing Texture's index
		else
		{
			index = textureIt->second;
		}
		// Get Handle with Texture index and store in SSBO
		m_SSBO->Bind();
		uint64_t handle = s_Textures[index]->GetTextureHandle();
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
				s_Textures.emplace_back(Texture::Create(texture.first));
				s_TextureMap.emplace(std::make_pair(texture.first, index));
			}
			// Texture already exists: Get existing Texture's index
			else
			{
				index = textureIt->second;
			}
			// Get Handle with Texture index and store in SSBO
			uint64_t handle = s_Textures[index]->GetTextureHandle();
			m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(texture.second), sizeof(uint64_t));
		}

		// Return SSBO storage index
		return m_Count++;
	}
}