#include "pch.h"
#include "Shader.h"
#include "Texture.h"
#include "RendererBindingTable.h"
#include "Material.h"

namespace Tunti
{
	// Material

	Material::Material(const Shader& shader)
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
	}

	Ref<MaterialInstance> Material::DefaulMaterialInstance()
	{
		if (!s_DefaultMaterial)
			s_DefaultMaterial = CreateRef<Material>(ShaderLibrary::LoadShader(RendererShaders::PBRDeferredPass));

		if (!s_DefaultMaterialInstance)
			s_DefaultMaterialInstance = CreateInstanceFrom(s_DefaultMaterial);

		return s_DefaultMaterialInstance;
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
			m_ModifiedProperties.insert({ name, value });
			return;
		}

		Log::Warn("Property does not exist: {0}", name);
	}

	void MaterialInstance::SetTexture(Texture2D texture, TextureType type)
	{

	}

	Ref<Material> Material::s_DefaultMaterial = nullptr;
	Ref<MaterialInstance> Material::s_DefaultMaterialInstance = nullptr;
}