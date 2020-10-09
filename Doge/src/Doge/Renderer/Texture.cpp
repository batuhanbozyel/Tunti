#include "pch.h"
#include "Texture.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Doge
{
	uint32_t TextureManager::s_TextureCount = 0;
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

	Scope<Texture> Texture::CreateWhiteTexture()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLTexture>();
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	// CubemapTexture

	Scope<CubemapTexture> CubemapTexture::Create(const std::array<std::string, 6>& CubemapFiles)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLCubemapTexture>(CubemapFiles);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	// TextureManager

	void TextureManager::Init()
	{
		s_TextureManager = CreateScope<TextureManager>();
	}

	TextureManager::TextureManager()
	{
		if (!s_TextureManager)
		{
			m_SSBO = ShaderStorageBuffer::Create(SizeofTextureMap * MAX_TEXTURES, 0);

			Ref<Texture> texture = Texture::CreateWhiteTexture();
			texture->SetShaderStorageIndex(0);
			m_TextureMap.insert(std::make_pair("default", texture));
			// Get Handle with Texture index and store in SSBO
			m_SSBO->Bind();
			uint64_t handle = texture->GetTextureHandle();
			m_SSBO->SetData(&handle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Diffuse), sizeof(uint64_t));
			m_SSBO->SetData(&handle, SizeofTextureMap * 0 + OffsetofTextureType(TextureType::Specular), sizeof(uint64_t));
			s_TextureCount++;
		}
	}

	Ref<Texture> TextureManager::LoadTextureImpl(const std::string& path)
	{
		return LoadTextureMapsImpl({ {path, TextureType::Diffuse}, {path, TextureType::Specular} });
	}

	Ref<Texture> TextureManager::LoadTextureMapsImpl(const std::vector<std::pair<std::string, TextureType>>& texturePaths)
	{
		// To return first Texture in the TextureMap which is Diffuse Texture
		Ref<Texture> diffuseTexture = nullptr;

		bool isUnique = true;
		std::vector<std::pair<Ref<Texture>, TextureType>> textures;
		for (const auto& texturePair : texturePaths)
		{
			const auto& textureIt = m_TextureMap.find(texturePair.first);
			Ref<Texture> texture;
			TextureType type = texturePair.second;
			std::string texturePath = texturePair.first;

			// If any of the texturePaths lead to an existing Texture
			// Set isUnique to false
			if (textureIt != m_TextureMap.end())
			{
				isUnique = false;
				texture = (textureIt->second.expired() ? textureIt->second.lock() : Texture::Create(texturePath));
			}
			// Create Textures that don't already exist
			else
			{
				texture = Texture::Create(texturePath);
				texture->SetShaderStorageIndex(s_TextureCount);
				m_TextureMap.insert(std::make_pair(texturePath, texture));
			}
			textures.push_back(std::make_pair(texture, type));

			if (type == TextureType::Diffuse)
				diffuseTexture = texture;
		}

		// If the TextureMap is flagged as New
		// Set each Texture's index into Shader Storage Buffer
		// Increment TextureMap count
		if (isUnique)
		{
			diffuseTexture = nullptr;
			m_SSBO->Bind();
			for (const auto& texture : textures)
			{
				uint64_t handle = texture.first->GetTextureHandle();
				TextureType type = texture.second;
				m_SSBO->SetData(&handle, SizeofTextureMap * s_TextureCount + OffsetofTextureType(type), sizeof(uint64_t));

				if (type == TextureType::Diffuse)
					diffuseTexture = texture.first;
			}
			s_TextureCount++;
		}

		LOG_ASSERT(diffuseTexture, "Diffuse Texture must be specified!");

		return diffuseTexture;
	}

	Ref<CubemapTexture> TextureManager::LoadCubemapImpl(const std::string& folderPath, const std::array<std::string, 6>& cubemapFaces)
	{
		const auto& mapIt = m_CubemapMap.find(folderPath);

		if (mapIt == m_CubemapMap.end())
		{
			Ref<CubemapTexture> newCubemap = CubemapTexture::Create(cubemapFaces);
			m_CubemapMap.insert(std::make_pair(folderPath, newCubemap));
			return newCubemap;
		}
		if (mapIt->second.expired())
			mapIt->second = Ref<CubemapTexture>(CubemapTexture::Create(cubemapFaces));

		return mapIt->second.lock();
	}

	Ref<Texture> TextureManager::LoadTexture(const std::string& path)
	{
		return s_TextureManager->LoadTextureImpl(path);
	}

	Ref<Texture> TextureManager::LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths)
	{
		return s_TextureManager->LoadTextureMapsImpl(texturePaths);
	}

	Ref<CubemapTexture> TextureManager::LoadCubemap(const std::string& folderPath,
		const std::string& rightFace,
		const std::string& leftFace,
		const std::string& topFace,
		const std::string& bottomFace,
		const std::string& frontFace,
		const std::string& backFace)
	{
		if (*folderPath.rbegin() == '/')
		{
			return s_TextureManager->LoadCubemapImpl(folderPath, {
				folderPath + rightFace,
				folderPath + leftFace,
				folderPath + topFace,
				folderPath + bottomFace,
				folderPath + frontFace,
				folderPath + backFace });
		}
		else
		{
			return s_TextureManager->LoadCubemapImpl(folderPath, {
				folderPath + "/" + rightFace,
				folderPath + "/" + leftFace,
				folderPath + "/" + topFace,
				folderPath + "/" + bottomFace,
				folderPath + "/" + frontFace,
				folderPath + "/" + backFace });
		}
	}
}