#include "pch.h"
#include <glad/glad.h>

#include "Texture.h"
#include "Renderer.h"

#include "Tunti/Core/Application.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Tunti
{

	PBRTextureMaps TextureLibrary::DefaultPBRTextureMaps()
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return PBRTextureMaps();
			case RenderAPI::OpenGL: return OpenGLTextureCache::GetInstance()->DefaultPBRTextureMaps();
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return PBRTextureMaps();
	}

	EnvironmentMapTexture TextureLibrary::DefaultEnvironmentMap()
	{
		return LoadEnvironmentMap(RendererConstants::DefaultEnvironmentMap);
	}

	PBRTextureMaps TextureLibrary::LoadTextureMaps(const std::array<std::string, static_cast<uint16_t>(PBRTextureMap::COUNT)>& textureFiles)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return PBRTextureMaps();
			case RenderAPI::OpenGL: return OpenGLTextureCache::GetInstance()->CreateTextureMaps(textureFiles);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return PBRTextureMaps();
	}

	EnvironmentMapTexture TextureLibrary::LoadEnvironmentMap(const std::string& path)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return EnvironmentMapTexture();
			case RenderAPI::OpenGL: return OpenGLTextureCache::GetInstance()->CreateEnvironmentMap(path);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return EnvironmentMapTexture();
	}
}