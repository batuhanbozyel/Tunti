#pragma once
#include "Doge/Renderer/Shader.h"

namespace Doge
{
	struct Shader;
	class MaterialInstance;

	using MaterialProperty = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

	class Material
	{
	public:
		const uint32_t MaterialIndex;
		const Shader ShaderHandle;
	public:
		explicit Material(const Shader& shader);

		static Ref<MaterialInstance> CreateInstanceFrom(const Ref<Material>& material);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		const std::unordered_map<std::string, MaterialProperty>& GetProperties() const { return m_Properties; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		std::vector<Ref<MaterialInstance>> m_ChildInstances;
		
	private:
		static uint32_t s_MaterialCount;
	};

	class MaterialInstance
	{
	public:
		const uint32_t MaterialInstanceIndex;
	public:
		explicit MaterialInstance(const Ref<Material>& parentMaterial, uint32_t index);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		const std::unordered_map<std::string, MaterialProperty>& GetModifiedProperties() const { return m_ModifiedProperties; }
		const Ref<Material>& GetParentMaterial() const { return m_ParentMaterial; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_ModifiedProperties;
		const Ref<Material> m_ParentMaterial;
	};
}