#include "pch.h"
#include "OpenGLShader.h"
#include <glad/glad.h>

namespace Doge
{

	OpenGLShader::OpenGLShader(const char* filePath)
	{
		const std::string& source = ReadFile(filePath);
		auto& parsedSource = ParseShaderSource(source);
		Compile(parsedSource[GL_VERTEX_SHADER], parsedSource[GL_FRAGMENT_SHADER]);
		CalculateUniformLocations();
	}

	OpenGLShader::OpenGLShader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		Compile(vertexSrc, fragmentSrc);
		CalculateUniformLocations();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt(const char* name, int value) const
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformInt2(const char* name, const glm::ivec2& value) const
	{
		glUniform2i(GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformInt3(const char* name, const glm::ivec3& value) const
	{
		glUniform3i(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformInt4(const char* name, const glm::ivec4& value) const
	{
		glUniform4i(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformUInt(const char* name, uint32_t value) const
	{
		glUniform1ui(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformUInt2(const char* name, const glm::uvec2& value) const
	{
		glUniform2ui(GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformUInt3(const char* name, const glm::uvec3& value) const
	{
		glUniform3ui(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformUInt4(const char* name, const glm::uvec4& value) const
	{
		glUniform4ui(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformFloat(const char* name, float value) const
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformFloat2(const char* name, const glm::vec2& value) const
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformFloat3(const char* name, const glm::vec3& value) const
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformFloat4(const char* name, const glm::vec4& value) const
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformMat3(const char* name, const glm::mat3& value) const
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLShader::SetUniformMat4(const char* name, const glm::mat4& value) const
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	uint32_t OpenGLShader::GetUniformLocation(const char* name) const
	{
		auto& mapLocation = m_UniformCache.find(name);
		LOG_ASSERT(mapLocation != m_UniformCache.end(), "Uniform could not found!");
		return mapLocation->second;
	}

	void OpenGLShader::CalculateUniformLocations()
	{
		GLint uniform_count = 0;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &uniform_count);

		if (uniform_count != 0)
		{
			GLint 	max_name_len = 0;
			GLsizei length = 0;
			GLsizei count = 0;
			GLenum 	type = GL_NONE;
			glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

			auto uniform_name = std::make_unique<char[]>(max_name_len);

			for (GLint i = 0; i < uniform_count; ++i)
			{
				glGetActiveUniform(m_RendererID, i, max_name_len, &length, &count, &type, uniform_name.get());

				uint32_t location = glGetUniformLocation(m_RendererID, uniform_name.get());

				m_UniformCache.emplace(std::make_pair(std::string(uniform_name.get(), length), location));
			}
		}
	}

	void OpenGLShader::Compile(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const char* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Vertex shader compilation failed
			LOG_ERROR("Vertex Shader complation failed: {0}", infoLog.data());
			LOG_ASSERT(false, "Vertex Shader compilation failed!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Fragment shader compilation failed
			LOG_ERROR("Fragment Shader compilation failed: {0}", infoLog.data());
			LOG_ASSERT(false, "Fragment Shader compilation failed!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);

		// Link our program
		glLinkProgram(m_RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Shader linking failed
			LOG_ERROR("Shader Linking failed: {0}", infoLog.data());
			LOG_ASSERT(false, "Shader Linking failed!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
	}

	std::unordered_map<uint32_t, std::string> OpenGLShader::ParseShaderSource(const std::string& source)
	{
		auto ShaderType = [](const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			else if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			return 0;
		};

		std::unordered_map<uint32_t, std::string> shaderSources;

		size_t begin = source.find("#type", 0);
		while (begin != std::string::npos)
		{
			size_t end = source.find_first_of("\r\n", begin);
			// Syntax error
			LOG_ASSERT(end != std::string::npos, "Shader program: Syntax Error!");
			begin = begin + strlen("#type") + 1;
			std::string type = source.substr(begin, end - begin);
			// Invalid shader type
			LOG_ASSERT(ShaderType(type), "Invalid Shader type!");

			begin = source.find_first_not_of("\r\n", end);
			//  Syntax error
			LOG_ASSERT(begin != std::string::npos, "Shader program: Syntax Error!");
			end = source.find("#type", begin);

			shaderSources[ShaderType(type)] = (end == std::string::npos) ? source.substr(begin) : source.substr(begin, end - begin);
			begin = end;
		}
		return shaderSources;
	}
}