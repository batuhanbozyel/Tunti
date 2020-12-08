#pragma once

namespace Doge
{
	struct Shader;
	class MaterialInstance;

	using MaterialProperty = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

	class Material
	{
	public:
		static const Ref<Material> PhongMaterial;

		enum class DataIndex : uint16_t
		{
			Float	= 0,
			Float2	= 1,
			Float3	= 2,
			Float4	= 3
		};
	public:
		explicit Material(const Shader& shader, const std::unordered_map<std::string, MaterialProperty>& properties);

		static Ref<MaterialInstance> CreateInstance(const Ref<Material>& material);

		const std::unordered_map<std::string, MaterialProperty>& GetProperties() const { return m_Properties; }
		const Shader GetShader() const { return m_Shader; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_Properties;

		std::vector<Ref<MaterialInstance>> m_ChildInstances;
		const Shader m_Shader;
	};

	class MaterialInstance
	{
	public:
		explicit MaterialInstance(const Ref<Material>& parentMaterial);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		const std::unordered_map<std::string, MaterialProperty>& GetModifiedProperties() const { return m_ModifiedProperties; }

		const Ref<Material>& GetParentMaterial() const { return m_ParentMaterial; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_ModifiedProperties;

		const Ref<Material> m_ParentMaterial;
	};
}