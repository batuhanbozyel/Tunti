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

	OpenGLTexture2D::OpenGLTexture2D(const std::array<unsigned char, 4>& color)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTextureParameterf(m_TextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, 1, 1);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color.data());

		m_TextureHandle = glGetTextureHandleARB(m_TextureID);
		glMakeTextureHandleResidentARB(m_TextureHandle);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glMakeTextureHandleNonResidentARB(m_TextureHandle);
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLCubemapTexture

	OpenGLCubemapTexture::OpenGLCubemapTexture(const std::array<TextureData, 6>& cubemapData)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureID);

		glTextureStorage2D(m_TextureID, 1, GL_RGBA8, cubemapData[0].height, cubemapData[0].height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (uint32_t i = 0; i < 6; i++)
			glTextureSubImage3D(m_TextureID, 0, 0, 0, i, cubemapData[i].height, cubemapData[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE, cubemapData[i].buffer);
	}

	OpenGLCubemapTexture::~OpenGLCubemapTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	// OpenGLEnvironmentMapTexture

	OpenGLEnvironmentMapTexture::OpenGLEnvironmentMapTexture(const TextureData& environmentMapData, bool isHDR)
	{
		static constexpr uint32_t kEnvMapSize = 1024;
		static constexpr uint32_t kIrradianceMapSize = 32;
		static constexpr uint32_t kBRDF_LUTSize = 256;
		static constexpr uint32_t kMipMapLevels = CalculateMipMapLevels(kEnvMapSize, kEnvMapSize);

		GLuint environmentMapTextureUnfiltered;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &environmentMapTextureUnfiltered);
		glTextureStorage2D(environmentMapTextureUnfiltered, kMipMapLevels, GL_RGBA16F, kEnvMapSize, kEnvMapSize);
		glTextureParameteri(environmentMapTextureUnfiltered, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(environmentMapTextureUnfiltered, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameterf(environmentMapTextureUnfiltered, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

		// Load & convert equirectangular environment map to a cubemap texture.
		{
			GLuint equirectangularToCubemapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::EquirectangularToCubemap));

			GLuint environmentMapEquirectengularTexture;
			glCreateTextures(GL_TEXTURE_2D, 1, &environmentMapEquirectengularTexture);
			glTextureParameteri(environmentMapEquirectengularTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(environmentMapEquirectengularTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameterf(environmentMapEquirectengularTexture, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);
			glTextureStorage2D(environmentMapEquirectengularTexture, 1, GL_RGB16F, environmentMapData.width, environmentMapData.height);
			if (isHDR) {
				glTextureSubImage2D(environmentMapEquirectengularTexture, 0, 0, 0, environmentMapData.width, environmentMapData.height, GL_RGB, GL_FLOAT, reinterpret_cast<const float*>(environmentMapData.buffer));
			}
			else {
				glTextureSubImage2D(environmentMapEquirectengularTexture, 0, 0, 0, environmentMapData.width, environmentMapData.height, GL_RGB, GL_UNSIGNED_BYTE, environmentMapData.buffer);
			}

			glUseProgram(equirectangularToCubemapShader);
			glBindTextureUnit(0, environmentMapEquirectengularTexture);
			glBindImageTexture(0, environmentMapTextureUnfiltered, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(environmentMapData.width / 32, environmentMapData.height / 32, 6);

			glDeleteTextures(1, &environmentMapEquirectengularTexture);
			glDeleteProgram(equirectangularToCubemapShader);
		}
		glGenerateTextureMipmap(environmentMapTextureUnfiltered);

		// Compute pre-filtered specular environment map.
		{
			GLuint prefilteredSpecularEnvironmentMapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::PrefilteredSpecularEnvironmentMap));

			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_EnvironmentMapTextureID);
			glTextureStorage2D(m_EnvironmentMapTextureID, kMipMapLevels, GL_RGBA16F, kEnvMapSize, kEnvMapSize);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_EnvironmentMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			// Copy 0th mipmap level into destination environment map.
			glCopyImageSubData(environmentMapTextureUnfiltered, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				m_EnvironmentMapTextureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, kEnvMapSize, kEnvMapSize, 6);

			glUseProgram(prefilteredSpecularEnvironmentMapShader);
			glBindTextureUnit(0, environmentMapTextureUnfiltered);

			// Pre-filter rest of the mip chain.
			constexpr float deltaRoughness = 1.0f / glm::max(float(kMipMapLevels - 1), 1.0f);
			for (int level = 1, size = kEnvMapSize / 2; level <= kMipMapLevels; ++level, size /= 2) {
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, m_EnvironmentMapTextureID, level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(prefilteredSpecularEnvironmentMapShader, 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			glDeleteProgram(prefilteredSpecularEnvironmentMapShader);
		}
		glDeleteTextures(1, &environmentMapTextureUnfiltered);

		// Compute diffuse irradiance cubemap.
		{
			GLuint irradianceCubemapShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::IrradianceCubemap));

			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_IrradianceMapTextureID);
			glTextureStorage2D(m_IrradianceMapTextureID, 1, GL_RGBA16F, kIrradianceMapSize, kIrradianceMapSize);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_IrradianceMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			glUseProgram(irradianceCubemapShader);
			glBindTextureUnit(0, m_EnvironmentMapTextureID);
			glBindImageTexture(0, m_EnvironmentMapTextureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
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
	}

	OpenGLEnvironmentMapTexture::~OpenGLEnvironmentMapTexture()
	{
		glDeleteTextures(1, &m_EnvironmentMapTextureID);
		glDeleteTextures(1, &m_IrradianceMapTextureID);
		glDeleteTextures(1, &m_BRDF_LUTTextureID);
	}

	// OpenGLTextureCache

	OpenGLTextureCache::OpenGLTextureCache()
		:	m_WhiteTexture(std::array<unsigned char, 4>{255, 255, 255, 255})
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		m_DefaultPBRTextureMaps.Albedo = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Normal = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Metalness = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.Roughness = Texture2D(m_WhiteTexture.m_TextureHandle);
		m_DefaultPBRTextureMaps.AmbientOcclusion = Texture2D(m_WhiteTexture.m_TextureHandle);

		m_EnvironmentMaps.emplace("default", Scope<OpenGLEnvironmentMapTexture>());
	}

	PBRTextureMaps OpenGLTextureCache::CreateTextureMaps(const std::array<std::string, static_cast<uint16_t>(PBRTextureMap::COUNT)>& textureFiles)
	{
		PBRTextureMaps textureMaps = m_DefaultPBRTextureMaps;
		textureMaps.Albedo = Texture2D(m_WhiteTexture.m_TextureHandle);
		textureMaps.Normal = Texture2D(m_WhiteTexture.m_TextureHandle);
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
			envMap.EnvironmentMapTextureID = envMapCached->m_EnvironmentMapTextureID;
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
			envMap.EnvironmentMapTextureID = envMapCached->m_EnvironmentMapTextureID;
			envMap.IrradianceMapTextureID = envMapCached->m_IrradianceMapTextureID;
			envMap.BRDF_LUTTextureID = envMapCached->m_BRDF_LUTTextureID;

			stbi_image_free(environmentMapData.buffer);
		}

		return envMap;
	}

	CubemapTexture OpenGLTextureCache::CreateCubemap(const std::array<std::string, 6>& cubemapTextures)
	{
		CubemapTexture cubemap;
		const auto& cubemapIt = m_Cubemaps.find(cubemapTextures[0]);

		if (cubemapIt != m_Cubemaps.end())
			cubemap.TextureID = cubemapIt->second->m_TextureID;
		else
		{
			bool loadFailed = false;
			std::array<TextureData, 6> cubemapData;
			for (uint32_t i = 0; i < 6; i++)
			{
				cubemapData[i].buffer = stbi_load(
					cubemapTextures[i].c_str(),
					&cubemapData[i].width,
					&cubemapData[i].height,
					&cubemapData[i].channel,
					4);

				if (cubemapData[i].buffer == nullptr)
				{
					for (uint32_t j = 0; j < i; j++)
						stbi_image_free(cubemapData[j].buffer);

					loadFailed = true;
					break;
				}
			}
			
			if (!loadFailed)
			{
				const auto& insertedPairIt = m_Cubemaps.insert(
					m_Cubemaps.end(),
					{
						cubemapTextures[0],
						CreateScope<OpenGLCubemapTexture>(cubemapData)
					});
				cubemap.TextureID = insertedPairIt->second->m_TextureID;

				for (uint32_t i = 0; i < 6; i++)
					stbi_image_free(cubemapData[i].buffer);
			}
		}

		return cubemap;
	}

	void OpenGLTextureCache::Flush()
	{
		m_Textures.clear();
		m_Cubemaps.clear();
	}
}
