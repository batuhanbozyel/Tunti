#pragma once
#include "Tunti/Renderer/Texture.h"

namespace Tunti
{
	class OpenGLTextureCache;
	class OpenGLShaderStorageBuffer;

	class OpenGLTexture2D final
	{
	public:
		explicit OpenGLTexture2D(const std::array<unsigned char, 4>& color);
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

	class OpenGLEnvironmentMapTexture final
	{
	public:
		explicit OpenGLEnvironmentMapTexture(const TextureData& environmentMapData, bool isHDR);
		~OpenGLEnvironmentMapTexture();
	private:
		GLuint m_EnvironmentMapTextureID;
		GLuint m_IrradianceMapTextureID;
		GLuint m_BRDF_LUTTextureID;

		friend class OpenGLTextureCache;
	};

	class OpenGLTextureCache final
	{
	public:
		~OpenGLTextureCache();

		Ref<TextureMap> DefaultTextureMap() const { return m_DefaultTextureMap; }
		Ref<TextureMap> CreateTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles);

		CubemapTexture CreateCubemap(const std::array<std::string, 6>& cubemapTextures);
		EnvironmentMapTexture CreateEnvironmentMap(const std::string& textureFile);

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
		uint32_t m_TextureMapCount = 1;
		GLuint m_TextureMapSSBO;

		OpenGLTexture2D m_WhiteTexture;
		OpenGLTexture2D m_BlackTexture;
		Ref<TextureMap> m_DefaultTextureMap;
		Ref<EnvironmentMapTexture> m_DefaultEnvironmentMap;
		std::unordered_map<std::string, Scope<OpenGLTexture2D>> m_Textures;
		std::unordered_map<std::string, Scope<OpenGLEnvironmentMapTexture>> m_EnvironmentMaps;
		std::unordered_map<std::string, Scope<OpenGLCubemapTexture>> m_Cubemaps;

		static inline OpenGLTextureCache* s_Instance = nullptr;
	};
}