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

		constexpr const void* GetPropertyValue() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:		return &std::get<float>(Value);
				case ShaderDataType::Float2:	return &std::get<glm::vec2>(Value);
				case ShaderDataType::Float3:	return &std::get<glm::vec3>(Value);
				case ShaderDataType::Float4:	return &std::get<glm::vec4>(Value);
				case ShaderDataType::Mat3:		return &std::get<glm::mat3>(Value);
				case ShaderDataType::Mat4:		return &std::get<glm::mat4>(Value);
				case ShaderDataType::Int:		return &std::get<int>(Value);
				case ShaderDataType::Int2:		return &std::get<glm::ivec2>(Value);
				case ShaderDataType::Int3:		return &std::get<glm::ivec3>(Value);
				case ShaderDataType::Int4:		return &std::get<glm::ivec4>(Value);
				case ShaderDataType::UInt:		return &std::get<uint32_t>(Value);
				case ShaderDataType::UInt2:		return &std::get<glm::uvec2>(Value);
				case ShaderDataType::UInt3:		return &std::get<glm::uvec3>(Value);
				case ShaderDataType::UInt4:		return &std::get<glm::uvec4>(Value);
				case ShaderDataType::Bool:		return &std::get<bool>(Value);
				case ShaderDataType::Texture2D:	return &std::get<uint64_t>(Value);
			}
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