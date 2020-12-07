#include "pch.h"
#include "Doge/Renderer/Shader.h"
#include "Doge/Renderer/Texture.h"
#include "Material.h"

namespace Doge
{
	const Ref<Material> Material::PhongMaterial = CreateRef<Material>(ShaderLibrary::PhongLighting,
		std::vector<std::pair<std::string, MaterialProperty>>{
			{ "u_Material.Shininess", 32.0f },
			{ "u_Material.Color", glm::vec3(1.0f) }
		});

	// Material

	Material::Material(const Shader& shader, const std::vector<std::pair<std::string, MaterialProperty>>& properties)
		: m_Shader(shader)
	{
		
	}

	void Material::SetModifiable(const std::string& name)
	{
		const auto& propIt = m_Properties.find(name);
		if (propIt != m_Properties.end())
		{
			m_ModifiableProperties.emplace(std::move(*propIt));
			m_Properties.erase(name);
		}
	}

	// MaterialInstance

	MaterialInstance::MaterialInstance(const Ref<Material>& baseMaterial)
		: m_BaseMaterial(baseMaterial), m_ModifiedProperties(baseMaterial->m_ModifiableProperties)
	{

	}

	template<typename T>
	void MaterialInstance::AddProperty(const std::string& name, const T& value)
	{
		if (m_AddedProperties.find(name) == m_AddedProperties.end()
			&& m_BaseMaterial->m_Properties.find(name) == m_BaseMaterial->m_Properties.end()
			&& m_BaseMaterial->m_ModifiableProperties.find(name) == m_BaseMaterial->m_ModifiableProperties.end())
		{
			m_AddedProperties.insert(m_AddedProperties.end(), { name, value });
		}
	}

	template<typename T>
	void MaterialInstance::ModifyProperty(const std::string& name, const T& value)
	{
		auto& propertyIt = m_ModifiedProperties.find(name);

		if (propertyIt == m_ModifiedProperties.end())
		{
			propertyIt = m_AddedProperties.find(name);
			if (propertyIt == m_AddedProperties.end())
				return;
		}
		
		T& propValue = std::get<T>(propertyIt->second);
		propValue = value;
	}
}