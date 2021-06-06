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

	class ShaderLibrary
	{
	public:
		static Shader LoadShader(const std::string& filePath);
		static Shader LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Shader LoadComputeShader(const std::string& filePath);
		static std::string ReadFile(const std::string& filePath);
	};
}