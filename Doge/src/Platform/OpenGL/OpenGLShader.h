#pragma once
#include "Doge/Renderer/Shader.h"

namespace Doge
{
	class OpenGLShader : public Shader
	{
	public:
		explicit OpenGLShader(const char* filePath);
		explicit OpenGLShader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt(const char* name, int value) const override;
		virtual void SetUniformInt2(const char* name, const glm::ivec2& value) const override;
		virtual void SetUniformInt3(const char* name, const glm::ivec3& value) const override;
		virtual void SetUniformInt4(const char* name, const glm::ivec4& value) const override;

		virtual void SetUniformUInt(const char* name, uint32_t value) const override;
		virtual void SetUniformUInt2(const char* name, const glm::uvec2& value) const override;
		virtual void SetUniformUInt3(const char* name, const glm::uvec3& value) const override;
		virtual void SetUniformUInt4(const char* name, const glm::uvec4& value) const override;

		virtual void SetUniformFloat(const char* name, float value) const override;
		virtual void SetUniformFloat2(const char* name, const glm::vec2& value) const override;
		virtual void SetUniformFloat3(const char* name, const glm::vec3& value) const override;
		virtual void SetUniformFloat4(const char* name, const glm::vec4& value) const override;

		virtual void SetUniformMat3(const char* name, const glm::mat3& value) const override;
		virtual void SetUniformMat4(const char* name, const glm::mat4& value) const override;
	private:
		virtual void CalculateUniformLocations() override;
		virtual uint32_t GetUniformLocation(const char* name) const override;
		virtual void Compile(const std::string& vertexSrc, const std::string& fragmentSrc) override;
		virtual std::unordered_map<uint32_t, std::string> ParseShaderSource(const std::string& source) override;
	};
}