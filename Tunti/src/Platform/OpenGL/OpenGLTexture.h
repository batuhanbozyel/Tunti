#pragma once
#include "Tunti/Renderer/Texture.h"

namespace Tunti
{
	class OpenGLTextureCache;
	class OpenGLShaderStorageBuffer;

	class OpenGLTexture2D final
	{
	public:
		explicit OpenGLTexture2D(const ColorRGB& color);
		explicit OpenGLTexture2D(const ColorRGBA& color);
		explicit OpenGLTexture2D(const TextureData& data);
		~OpenGLTexture2D();
	private:
		GLuint m_TextureID;
		GLuint64 m_TextureHandle;

		friend class OpenGLTextureCache;
	};

	class OpenGLEnvironmentMapTexture final
	{
	public:
		explicit OpenGLEnvironmentMapTexture(const TextureData& environmentMapData, bool isHDR);
		~OpenGLEnvironmentMapTexture();
	private:
		GLuint m_PrefilteredSpecularMapTextureID;
		GLuint m_IrradianceMapTextureID;
		GLuint m_BRDF_LUTTextureID;

		friend class OpenGLTextureCache;
	};

	class OpenGLTextureCache final
	{
	public:
		~OpenGLTextureCache() = default;

		PBRTextureMaps DefaultPBRTextureMaps() const { return m_DefaultPBRTextureMaps; }
		PBRTextureMaps CreateTextureMaps(const std::array<std::string, static_cast<uint16_t>(PBRTextureMap::COUNT)>& textureFiles);

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
		OpenGLTexture2D m_WhiteTexture;
		OpenGLTexture2D m_NormalTexture;
		PBRTextureMaps m_DefaultPBRTextureMaps;
		std::unordered_map<std::string, Ref<OpenGLTexture2D>> m_Textures;
		std::unordered_map<std::string, Ref<OpenGLEnvironmentMapTexture>> m_EnvironmentMaps;

		static inline OpenGLTextureCache* s_Instance = nullptr;
	};
}