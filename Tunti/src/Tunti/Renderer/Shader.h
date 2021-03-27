#pragma once
#include <glm/glm.hpp>

namespace Tunti
{
	struct Shader
	{
		uint64_t Handle = 0;

		operator uint64_t() const { return Handle; }

		Shader(uint64_t handle)
			: Handle(handle) {}

		Shader() = default;
	};

	enum class MaterialDataIndex : uint16_t
	{
		Float = 0,
		Float2 = 1,
		Float3 = 2,
		Float4 = 3,
		NONE
	};

	struct UniformProperty
	{
		MaterialDataIndex Type;
		uint32_t Location;
	};

	class ShaderLibrary
	{
	public:
		static Shader LoadShader(const std::string& filePath);
		static Shader LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Shader LoadComputeShader(const std::string& filePath);

		static std::unordered_map<std::string, UniformProperty> GetMaterialInfo(Shader shader);
		static std::string ReadFile(const std::string& filePath);
	};
}