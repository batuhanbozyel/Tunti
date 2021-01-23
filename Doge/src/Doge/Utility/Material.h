#pragma once
#include "Doge/Renderer/Shader.h"

namespace Doge
{
	struct Shader;
	class MaterialInstance;

	using MaterialProperty = std::variant<float, glm::vec2, glm::vec3, glm::vec4>;

	class Material final
	{
	public:
		explicit Material(const Shader& shader, const Ref<TextureMap>& textureMap);

		static Ref<Material> DefaultMaterial();
		static Ref<MaterialInstance> CreateInstanceFrom(const Ref<Material>& material);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		const Ref<TextureMap>& GetTextureMap() const { return m_TextureMap; }
		const std::unordered_map<std::string, MaterialProperty>& GetProperties() const { return m_Properties; }
	private:
		Ref<TextureMap> m_TextureMap;
		std::unordered_map<std::string, MaterialProperty> m_Properties;
		std::vector<Ref<MaterialInstance>> m_ChildInstances;

		static inline Ref<Material> s_DefaultMaterial = nullptr;
	};

	class MaterialInstance final
	{
	public:
		explicit MaterialInstance(const Ref<Material>& parentMaterial);

		template<typename T>
		void ModifyProperty(const std::string& name, const T& value);

		void SetTextureMap(const Ref<TextureMap>& textureMap) { m_TextureMap = textureMap; }
		void SetTexture(Texture2D texture, TextureType type);

		const Ref<TextureMap>& GetTextureMap() const { return m_TextureMap; }
		const std::unordered_map<std::string, MaterialProperty>& GetModifiedProperties() const { return m_ModifiedProperties; }
		const Ref<Material>& GetParentMaterial() const { return m_ParentMaterial; }
	private:
		Ref<TextureMap> m_TextureMap;
		std::unordered_map<std::string, MaterialProperty> m_ModifiedProperties;
		const Ref<Material> m_ParentMaterial;
	};
}