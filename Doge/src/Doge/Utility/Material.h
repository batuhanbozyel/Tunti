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
		explicit Material(const Shader& shader);

		void SetSharedUniforms() const;

		void SetModifiable(const MaterialProperty& prop);

		void SetBaseColor(const glm::vec3& color);
		void SetBaseShininess(const float shininess);

		const Shader& GetShaderRef() const { return m_Shader; }

		virtual void SetModifiedUniforms() const {};
	protected:
		template <class T>
		using MaterialData = std::pair<std::shared_ptr<T>, bool>;

		MaterialData<glm::vec3> m_Color = MaterialData<glm::vec3>(nullptr, false);
		MaterialData<float> m_Shininess = MaterialData<float>(nullptr, false);
	private:
		const Shader& m_Shader;
	};

	class MaterialInstance : public Material
	{
	public:
		explicit MaterialInstance(const Material& material);

		void SetColor(const glm::vec3& color);
		void SetShininess(const float shininess);

		void SetModifiedUniforms() const override;

		const Material& GetBaseMaterialRef() const { return m_BaseMaterialRef; }
	private:
		const Material& m_BaseMaterialRef;
	};
}