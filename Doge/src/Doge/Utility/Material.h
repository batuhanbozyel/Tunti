#pragma once

namespace Doge
{
	class Shader;

	enum class MaterialProperty
	{
		Color,
		Shininess
	};

	class Material
	{
	public:
		explicit Material(const Ref<Shader>& shader);

		void SetSharedUniforms() const;

		void SetModifiable(const MaterialProperty& prop);

		void SetBaseColor(const glm::vec3& color);
		void SetBaseShininess(const float shininess);

		const Ref<Shader>& GetShaderRef() const { return m_Shader; }

		virtual void SetModifiedUniforms() const {};
	protected:
		template <class T>
		using MaterialData = std::pair<Ref<T>, bool>;

		MaterialData<glm::vec3> m_Color = MaterialData<glm::vec3>(nullptr, false);
		MaterialData<float> m_Shininess = MaterialData<float>(nullptr, false);
	private:
		const Ref<Shader> m_Shader;
	};

	class MaterialInstance : public Material
	{
	public:
		explicit MaterialInstance(const Ref<Material>& material);

		void SetColor(const glm::vec3& color);
		void SetShininess(const float shininess);

		void SetModifiedUniforms() const override;

		const Ref<Material>& GetBaseMaterialRef() const { return m_BaseMaterialRef; }
	private:
		const Ref<Material> m_BaseMaterialRef;
	};
}