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
		explicit Material(const Shader& shader, const std::vector<std::pair<std::string, MaterialProperty>>& properties);

		void SetModifiable(const std::string& name);

		const std::unordered_map<std::string, MaterialProperty>& GetProperties() const { return m_Properties; }
		const Shader GetShader() const { return m_Shader; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		std::unordered_map<std::string, MaterialProperty> m_ModifiableProperties;
		Shader m_Shader;

		friend class MaterialInstance;
	};

	class MaterialInstance
	{
	public:
		explicit MaterialInstance(const Ref<Material>& baseMaterial);

		template<typename T>
		void AddProperty(const std::string& name, const T& value);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		const std::unordered_map<std::string, MaterialProperty>& GetAddedProperties() const { return m_AddedProperties; }
		const std::unordered_map<std::string, MaterialProperty>& GetModifiedProperties() const { return m_ModifiedProperties; }

		const Ref<Material>& GetBaseMaterial() const { return m_BaseMaterial; }
	private:
		std::unordered_map<std::string, MaterialProperty> m_AddedProperties;
		std::unordered_map<std::string, MaterialProperty> m_ModifiedProperties;
		Ref<Material> m_BaseMaterial;
	};
}