#include "pch.h"
#include "Texture.h"
#include "Renderer.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Doge
{
	Texture2D TextureLibrary::LoadTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return Texture2D();
			case RendererAPI::OpenGL: return OpenGLTextureCache::GetInstance()->AddTextureMap(textureFiles);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return Texture2D();
	}

	CubemapTexture TextureLibrary::LoadCubemap(const std::string& folderPath,
		const std::string& rightFace,
		const std::string& leftFace,
		const std::string& topFace,
		const std::string& bottomFace,
		const std::string& frontFace,
		const std::string& backFace)
	{
		std::array<std::string, 6> cubemapTextures;
		if (*folderPath.rbegin() == '/')
		{
			cubemapTextures = {
				folderPath + rightFace,
				folderPath + leftFace,
				folderPath + topFace,
				folderPath + bottomFace,
				folderPath + frontFace,
				folderPath + backFace
			};
		}
		else
		{
			cubemapTextures = {
				folderPath + "/" + rightFace,
				folderPath + "/" + leftFace,
				folderPath + "/" + topFace,
				folderPath + "/" + bottomFace,
				folderPath + "/" + frontFace,
				folderPath + "/" + backFace
			};
		}

		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return CubemapTexture();
			case RendererAPI::OpenGL: return OpenGLTextureCache::GetInstance()->AddCubemap(cubemapTextures);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return CubemapTexture();
	}
}