#pragma once
#include <glm/glm.hpp>

namespace Doge
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
		static const Shader PhongLighting;
	public:
		static Shader LoadShader(const std::string& filePath);
		static Shader LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	private:
		static std::string ReadFile(const std::string& filePath);
	};
}