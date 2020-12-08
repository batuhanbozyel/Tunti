#include "pch.h"
#include "Doge/Renderer/Shader.h"
#include "Doge/Renderer/Texture.h"
#include "Material.h"

namespace Doge
{
	const Ref<Material> Material::PhongMaterial = CreateRef<Material>(ShaderLibrary::PhongLighting,
		std::unordered_map<std::string, MaterialProperty>{
			{ "u_Material.Shininess", 32.0f },
			{ "u_Material.Color", glm::vec3(1.0f) }
		});

	// Material

	Material::Material(const Shader& shader, const std::unordered_map<std::string, MaterialProperty>& properties)
		: m_Shader(shader), m_Properties(properties)
	{

	}

	Ref<MaterialInstance> Material::CreateInstance(const Ref<Material>& material)
	{
		Ref<MaterialInstance> childInstance = CreateRef<MaterialInstance>(material);
		material->m_ChildInstances.push_back(childInstance);
		return childInstance;
	}

	// MaterialInstance

	MaterialInstance::MaterialInstance(const Ref<Material>& parentMaterial)
		: m_ParentMaterial(parentMaterial)
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
			m_ModifiedProperties.insert(name, value);
			return;
		}

		Log::Warn("Property does not exist: {0}", name);
	}
}