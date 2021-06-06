#pragma once
#include "Tunti/Renderer/Shader.h"

namespace Tunti
{
	class OpenGLShaderCache;

	class OpenGLShaderProgram final
	{
	public:
		explicit OpenGLShaderProgram(const std::string& source);
		explicit OpenGLShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShaderProgram();

		void Bind() const;
		void Unbind() const;

		void SetUniformInt(const char* name, int value) const;
		void SetUniformInt2(const char* name, const glm::ivec2& value) const;
		void SetUniformInt3(const char* name, const glm::ivec3& value) const;
		void SetUniformInt4(const char* name, const glm::ivec4& value) const;

		void SetUniformUInt(const char* name, uint32_t value) const ;
		void SetUniformUInt2(const char* name, const glm::uvec2& value) const;
		void SetUniformUInt3(const char* name, const glm::uvec3& value) const;
		void SetUniformUInt4(const char* name, const glm::uvec4& value) const;

		void SetUniformFloat(const char* name, float value) const;
		void SetUniformFloat2(const char* name, const glm::vec2& value) const;
		void SetUniformFloat3(const char* name, const glm::vec3& value) const;
		void SetUniformFloat4(const char* name, const glm::vec4& value) const;

		void SetUniformMat3(const char* name, const glm::mat3& value) const;
		void SetUniformMat4(const char* name, const glm::mat4& value) const;

		bool operator==(const Shader& shader) const
		{
			return m_ShaderProgramHandle == shader.Handle;
		}

		bool operator!=(const Shader& shader) const
		{
			return m_ShaderProgramHandle != shader.Handle;
		}

		bool operator==(const OpenGLShaderProgram& shader) const
		{
			return m_ShaderProgramHandle == shader.m_ShaderProgramHandle;
		}

		bool operator!=(const OpenGLShaderProgram& shader) const
		{
			return m_ShaderProgramHandle != shader.m_ShaderProgramHandle;
		}
	private:
		void CalculateUniformLocations();
		uint32_t GetUniformLocation(const char* name) const;
		void Compile(const std::string& vertexSrc, const std::string& fragmentSrc);
		std::unordered_map<uint32_t, std::string> ParseShaderSource(const std::string& source);
	private:
		GLuint m_ShaderProgramHandle;
		std::unordered_map<std::string, uint32_t> m_UniformCache;

		friend class OpenGLShaderCache;
	};

	class OpenGLShaderCache final
	{
	public:
		~OpenGLShaderCache() = default;

		Ref<OpenGLShaderProgram> LoadShaderProgram(const std::string& filepath);

		Shader LoadShaderProgram(const std::string& filePath, const std::string& source);
		Shader LoadShaderProgram(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		Shader LoadComputeShader(const std::string& source);

		void Flush();

		static OpenGLShaderCache* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLShaderCache;

			return s_Instance;
		}

		const Ref<OpenGLShaderProgram>& operator[](const std::string& shaderFile) const
		{
			return m_Shaders.find(m_ShaderFiles.find(shaderFile)->second)->second;
		}

		const Ref<OpenGLShaderProgram>& operator[](const Shader& shader) const
		{
			return m_Shaders.find(shader)->second;
		}

		OpenGLShaderCache(const OpenGLShaderCache& other) = delete;
		OpenGLShaderCache& operator=(const OpenGLShaderCache& other) = delete;
	private:
		explicit OpenGLShaderCache() = default;
	private:
		std::unordered_map<std::string, Shader> m_ShaderFiles;
		std::unordered_map<uint64_t, Ref<OpenGLShaderProgram>> m_Shaders;

		static inline OpenGLShaderCache* s_Instance = nullptr;
	};
}