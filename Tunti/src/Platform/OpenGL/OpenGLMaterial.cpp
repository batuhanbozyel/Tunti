#include "pch.h"
#include <glad/glad.h>
#include "OpenGLMaterial.h"
#include "OpenGLTexture.h"

#include "Tunti/Renderer/RendererBindingTable.h"

namespace Tunti
{
	// OpenGLMaterial

	OpenGLMaterial::OpenGLMaterial(const std::initializer_list<MaterialProperty>& props, uint32_t cacheIdx)
		: m_Material(CreateRef<Material>(props))
	{
		m_Material->Index = cacheIdx;
		glCreateBuffers(1, &m_Buffer);
		glNamedBufferStorage(m_Buffer, m_Material->CalculateSize() * kMaxInstancePerMaterial, nullptr, GL_DYNAMIC_STORAGE_BIT);

		m_DefaultInstance = CreateInstance();
	}

	OpenGLMaterial::~OpenGLMaterial()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	Ref<MaterialInstance> OpenGLMaterial::CreateInstance()
	{
		Ref<MaterialInstance> materialInstance = CreateRef<MaterialInstance>(m_Material);
		uint32_t materialSize = m_Material->CalculateSize();
		void* instanceBuffer = malloc(materialSize);

		if (instanceBuffer)
		{
			uint32_t offset = 0;
			std::byte* instanceBufferIt = reinterpret_cast<std::byte*>(instanceBuffer);
			for (const auto& prop : m_Material->Properties)
			{
				memcpy(instanceBufferIt + offset, prop.GetPropertyValue(), prop.ValueSize());
				offset += prop.ValueSize();
			}
			glNamedBufferSubData(m_Buffer, m_InstanceCount * materialSize, materialSize, instanceBuffer);

			free(instanceBuffer);
			materialInstance->Index = m_InstanceCount++;
			return materialInstance;
		}

		// FIXME: Handle malloc allocation failure
		return nullptr;
	}

	void OpenGLMaterial::Bind() const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, RendererBindingTable::MaterialBufferShaderStorageBuffer, m_Buffer);
	}

	void OpenGLMaterial::SetValue(uint32_t offset, uint32_t size, const void* value) const
	{
		glNamedBufferSubData(m_Buffer, offset, size, value);
	}

	// OpenGLMaterialCache

	Ref<Material> OpenGLMaterialCache::PBRMaterial()
	{
		static std::once_flag PBRMaterial_Flag;
		static Ref<Material> material;
		std::call_once(PBRMaterial_Flag, [&]()
		{
			PBRTextureMaps textureMaps = OpenGLTextureCache::GetInstance()->DefaultPBRTextureMaps();
			material = CreateMaterial({
				MaterialProperty{ShaderDataType::Float4, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}},
				MaterialProperty{ShaderDataType::Texture2D, textureMaps.Albedo},
				MaterialProperty{ShaderDataType::Texture2D, textureMaps.Normal},
				MaterialProperty{ShaderDataType::Texture2D, textureMaps.Metalness},
				MaterialProperty{ShaderDataType::Texture2D, textureMaps.Roughness},
				MaterialProperty{ShaderDataType::Texture2D, textureMaps.AmbientOcclusion},
				MaterialProperty{ShaderDataType::Float, 1.0f},
				MaterialProperty{ShaderDataType::Float, 1.0f}
			});
		});
		return material;
	}

	Ref<Material> OpenGLMaterialCache::CreateMaterial(const std::initializer_list<MaterialProperty>& props)
	{
		return m_MaterialsCache.emplace_back(CreateScope<OpenGLMaterial>(props, m_MaterialsCache.size()))->m_Material;
	}

	Ref<MaterialInstance> OpenGLMaterialCache::GetDefaultInstanceFrom(const Ref<Material>& material)
	{
		return m_MaterialsCache[material->Index]->m_DefaultInstance;
	}

	Ref<MaterialInstance> OpenGLMaterialCache::CreateInstanceFrom(const Ref<Material>& material)
	{
		return m_MaterialsCache[material->Index]->CreateInstance();
	}

	void OpenGLMaterialCache::Flush()
	{
		m_MaterialsCache.clear();
	}
}