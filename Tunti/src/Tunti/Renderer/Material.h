#pragma once
#include "ShaderDataType.h"
#include "Texture.h"

namespace Tunti
{
	using MaterialPropertyValue = std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, int, glm::ivec2, glm::ivec3, glm::ivec4, uint32_t, glm::uvec2, glm::uvec3, glm::uvec4, bool, uint64_t>;
	static constexpr uint32_t kMaxInstancePerMaterial = 1024;

	enum PBRMaterial
	{
		Albedo,
		AlbedoMap,
		NormalMap,
		MetalnessMap,
		RoughnessMap,
		AmbientOcclusionMap,
		Metalness,
		Roughness
	};

	struct MaterialProperty
	{
		const decltype(ShaderDataType::Float) Type;
		MaterialPropertyValue Value;

		constexpr uint32_t ShaderAlignmentSize() const
		{
			uint32_t size = ShaderDataType::Size(Type);
			return size + (4 - size % 4) % 4;
		}

		constexpr uint32_t ValueSize() const
		{
			return ShaderDataType::Size(Type);
		}

		template<class T>
		constexpr T GetValue() const
		{
			return std::get<T>(Value);
		}

		explicit constexpr MaterialProperty(decltype(ShaderDataType::Float) type, const MaterialPropertyValue& defaultValue)
			: Type(type), Value(defaultValue) {}
	};

	struct Material
	{
		const std::vector<MaterialProperty> Properties;
		uint32_t Index = 0;

		operator uint32_t() const
		{
			return Index;
		}

		uint32_t CalculateSize() const;

		Material(const std::initializer_list<MaterialProperty>& props);
	};

	struct MaterialInstance
	{
		std::vector<MaterialProperty> Properties;
		Ref<Material> BaseMaterial;
		uint32_t Index;

		void SetValue(uint32_t propertyIdx, const MaterialPropertyValue& value);

		explicit MaterialInstance(const Ref<Material>& baseMaterial);
	};

	class MaterialLibrary final
	{
	public:
		static Ref<Material> PBRMaterial();
		static Ref<Material> CreateMaterial(const std::initializer_list<MaterialProperty>& props);

		static Ref<MaterialInstance> GetDefaultInstanceFrom(const Ref<Material>& material);
		static Ref<MaterialInstance> CreateInstanceFrom(const Ref<Material>& material);
	};
}