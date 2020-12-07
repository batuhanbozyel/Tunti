#pragma once
#include "OpenGLBuffer.h"
#include "Doge/Renderer/Texture.h"

namespace Doge
{
	class OpenGLTextureCache;

	class OpenGLTexture2D
	{
	public:
		explicit OpenGLTexture2D();
		explicit OpenGLTexture2D(const std::string& texturePath);
		~OpenGLTexture2D();
	private:
		GLuint m_TextureID;

		friend class OpenGLTextureCache;
	};

	class OpenGLCubemapTexture
	{
	public:
		explicit OpenGLCubemapTexture(const std::array<std::string, 6>& cubemapTexturePaths);
		~OpenGLCubemapTexture();
	private:
		GLuint m_TextureID;

		friend class OpenGLTextureCache;
	};

	class OpenGLTextureCache
	{
	public:
		~OpenGLTextureCache() = default;

		Texture2D AddTexture2D(const std::string& textureFile);
		Texture2D AddTexture2D(const std::vector<std::tuple<std::string, TextureType>>& textureFiles);
		CubemapTexture AddCubemap(const std::array<std::string, 6>& cubemapTextures);

		static inline OpenGLTextureCache* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLTextureCache;

			return s_Instance;
		}

		OpenGLTextureCache(const OpenGLTextureCache& other) = delete;
		OpenGLTextureCache& operator=(const OpenGLTextureCache& other) = delete;
	private:
		explicit OpenGLTextureCache();
	private:
		// Constructing with a default white texture
		uint32_t m_TextureCount = 1;
		Scope<OpenGLShaderStorageBuffer> m_SSBO;

		std::unordered_map<std::string, Scope<OpenGLTexture2D>> m_Textures;
		std::unordered_map<std::array<std::string, 6>, Scope<OpenGLCubemapTexture>> m_Cubemaps;

		static OpenGLTextureCache* s_Instance;
	};
}