#pragma once
#include <glm/glm.hpp>

namespace Doge
{
	class Shader
	{
	public:
		friend class ShaderLibrary;
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformInt(const char* name, int value) const;
		void SetUniformInt2(const char* name, const glm::ivec2& value) const;
		void SetUniformInt3(const char* name, const glm::ivec3& value) const;
		void SetUniformInt4(const char* name, const glm::ivec4& value) const;

		void SetUniformUInt(const char* name, uint32_t value) const;
		void SetUniformUInt2(const char* name, const glm::uvec2& value) const;
		void SetUniformUInt3(const char* name, const glm::uvec3& value) const;
		void SetUniformUInt4(const char* name, const glm::uvec4& value) const;

		void SetUniformFloat(const char* name, float value) const;
		void SetUniformFloat2(const char* name, const glm::vec2& value) const;
		void SetUniformFloat3(const char* name, const glm::vec3& value) const;
		void SetUniformFloat4(const char* name, const glm::vec4& value) const;

		void SetUniformMat3(const char* name, const glm::mat3& value) const;
		void SetUniformMat4(const char* name, const glm::mat4& value) const;

		bool operator==(const Shader& shader) const;
		bool operator!=(const Shader& shader) const;
	private:
		explicit Shader(const char* filePath);
		explicit Shader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc);
	private:
		void CalculateUniformLocations();
		uint32_t GetUniformLocation(const char* name) const;

		static std::string ReadFile(const char* filePath);
		static std::unordered_map<uint32_t, std::string> ParseShaderSource(const std::string& source);
		void Compile(const std::string& vertexSrc, const std::string& fragmentSrc);
	private:
		uint32_t m_RendererID;
		std::unordered_map<std::string, uint32_t> m_UniformCache;
	};

	class ShaderLibrary
	{
	public:
		static const Shader* CreateShader(const char* filePath);
		static const Shader* CreateShader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Shader& GetShader(const std::string& shader);
	private:
		static std::unordered_map<std::string, Shader*> s_ShaderCache;
	};
}