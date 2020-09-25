#include "pch.h"
#include "Material.h"

#include "Doge/Renderer/Shader.h"

namespace Doge
{
	// Master Material methods

	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{

	}

	void Material::SetSharedUniforms() const
	{
		if (!m_Color.second && m_Color.first != nullptr)
			m_Shader->SetUniformFloat3("u_Material.Color", *m_Color.first);

		if (!m_Shininess.second && m_Shininess.first != nullptr)
			m_Shader->SetUniformFloat("u_Material.Shininess", *m_Shininess.first);
	}

	void Material::SetModifiable(const MaterialProperty& prop)
	{
		switch (prop)
		{
		case MaterialProperty::Color:
			if (m_Color.first != nullptr) m_Color.second = true;
			break;
		case MaterialProperty::Shininess:
			if (m_Shininess.first != nullptr) m_Shininess.second = true;
			break;
		}
	}

	void Material::SetBaseColor(const glm::vec3& color)
	{
		m_Color.first = CreateRef<glm::vec3>(color);
	}

	void Material::SetBaseShininess(const float shininess)
	{
		m_Shininess.first = CreateRef<float>(shininess);
	}

	// Material Instance methods

	MaterialInstance::MaterialInstance(const Ref<Material>& material)
		: Material(*material), m_BaseMaterialRef(material)
	{

	}

	void MaterialInstance::SetModifiedUniforms() const
	{
		const Ref<Shader>& shader = m_BaseMaterialRef->GetShaderRef();
		if (m_Color.second)
			shader->SetUniformFloat3("u_Material.Color", *m_Color.first);

		if (m_Shininess.second)
			shader->SetUniformFloat("u_Material.Shininess", *m_Shininess.first);
	}

	void MaterialInstance::SetColor(const glm::vec3& color)
	{
		if (m_Color.second)
			m_Color.first = CreateRef<glm::vec3>(color);
	}

	void MaterialInstance::SetShininess(const float shininess)
	{
		if (m_Shininess.second)
			m_Shininess.first = CreateRef<float>(shininess);
	}
}