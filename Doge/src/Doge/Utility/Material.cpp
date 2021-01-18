#include "pch.h"
#include "Doge/Renderer/Shader.h"
#include "Doge/Renderer/Texture.h"
#include "Material.h"

namespace Doge
{
	uint32_t Material::s_MaterialCount = 0;

	// Material

	Material::Material(const Shader& shader)
		: ShaderHandle(shader), MaterialIndex(s_MaterialCount)
	{
		const auto& uniforms = ShaderLibrary::GetMaterialInfo(shader);
		for (const auto& [name, uniform] : uniforms)
		{
			switch (uniform.Type)
			{
				case MaterialDataIndex::Float:
				{
					m_Properties.insert({ name, MaterialProperty(1.0f) });
					break;
				}
				case MaterialDataIndex::Float2:
				{
					m_Properties.insert({ name, MaterialProperty(glm::vec2(1.0f)) });
					break;
				}
				case MaterialDataIndex::Float3:
				{
					m_Properties.insert({ name, MaterialProperty(glm::vec3(1.0f)) });
					break;
				}
				case MaterialDataIndex::Float4:
				{
					m_Properties.insert({ name, MaterialProperty(glm::vec4(1.0f)) });
					break;
				}	
			}
		}
			
		s_MaterialCount++;
	}

	template<typename T>
	void Material::ModifyProperty(const std::string& name, const T& value)
	{
		auto& propertyIt = m_Properties.find(name);
		if (propertyIt != m_Properties.end())
		{
			T& propValue = std::get<T>(propertyIt->second);
			propValue = value;
			return;
		}
		Log::Warn("Property does not exist: {0}", name);
	}

	Ref<MaterialInstance> Material::CreateInstanceFrom(const Ref<Material>& material)
	{
		Ref<MaterialInstance> childInstance = CreateRef<MaterialInstance>(material, material->m_ChildInstances.size());
		material->m_ChildInstances.push_back(childInstance);
		return childInstance;
	}

	// MaterialInstance

	MaterialInstance::MaterialInstance(const Ref<Material>& parentMaterial, uint32_t index)
		: m_ParentMaterial(parentMaterial), MaterialInstanceIndex(index)
	{

	}

	template<typename T>
	void MaterialInstance::ModifyProperty(const std::string& name, const T& value)
	{
		auto& propertyIt = m_ModifiedProperties.find(name);
		if (propertyIt != m_ModifiedProperties.end())
		{
			T& propValue = std::get<T>(propertyIt->second);
			propValue = value;
			return;
		}

		propertyIt = m_ParentMaterial->GetProperties().find(name);
		if (propertyIt != m_ParentMaterial->GetProperties().end())
		{
			m_ModifiedProperties.insert({ name, value });
			return;
		}

		Log::Warn("Property does not exist: {0}", name);
	}
}