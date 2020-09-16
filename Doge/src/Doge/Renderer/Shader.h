#pragma once
#include <glm/glm.hpp>

namespace Doge
{
	class Shader
	{
	public:
		friend class ShaderLibrary;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformInt(const char* name, int value) const = 0;
		virtual void SetUniformInt2(const char* name, const glm::ivec2& value) const = 0;
		virtual void SetUniformInt3(const char* name, const glm::ivec3& value) const = 0;
		virtual void SetUniformInt4(const char* name, const glm::ivec4& value) const = 0;

		virtual void SetUniformUInt(const char* name, uint32_t value) const = 0;
		virtual void SetUniformUInt2(const char* name, const glm::uvec2& value) const = 0;
		virtual void SetUniformUInt3(const char* name, const glm::uvec3& value) const = 0;
		virtual void SetUniformUInt4(const char* name, const glm::uvec4& value) const = 0;

		virtual void SetUniformFloat(const char* name, float value) const = 0;
		virtual void SetUniformFloat2(const char* name, const glm::vec2& value) const = 0;
		virtual void SetUniformFloat3(const char* name, const glm::vec3& value) const = 0;
		virtual void SetUniformFloat4(const char* name, const glm::vec4& value) const = 0;

		virtual void SetUniformMat3(const char* name, const glm::mat3& value) const = 0;
		virtual void SetUniformMat4(const char* name, const glm::mat4& value) const = 0;

		bool operator==(const Shader& shader) const;
		bool operator!=(const Shader& shader) const;
	protected:
		virtual void CalculateUniformLocations() = 0;
		virtual uint32_t GetUniformLocation(const char* name) const = 0;

		static std::string ReadFile(const char* filePath);
		virtual std::unordered_map<uint32_t, std::string> ParseShaderSource(const std::string& source) = 0;

		virtual void Compile(const std::string& vertexSrc, const std::string& fragmentSrc) = 0;
	private:
		static Scope<Shader> Create(const char* filePath);
		static Scope<Shader> Create(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc);
	protected:
		uint32_t m_RendererID;
		std::unordered_map<std::string, uint32_t> m_UniformCache;
	};

	class ShaderLibrary
	{
	public:
		static Ref<Shader> CreateShader(const char* filePath);
		static Ref<Shader> CreateShader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Shader& GetShader(const std::string& shader);
	private:
		static std::unordered_map<std::string, Ref<Shader>> s_ShaderCache;
	};
}