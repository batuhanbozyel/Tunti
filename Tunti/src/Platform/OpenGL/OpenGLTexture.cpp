#include "pch.h"
#include <glad/glad.h>
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

#include "Tunti/Renderer/Shader.h"
#include "Tunti/Renderer/RendererBindingTable.h"

#include "stb_image/stb_image.h"

namespace Tunti
{
	// OpenGLTexture
	static GLfloat MaxAnisotropy = 1.0f;

	OpenGLTexture2D::OpenGLTexture2D(const TextureData& data)
	{
		uint32_t mipmapLevels = CalculateMipMapLevels(data.width, data.height);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, mipmapLevels > 1 ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR);

		glTextureStorage2D(m_TextureID, mipmapLevels, GL_RGBA8, data.width, data.height);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, data.width, data.height, GL_RGBA, GL_UNSIGNED_BYTE, data.buffer);

		if(mipmapLevels > 1) glGenerateTextureMipmap(m_TextureID);

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
		// Temporary
		glMakeTextureHandleResidentARB(m_TextureHandle);
	}

	OpenGLTexture2D::OpenGLTexture2D(const ColorRGB& color)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTextureParameterf(m_TextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

		glTextureStorage2D(m_TextureID, 1, GL_RGB8, 1, 1);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color);

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
		glMakeTextureHandleResidentARB(m_TextureHandle);
	}

	OpenGLTexture2D::OpenGLTexture2D(const ColorRGBA& color)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTextureParameterf(m_TextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, 1, 1);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
		glMakeTextureHandleResidentARB(m_TextureHandle);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glMakeTextureHandleNonResidentARB(m_TextureHandle);
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLEnvironmentMapTexture

	OpenGLEnvironmentMapTexture::OpenGLEnvironmentMapTexture(const TextureData& environmentMapData, bool isHDR)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		static constexpr uint32_t kEnvMapSize = 1024;
		static constexpr uint32_t kIrradianceMapSize = 32;
		static constexpr uint32_t kBRDF_LUTSize = 256;

		GLuint environmentMapTextureID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &environmentMapTextureID);
		glTextureStorage2D(environmentMapTextureID, CalculateMipMapLevels(kEnvMapSize, kEnvMapSize), GL_RGBA16F, kEnvMapSize, kEnvMapSize);
		glTextureParameteri(environmentMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(environmentMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameterf(environmentMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

		// Load & convert equirectangular environment map to a cubemap texture.
		{
			GLuint equirectangularToCubemapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::EquirectangularToCubemap));

			GLuint environmentMapEquirectengularTexture;
			glCreateTextures(GL_TEXTURE_2D, 1, &environmentMapEquirectengularTexture);
			glTextureParameteri(environmentMapEquirectengularTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(environmentMapEquirectengularTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(environmentMapEquirectengularTexture, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);
			glTextureStorage2D(environmentMapEquirectengularTexture, 1, GL_RGB16F, environmentMapData.width, environmentMapData.height);
			if (isHDR) {
				glTextureSubImage2D(environmentMapEquirectengularTexture, 0, 0, 0, environmentMapData.width, environmentMapData.height, GL_RGB, GL_FLOAT, reinterpret_cast<const float*>(environmentMapData.buffer));
			}
			else {
				glTextureSubImage2D(environmentMapEquirectengularTexture, 0, 0, 0, environmentMapData.width, environmentMapData.height, GL_RGB, GL_UNSIGNED_BYTE, environmentMapData.buffer);
			}

			glUseProgram(equirectangularToCubemapShader);
			glProgramUniform1i(equirectangularToCubemapShader, 0, RendererBindingTable::EmptyTextureUnitBase);
			glBindTextureUnit(RendererBindingTable::EmptyTextureUnitBase, environmentMapEquirectengularTexture);
			glBindImageTexture(0, environmentMapTextureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(kEnvMapSize / 32, kEnvMapSize / 32, 6);

			glDeleteTextures(1, &environmentMapEquirectengularTexture);
			glDeleteProgram(equirectangularToCubemapShader);
		}
		glGenerateTextureMipmap(environmentMapTextureID);

		// Compute pre-filtered specular environment map.
		{
			GLuint prefilteredSpecularEnvironmentMapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::PrefilteredSpecularEnvironmentMap));

			constexpr uint32_t levels = CalculateMipMapLevels(kEnvMapSize, kEnvMapSize);
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_PrefilteredSpecularMapTextureID);
			glTextureStorage2D(m_PrefilteredSpecularMapTextureID, levels, GL_RGBA16F, kEnvMapSize, kEnvMapSize);
			glTextureParameteri(m_PrefilteredSpecularMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(m_PrefilteredSpecularMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_PrefilteredSpecularMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			// Copy 0th mipmap level into destination environment map.
			glCopyImageSubData(environmentMapTextureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				m_PrefilteredSpecularMapTextureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, kEnvMapSize, kEnvMapSize, 6);

			glUseProgram(prefilteredSpecularEnvironmentMapShader);
			glProgramUniform1i(prefilteredSpecularEnvironmentMapShader, 0, RendererBindingTable::EmptyTextureUnitBase);
			glBindTextureUnit(RendererBindingTable::EmptyTextureUnitBase, environmentMapTextureID);

			// Pre-filter rest of the mip chain.
			constexpr float deltaRoughness = 1.0f / glm::max(float(levels - 1), 1.0f);
			for (int level = 1, size = kEnvMapSize / 2; level <= levels; ++level, size /= 2) {
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, m_PrefilteredSpecularMapTextureID, level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(prefilteredSpecularEnvironmentMapShader, 1, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			glDeleteProgram(prefilteredSpecularEnvironmentMapShader);
		}
		glDeleteTextures(1, &environmentMapTextureID);

		// Compute diffuse irradiance cubemap.
		{
			GLuint irradianceCubemapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::IrradianceCubemap));

			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_IrradianceMapTextureID);
			glTextureStorage2D(m_IrradianceMapTextureID, 1, GL_RGBA16F, kIrradianceMapSize, kIrradianceMapSize);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_IrradianceMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			glUseProgram(irradianceCubemapShader);
			glProgramUniform1i(irradianceCubemapShader, 0, RendererBindingTable::EmptyTextureUnitBase);
			glBindTextureUnit(RendererBindingTable::EmptyTextureUnitBase, m_PrefilteredSpecularMapTextureID);
			glBindImageTexture(0, m_IrradianceMapTextureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(kIrradianceMapSize / 32, kIrradianceMapSize / 32, 6);
			glDeleteProgram(irradianceCubemapShader);
		}

		// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
		{
			GLuint brdfLUTTextureShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::BRDF_LUTTexture));

			glCreateTextures(GL_TEXTURE_2D, 1, &m_BRDF_LUTTextureID);
			glTextureStorage2D(m_BRDF_LUTTextureID, 1, GL_RG16F, kBRDF_LUTSize, kBRDF_LUTSize);
			glTextureParameteri(m_BRDF_LUTTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_BRDF_LUTTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_BRDF_LUTTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);
			glTextureParameteri(m_BRDF_LUTTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_BRDF_LUTTextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glUseProgram(brdfLUTTextureShader);
			glBindImageTexture(0, m_BRDF_LUTTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(kBRDF_LUTSize / 32, kBRDF_LUTSize / 32, 1);
			glDeleteProgram(brdfLUTTextureShader);
		}
		glFinish();
		glBindTextureUnit(RendererBindingTable::EmptyTextureUnitBase, 0);
	}

	OpenGLEnvironmentMapTexture::~OpenGLEnvironmentMapTexture()
	{
		glDeleteTextures(1, &m_PrefilteredSpecularMapTextureID);
		glDeleteTextures(1, &m_IrradianceMapTextureID);
		glDeleteTextures(1, &m_BRDF_LUTTextureID);
	}

	// OpenGLTextureCache

	OpenGLTextureCache::OpenGLTextureCache()
		:	m_WhiteTexture(ColorRGBA{ 255, 255, 255, 255 }),
		m_NormalTexture(ColorRGB{ 128, 128, 255 })
	{
		m_DefaultPBRTextureMaps.Albedo = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Normal = Texture2D(m_NormalTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Metalness = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Roughness = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.AmbientOcclusion = Texture2D(m_WhiteTexture.m_TextureHandle);
	}

	PBRTextureMaps OpenGLTextureCache::CreateTextureMaps(const std::array<std::string, static_cast<uint16_t>(PBRTextureMap::COUNT)>& textureFiles)
	{
		PBRTextureMaps textureMaps = m_DefaultPBRTextureMaps;
		textureMaps.Albedo = Texture2D(m_WhiteTexture.m_TextureHandle);
		textureMaps.Normal = Texture2D(m_NormalTexture.m_TextureHandle);
		textureMaps.Metalness = Texture2D(m_WhiteTexture.m_TextureHandle);
		textureMaps.Roughness = Texture2D(m_WhiteTexture.m_TextureHandle);
		textureMaps.AmbientOcclusion = Texture2D(m_WhiteTexture.m_TextureHandle);
		{
			const std::string& textureFile = textureFiles[(uint32_t)PBRTextureMap::Albedo];
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				if (textureIt != m_Textures.end())
				{
					textureMaps.Albedo = Texture2D(textureIt->second->m_TextureHandle);
				}
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(1) : stbi_set_flip_vertically_on_load(0);

					TextureData data;
					data.buffer = stbi_load(
						textureFile.c_str(),
						&data.width,
						&data.height,
						&data.channel,
						4);

					if (data.buffer)
					{
						const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
						textureMaps.Albedo = Texture2D(insertedPairIt->second->m_TextureHandle);
						stbi_image_free(data.buffer);
					}
				}
			}
		}
		{
			const std::string& textureFile = textureFiles[(uint32_t)PBRTextureMap::Normal];
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				if (textureIt != m_Textures.end())
				{
					textureMaps.Normal = Texture2D(textureIt->second->m_TextureHandle);
				}
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(1) : stbi_set_flip_vertically_on_load(0);

					TextureData data;
					data.buffer = stbi_load(
						textureFile.c_str(),
						&data.width,
						&data.height,
						&data.channel,
						4);

					if (data.buffer)
					{
						const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
						textureMaps.Normal = Texture2D(insertedPairIt->second->m_TextureHandle);
						stbi_image_free(data.buffer);
					}
				}
			}
		}
		{
			const std::string& textureFile = textureFiles[(uint32_t)PBRTextureMap::Metalness];
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				if (textureIt != m_Textures.end())
				{
					textureMaps.Metalness = Texture2D(textureIt->second->m_TextureHandle);
				}
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(1) : stbi_set_flip_vertically_on_load(0);

					TextureData data;
					data.buffer = stbi_load(
						textureFile.c_str(),
						&data.width,
						&data.height,
						&data.channel,
						4);

					if (data.buffer)
					{
						const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
						textureMaps.Metalness = Texture2D(insertedPairIt->second->m_TextureHandle);
						stbi_image_free(data.buffer);
					}
				}
			}
		}
		{
			const std::string& textureFile = textureFiles[(uint32_t)PBRTextureMap::Roughness];
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				if (textureIt != m_Textures.end())
				{
					textureMaps.Roughness = Texture2D(textureIt->second->m_TextureHandle);
				}
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(1) : stbi_set_flip_vertically_on_load(0);

					TextureData data;
					data.buffer = stbi_load(
						textureFile.c_str(),
						&data.width,
						&data.height,
						&data.channel,
						4);

					if (data.buffer)
					{
						const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
						textureMaps.Roughness = Texture2D(insertedPairIt->second->m_TextureHandle);
						stbi_image_free(data.buffer);
					}
				}
			}
		}
		{
			const std::string& textureFile = textureFiles[(uint32_t)PBRTextureMap::AmbientOcclusion];
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				if (textureIt != m_Textures.end())
				{
					textureMaps.AmbientOcclusion = Texture2D(textureIt->second->m_TextureHandle);
				}
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(1) : stbi_set_flip_vertically_on_load(0);

					TextureData data;
					data.buffer = stbi_load(
						textureFile.c_str(),
						&data.width,
						&data.height,
						&data.channel,
						4);

					if (data.buffer)
					{
						const auto& insertedPairIt = m_Textures.insert(m_Textures.end(), { textureFile, CreateScope<OpenGLTexture2D>(data) });
						textureMaps.AmbientOcclusion = Texture2D(insertedPairIt->second->m_TextureHandle);
						stbi_image_free(data.buffer);
					}
				}
			}
		}
		return textureMaps;
	}

	EnvironmentMapTexture OpenGLTextureCache::CreateEnvironmentMap(const std::string& textureFile)
	{
		EnvironmentMapTexture envMap;
		TextureData environmentMapData;
		bool isHDR = stbi_is_hdr(textureFile.c_str());

		const auto& envMapIt = m_EnvironmentMaps.find(textureFile);
		if (envMapIt != m_EnvironmentMaps.end())
		{
			const auto& envMapCached = envMapIt->second;
			envMap.PrefilteredSpecularMapTextureID = envMapCached->m_PrefilteredSpecularMapTextureID;
			envMap.IrradianceMapTextureID = envMapCached->m_IrradianceMapTextureID;
			envMap.BRDF_LUTTextureID = envMapCached->m_BRDF_LUTTextureID;
		}
		else if (isHDR)
		{
			environmentMapData.buffer = reinterpret_cast<unsigned char*>(stbi_loadf(textureFile.c_str(), &environmentMapData.width, &environmentMapData.height, &environmentMapData.channel, 3));
		}
		else
		{
			environmentMapData.buffer = stbi_load(textureFile.c_str(), &environmentMapData.width, &environmentMapData.height, &environmentMapData.channel, 3);
		}

		if (environmentMapData.buffer)
		{
			const auto& insertedPairIt = m_EnvironmentMaps.insert(
				m_EnvironmentMaps.end(),
				{
					textureFile,
					CreateScope<OpenGLEnvironmentMapTexture>(environmentMapData, isHDR)
				});
			const auto& envMapCached = insertedPairIt->second;
			envMap.PrefilteredSpecularMapTextureID = envMapCached->m_PrefilteredSpecularMapTextureID;
			envMap.IrradianceMapTextureID = envMapCached->m_IrradianceMapTextureID;
			envMap.BRDF_LUTTextureID = envMapCached->m_BRDF_LUTTextureID;

			stbi_image_free(environmentMapData.buffer);
		}

		return envMap;
	}

	void OpenGLTextureCache::Flush()
	{
		m_Textures.clear();
		m_EnvironmentMaps.clear();
	}
}
