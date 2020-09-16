#include "pch.h"
#include "Texture.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Doge
{
	constexpr uint32_t max_textures = 1024;
	Scope<TextureManager> TextureManager::s_TextureManager = nullptr;

	// Texture

	Scope<Texture> Texture::Create(const std::string& texturePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLTexture>(texturePath);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Scope<Doge::Texture> Texture::CreateWhiteTexture()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLTexture>();
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	// TextureManager

	TextureManager::TextureManager()
	{
		s_Textures.reserve(max_textures);
		m_SSBO = ShaderStorageBuffer::Create(sizeof(TextureMaps) * max_textures, 0);

		s_Textures.emplace_back(Texture::CreateWhiteTexture());
		// Get Handle with Texture index and store in SSBO
		m_SSBO->Bind();
		uint64_t handle = s_Textures[0]->GetTextureHandle();
		m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(TextureType::Diffuse), sizeof(uint64_t));
		m_SSBO->SetData(&handle, sizeof(TextureMaps) * m_Count + static_cast<size_t>(TextureType::Specular), sizeof(uint64_t));

		m_Count++;
	}

	uint32_t TextureManager::LoadTextureImpl(const std::string& path)
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

	uint32_t TextureManager::LoadTextureMapsImpl(const std::vector<std::pair<std::string, TextureType>>& texturePaths)
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

	void TextureManager::Init()
	{
		s_TextureManager.reset(new TextureManager);
	}

	uint32_t TextureManager::LoadTexture(const std::string& path)
	{
		return s_TextureManager->LoadTextureImpl(path);
	}

	uint32_t TextureManager::LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths)
	{
		return s_TextureManager->LoadTextureMapsImpl(texturePaths);
	}
}