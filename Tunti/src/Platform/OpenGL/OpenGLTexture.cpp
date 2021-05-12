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

	OpenGLEnvironmentMapTexture::OpenGLEnvironmentMapTexture(const TextureData& environmentMapData)
	{
		static constexpr uint32_t envMapSize = 1024;
		static constexpr uint32_t irradianceMapSize = 32;
		static constexpr uint32_t brdfToLUTSize = 256;
		static constexpr uint32_t mipmapLevels = CalculateMipMapLevels(envMapSize, envMapSize);

		GLuint environmentMapTextureUnfiltered;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &environmentMapTextureUnfiltered);
		glTextureStorage2D(environmentMapTextureUnfiltered, mipmapLevels, GL_RGBA16F, envMapSize, envMapSize);
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
			glTextureSubImage2D(environmentMapEquirectengularTexture, 0, 0, 0, environmentMapData.width, environmentMapData.height, GL_RGB, GL_FLOAT, environmentMapData.buffer);

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
			glTextureStorage2D(m_EnvironmentMapTextureID, mipmapLevels, GL_RGBA16F, envMapSize, envMapSize);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_EnvironmentMapTextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameterf(m_EnvironmentMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			// Copy 0th mipmap level into destination environment map.
			glCopyImageSubData(environmentMapTextureUnfiltered, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				m_EnvironmentMapTextureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, envMapSize, envMapSize, 6);

			glUseProgram(prefilteredSpecularEnvironmentMapShader);
			glBindTextureUnit(0, environmentMapTextureUnfiltered);

			// Pre-filter rest of the mip chain.
			constexpr float deltaRoughness = 1.0f / glm::max(float(mipmapLevels - 1), 1.0f);
			for (int level = 1, size = envMapSize / 2; level <= mipmapLevels; ++level, size /= 2) {
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
			glTextureStorage2D(m_IrradianceMapTextureID, 1, GL_RGBA16F, irradianceMapSize, irradianceMapSize);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_IrradianceMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_IrradianceMapTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);

			glUseProgram(irradianceCubemapShader);
			glBindTextureUnit(0, m_EnvironmentMapTextureID);
			glBindImageTexture(0, m_EnvironmentMapTextureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glDispatchCompute(irradianceMapSize / 32, irradianceMapSize / 32, 6);
			glDeleteProgram(irradianceCubemapShader);
		}

		// Compute Cook-Torrance BRDF 2D LUT for split-sum approximation.
		{
			GLuint brdfto2DLUTTextureShader = OpenGLShaderCache::GetInstance()->LoadComputeShader(ShaderLibrary::ReadFile(RendererShaders::BRDFto2DLUTTexture));

			glCreateTextures(GL_TEXTURE_2D, 1, &m_BRDFto2DLUTTextureID);
			glTextureStorage2D(m_BRDFto2DLUTTextureID, 1, GL_RG16F, brdfToLUTSize, brdfToLUTSize);
			glTextureParameteri(m_BRDFto2DLUTTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_BRDFto2DLUTTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameterf(m_BRDFto2DLUTTextureID, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);
			glTextureParameteri(m_BRDFto2DLUTTextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_BRDFto2DLUTTextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glUseProgram(brdfto2DLUTTextureShader);
			glBindImageTexture(0, m_BRDFto2DLUTTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
			glDispatchCompute(brdfToLUTSize / 32, brdfToLUTSize / 32, 1);
			glDeleteProgram(brdfto2DLUTTextureShader);
		}
		glFinish();
	}

	OpenGLEnvironmentMapTexture::~OpenGLEnvironmentMapTexture()
	{
		glDeleteTextures(1, &m_EnvironmentMapTextureID);
		glDeleteTextures(1, &m_IrradianceMapTextureID);
		glDeleteTextures(1, &m_BRDFto2DLUTTextureID);
	}

	// OpenGLTextureCache

	OpenGLTextureCache::OpenGLTextureCache()
		: m_DefaultTextureMap(CreateRef<TextureMap>()),
		m_DefaultEnvironmentMap(CreateRef<EnvironmentMapTexture>()),
		m_WhiteTexture(std::array<unsigned char, 4>{255, 255, 255, 255}),
		m_BlackTexture(std::array<unsigned char, 4>{0, 0, 0, 255})
	{
		std::fill(m_DefaultTextureMap->Textures.begin(), m_DefaultTextureMap->Textures.end(), m_WhiteTexture.m_TextureHandle);
		glCreateBuffers(1, &m_TextureMapSSBO);
		glNamedBufferStorage(m_TextureMapSSBO, SizeofTextureMap * MaxTextures, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, RendererBindingTable::TextureMapsShaderStorageBuffer, m_TextureMapSSBO);
		glNamedBufferSubData(m_TextureMapSSBO, 0, m_DefaultTextureMap->Textures.size() * sizeof(GLuint64), m_DefaultTextureMap->Textures.data());
		m_EnvironmentMaps.emplace("default", Scope<OpenGLEnvironmentMapTexture>());
	}

	OpenGLTextureCache::~OpenGLTextureCache()
	{
		glDeleteBuffers(1, &m_TextureMapSSBO);
	}

	Ref<TextureMap> OpenGLTextureCache::CreateTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles)
	{
		Ref<TextureMap> textureMap = CreateRef<TextureMap>();
		TextureType type = static_cast<TextureType>(0);
		for (const auto& textureFile : textureFiles)
		{
			uint16_t typeIndex = static_cast<uint16_t>(type);
			if (!textureFile.empty())
			{
				const auto& textureIt = m_Textures.find(textureFile);
				// if texture already exists
				if (textureIt != m_Textures.end())
					textureMap->Textures[typeIndex] = textureIt->second->m_TextureHandle;
				else
				{
					std::filesystem::path(textureFile).extension().string() == ".tga" ? stbi_set_flip_vertically_on_load(0) : stbi_set_flip_vertically_on_load(1);

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
						textureMap->Textures[typeIndex] = insertedPairIt->second->m_TextureHandle;
						stbi_image_free(data.buffer);
					}
				}
				glNamedBufferSubData(
					m_TextureMapSSBO,
					SizeofTextureMap * m_TextureMapCount + OffsetofTextureType(type),
					sizeof(GLuint64),
					&textureMap->Textures[typeIndex]);
			}
			else
			{
				if (type == TextureType::Metalness)
				{
					textureMap->Textures[typeIndex] = m_BlackTexture.m_TextureHandle;
					glNamedBufferSubData(
						m_TextureMapSSBO,
						SizeofTextureMap * m_TextureMapCount + OffsetofTextureType(type),
						sizeof(GLuint64),
						&m_BlackTexture.m_TextureHandle);
				}
				else
				{
					textureMap->Textures[typeIndex] = m_WhiteTexture.m_TextureHandle;
					glNamedBufferSubData(
						m_TextureMapSSBO,
						SizeofTextureMap * m_TextureMapCount + OffsetofTextureType(type),
						sizeof(GLuint64),
						&m_WhiteTexture.m_TextureHandle);
				}
				
			}
			
			type = static_cast<TextureType>(typeIndex + 1);
		}
		textureMap->Index = m_TextureMapCount++;
		return textureMap;
	}

	EnvironmentMapTexture OpenGLTextureCache::CreateEnvironmentMap(const std::string& textureFile)
	{
		EnvironmentMapTexture envMap;
		TextureData environmentMapData;
		const auto& envMapIt = m_EnvironmentMaps.find(textureFile);

		if (envMapIt != m_EnvironmentMaps.end())
		{
			const auto& envMapCached = envMapIt->second;
			envMap.EnvironmentMapTextureID = envMapCached->m_EnvironmentMapTextureID;
			envMap.IrradianceMapTextureID = envMapCached->m_IrradianceMapTextureID;
			envMap.BRDFto2DLUTTextureID = envMapCached->m_BRDFto2DLUTTextureID;
		}
		else if (stbi_is_hdr(textureFile.c_str()))
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
					CreateScope<OpenGLEnvironmentMapTexture>(environmentMapData)
				});
			const auto& envMapCached = insertedPairIt->second;
			envMap.EnvironmentMapTextureID = envMapCached->m_EnvironmentMapTextureID;
			envMap.IrradianceMapTextureID = envMapCached->m_IrradianceMapTextureID;
			envMap.BRDFto2DLUTTextureID = envMapCached->m_BRDFto2DLUTTextureID;

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
		GLuint64 zero = 0;
		glNamedBufferSubData(
			m_TextureMapSSBO,
			static_cast<uint16_t>(TextureType::COUNT) * sizeof(GLuint64),
			static_cast<uint16_t>(TextureType::COUNT) * sizeof(GLuint64) * (m_TextureMapCount - 1),
			&zero);

		m_Textures.clear();
		m_Cubemaps.clear();
		m_TextureMapCount = 1;
	}
}
