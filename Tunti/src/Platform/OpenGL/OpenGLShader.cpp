#include "pch.h"
#include <glad/glad.h>
#include "OpenGLShader.h"

namespace Tunti
{
	OpenGLShaderProgram::OpenGLShaderProgram(const std::string& source)
	{
		auto& parsedSource = ParseShaderSource(source);
		Compile(parsedSource[GL_VERTEX_SHADER], parsedSource[GL_FRAGMENT_SHADER]);
		CalculateUniformLocations();
	}

	OpenGLShaderProgram::OpenGLShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		Compile(vertexSrc, fragmentSrc);
		CalculateUniformLocations();
	}

	OpenGLShaderProgram::~OpenGLShaderProgram()
	{
		glDeleteProgram(m_ShaderProgramHandle);
	}

	void OpenGLShaderProgram::Bind() const
	{
		glUseProgram(m_ShaderProgramHandle);
	}

	void OpenGLShaderProgram::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShaderProgram::SetUniformInt(const char* name, int value) const
	{
		glProgramUniform1i(m_ShaderProgramHandle, GetUniformLocation(name), value);
	}

	void OpenGLShaderProgram::SetUniformInt2(const char* name, const glm::ivec2& value) const
	{
		glProgramUniform2i(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShaderProgram::SetUniformInt3(const char* name, const glm::ivec3& value) const
	{
		glProgramUniform3i(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShaderProgram::SetUniformInt4(const char* name, const glm::ivec4& value) const
	{
		glProgramUniform4i(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShaderProgram::SetUniformUInt(const char* name, uint32_t value) const
	{
		glProgramUniform1ui(m_ShaderProgramHandle, GetUniformLocation(name), value);
	}

	void OpenGLShaderProgram::SetUniformUInt2(const char* name, const glm::uvec2& value) const
	{
		glProgramUniform2ui(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShaderProgram::SetUniformUInt3(const char* name, const glm::uvec3& value) const
	{
		glProgramUniform3ui(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShaderProgram::SetUniformUInt4(const char* name, const glm::uvec4& value) const
	{
		glProgramUniform4ui(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShaderProgram::SetUniformFloat(const char* name, float value) const
	{
		glProgramUniform1f(m_ShaderProgramHandle, GetUniformLocation(name), value);
	}

	void OpenGLShaderProgram::SetUniformFloat2(const char* name, const glm::vec2& value) const
	{
		glProgramUniform2f(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShaderProgram::SetUniformFloat3(const char* name, const glm::vec3& value) const
	{
		glProgramUniform3f(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShaderProgram::SetUniformFloat4(const char* name, const glm::vec4& value) const
	{
		glProgramUniform4f(m_ShaderProgramHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShaderProgram::SetUniformMat3(const char* name, const glm::mat3& value) const
	{
		glProgramUniformMatrix3fv(m_ShaderProgramHandle, GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLShaderProgram::SetUniformMat4(const char* name, const glm::mat4& value) const
	{
		glProgramUniformMatrix4fv(m_ShaderProgramHandle, GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	uint32_t OpenGLShaderProgram::GetUniformLocation(const char* name) const
	{
		auto& mapLocation = m_UniformCache.find(name);
		LOG_ASSERT(mapLocation != m_UniformCache.end(), std::string(name) + ": Uniform could not found!");
		return mapLocation->second;
	}

	void OpenGLShaderProgram::CalculateUniformLocations()
	{
		GLint uniform_count = 0;
		glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_UNIFORMS, &uniform_count);

		if (uniform_count != 0)
		{
			GLint     max_name_len = 0;
			GLsizei length = 0;
			GLsizei count = 0;
			GLenum     type = GL_NONE;
			glGetProgramiv(m_ShaderProgramHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

			Scope<char[]> uniform_name = CreateScope<char[]>(max_name_len);

			for (GLint i = 0; i < uniform_count; ++i)
			{
				glGetActiveUniform(m_ShaderProgramHandle, i, max_name_len, &length, &count, &type, uniform_name.get());
				uint32_t location = glGetUniformLocation(m_ShaderProgramHandle, uniform_name.get());

				if (uniform_name[0] == 'u')
					m_UniformCache.emplace(std::make_pair(std::string(uniform_name.get(), length), location));
			}
		}
	}

	void OpenGLShaderProgram::Compile(const std::string& vertexSrc, const std::string& fragmentSrc)
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
			Log::Error("Vertex Shader compilation failed:");
			LOG_ASSERT(false, infoLog.data());
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
			Log::Error("Fragment Shader compilation failed: ");
			LOG_ASSERT(false, infoLog.data());
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ShaderProgramHandle = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_ShaderProgramHandle, vertexShader);
		glAttachShader(m_ShaderProgramHandle, fragmentShader);

		// Link our program
		glLinkProgram(m_ShaderProgramHandle);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_ShaderProgramHandle, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ShaderProgramHandle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ShaderProgramHandle, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_ShaderProgramHandle);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Shader linking failed
			Log::Error("Shader Linking failed: ");
			LOG_ASSERT(false, infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_ShaderProgramHandle, vertexShader);
		glDetachShader(m_ShaderProgramHandle, fragmentShader);
	}

	std::unordered_map<uint32_t, std::string> OpenGLShaderProgram::ParseShaderSource(const std::string& source)
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

	// OpenGLShaderCache

	Shader OpenGLShaderCache::LoadShaderProgram(const std::string& filePath, const std::string& source)
	{
		Shader shader;
		const auto& shaderFileIt = m_ShaderFiles.find(filePath);

		if (shaderFileIt != m_ShaderFiles.end())
		{
			shader = shaderFileIt->second;
			LOG_ASSERT(m_Shaders.find(shader) != m_Shaders.end(), "OpenGL Shader Program is not loaded properly!");
		}
		else
		{
			Ref<OpenGLShaderProgram> newShader = CreateRef<OpenGLShaderProgram>(source);
			shader.Handle = newShader->m_ShaderProgramHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { filePath, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
		}

		return shader;
	}

	Shader OpenGLShaderCache::LoadShaderProgram(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		Shader shader;
		const auto& shaderFileIt = m_ShaderFiles.find(name);

		if (shaderFileIt != m_ShaderFiles.end())
		{
			shader = shaderFileIt->second;
			LOG_ASSERT(m_Shaders.find(shader) != m_Shaders.end(), "OpenGL Shader Program is not loaded properly!");
		}
		else
		{
			Ref<OpenGLShaderProgram> newShader = CreateRef<OpenGLShaderProgram>(vertexSrc, fragmentSrc);
			shader.Handle = newShader->m_ShaderProgramHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { name, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
		}

		return shader;
	}

	Ref<OpenGLShaderProgram> OpenGLShaderCache::LoadShaderProgram(const std::string& filepath)
	{
		std::string source = ShaderLibrary::ReadFile(filepath);

		Ref<OpenGLShaderProgram> shaderRef;
		const auto& shaderFileIt = m_ShaderFiles.find(filepath);

		if (shaderFileIt != m_ShaderFiles.end())
		{
			Shader shader = shaderFileIt->second;
			LOG_ASSERT(m_Shaders.find(shader) != m_Shaders.end(), "OpenGL Shader Program is not loaded properly!");
			shaderRef = m_Shaders.find(shader)->second;
		}
		else
		{
			Ref<OpenGLShaderProgram> newShader = CreateRef<OpenGLShaderProgram>(source);
			Shader shader = newShader->m_ShaderProgramHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { filepath, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
			return newShader;
		}

		return shaderRef;
	}

	Shader OpenGLShaderCache::LoadComputeShader(const std::string& source)
	{
		const GLchar* srcBufferPtr = source.c_str();
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &srcBufferPtr, nullptr);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			Log::Error("Compute Shader compilation failed:");
			LOG_ASSERT(false, infoLog.data());
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, shader);
		glLinkProgram(program);

		glDetachShader(program, shader);
		glDeleteShader(shader);

		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (status == GL_TRUE) {
			glValidateProgram(program);
			glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		}

		if (status == GL_FALSE) {
			GLsizei infoLogSize;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogSize]);
			glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog.get());
			Log::Error("Compute Shader Linking failed: ");
			LOG_ASSERT(false, infoLog.get());
		}

		return program;
	}

	void OpenGLShaderCache::Flush()
	{
		m_ShaderFiles.clear();
		m_Shaders.clear();
	}
}