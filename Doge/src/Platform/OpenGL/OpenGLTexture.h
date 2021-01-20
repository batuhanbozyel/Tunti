#pragma once
#include "Doge/Renderer/Texture.h"

namespace Doge
{
	class OpenGLTextureCache;
	class OpenGLShaderStorageBuffer;

	class OpenGLTexture2D final
	{
	public:
		explicit OpenGLTexture2D();
		explicit OpenGLTexture2D(const TextureData& data);
		~OpenGLTexture2D();
	private:
		GLuint m_TextureID;
		GLuint64 m_TextureHandle;

		friend class OpenGLTextureCache;
	};

	class OpenGLCubemapTexture final
	{
	public:
		explicit OpenGLCubemapTexture(const std::array<TextureData, 6>& cubemapData);
		~OpenGLCubemapTexture();
	private:
		GLuint m_TextureID;

		friend class OpenGLTextureCache;
	};

	class OpenGLTextureCache final
	{
	public:
		~OpenGLTextureCache();

		Texture2D AddTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles);
		CubemapTexture AddCubemap(const std::array<std::string, 6>& cubemapTextures);

		void Flush();

		static OpenGLTextureCache* const GetInstance()
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
		GLuint m_TextureMapSSBO;

		Scope<OpenGLTexture2D> m_DefaultTexture;
		std::unordered_map<std::string, Scope<OpenGLTexture2D>> m_Textures;
		std::unordered_map<std::string, Scope<OpenGLCubemapTexture>> m_Cubemaps;

		static inline OpenGLTextureCache* s_Instance = nullptr;
	};
}