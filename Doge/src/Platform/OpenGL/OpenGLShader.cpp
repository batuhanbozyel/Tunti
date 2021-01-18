#include "pch.h"
#include <glad/glad.h>
#include "OpenGLShader.h"

namespace Doge
{
	OpenGLShader::OpenGLShader(const std::string& source)
	{
		auto& parsedSource = ParseShaderSource(source);
		Compile(parsedSource[GL_VERTEX_SHADER], parsedSource[GL_FRAGMENT_SHADER]);
		CalculateUniformLocations();
	}

	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		Compile(vertexSrc, fragmentSrc);
		CalculateUniformLocations();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ShaderHandle);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_ShaderHandle);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt(const char* name, int value) const
	{
		glProgramUniform1i(m_ShaderHandle, GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformInt2(const char* name, const glm::ivec2& value) const
	{
		glProgramUniform2i(m_ShaderHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformInt3(const char* name, const glm::ivec3& value) const
	{
		glProgramUniform3i(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformInt4(const char* name, const glm::ivec4& value) const
	{
		glProgramUniform4i(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformUInt(const char* name, uint32_t value) const
	{
		glProgramUniform1ui(m_ShaderHandle, GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformUInt2(const char* name, const glm::uvec2& value) const
	{
		glProgramUniform2ui(m_ShaderHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformUInt3(const char* name, const glm::uvec3& value) const
	{
		glProgramUniform3ui(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformUInt4(const char* name, const glm::uvec4& value) const
	{
		glProgramUniform4ui(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformFloat(const char* name, float value) const
	{
		glProgramUniform1f(m_ShaderHandle, GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniformFloat2(const char* name, const glm::vec2& value) const
	{
		glProgramUniform2f(m_ShaderHandle, GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniformFloat3(const char* name, const glm::vec3& value) const
	{
		glProgramUniform3f(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformFloat4(const char* name, const glm::vec4& value) const
	{
		glProgramUniform4f(m_ShaderHandle, GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformMat3(const char* name, const glm::mat3& value) const
	{
		glProgramUniformMatrix3fv(m_ShaderHandle, GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLShader::SetUniformMat4(const char* name, const glm::mat4& value) const
	{
		glProgramUniformMatrix4fv(m_ShaderHandle, GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
	}

	std::unordered_map<std::string, UniformProperty> OpenGLShader::GetMaterialInfo() const
	{
		std::unordered_map<std::string, UniformProperty> uniforms;

		for (const auto& uniform : m_UniformCache)
			if(uniform.second.Type != MaterialDataIndex::NONE)
				uniforms.insert(uniform);

		return uniforms;
	}

	uint32_t OpenGLShader::GetUniformLocation(const char* name) const
	{
		auto& mapLocation = m_UniformCache.find(name);
		LOG_ASSERT(mapLocation != m_UniformCache.end(), "Uniform could not found!");
		return mapLocation->second.Location;
	}

	void OpenGLShader::CalculateUniformLocations()
	{
		GLint uniform_count = 0;
		glGetProgramiv(m_ShaderHandle, GL_ACTIVE_UNIFORMS, &uniform_count);

		if (uniform_count != 0)
		{
			GLint 	max_name_len = 0;
			GLsizei length = 0;
			GLsizei count = 0;
			GLenum 	type = GL_NONE;
			glGetProgramiv(m_ShaderHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

			Scope<char[]> uniform_name = CreateScope<char[]>(max_name_len);

			for (GLint i = 0; i < uniform_count; ++i)
			{
				glGetActiveUniform(m_ShaderHandle, i, max_name_len, &length, &count, &type, uniform_name.get());
				
				UniformProperty prop;
				prop.Location = glGetUniformLocation(m_ShaderHandle, uniform_name.get());
				prop.Type = MaterialDataIndex::NONE;

				switch (type)
				{
					case GL_FLOAT:
					{
						prop.Type = MaterialDataIndex::Float;
						break;
					}
					case GL_FLOAT_VEC2:
					{
						prop.Type = MaterialDataIndex::Float2;
						break;
					}
					case GL_FLOAT_VEC3:
					{
						prop.Type = MaterialDataIndex::Float3;
						break;
					}
					case GL_FLOAT_VEC4:
					{
						prop.Type = MaterialDataIndex::Float4;
						break;
					}
				}
				m_UniformCache.emplace(std::make_pair(std::string(uniform_name.get(), length), prop));
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
		m_ShaderHandle = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_ShaderHandle, vertexShader);
		glAttachShader(m_ShaderHandle, fragmentShader);

		// Link our program
		glLinkProgram(m_ShaderHandle);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_ShaderHandle, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ShaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ShaderHandle, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_ShaderHandle);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Shader linking failed
			Log::Error("Shader Linking failed: ");
			LOG_ASSERT(false, infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_ShaderHandle, vertexShader);
		glDetachShader(m_ShaderHandle, fragmentShader);
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

	// OpenGLShaderCache

	OpenGLShaderCache* OpenGLShaderCache::s_Instance = nullptr;

	Shader OpenGLShaderCache::LoadShader(const std::string& filePath, const std::string& source)
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
			Ref<OpenGLShader> newShader = CreateRef<OpenGLShader>(source);
			shader.Handle = newShader->m_ShaderHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { filePath, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
		}

		return shader;
	}

	Shader OpenGLShaderCache::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
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
			Ref<OpenGLShader> newShader = CreateRef<OpenGLShader>(vertexSrc, fragmentSrc);
			shader.Handle = newShader->m_ShaderHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { name, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
		}

		return shader;
	}

	Ref<OpenGLShader> OpenGLShaderCache::LoadShader(const std::string& filepath)
	{
		std::string source = ShaderLibrary::ReadFile(filepath);

		Ref<OpenGLShader> shaderRef;
		const auto& shaderFileIt = m_ShaderFiles.find(filepath);

		if (shaderFileIt != m_ShaderFiles.end())
		{
			Shader shader = shaderFileIt->second;
			LOG_ASSERT(m_Shaders.find(shader) != m_Shaders.end(), "OpenGL Shader Program is not loaded properly!");
			shaderRef = m_Shaders.find(shader)->second;
		}
		else
		{
			Ref<OpenGLShader> newShader = CreateRef<OpenGLShader>(source);
			Shader shader = newShader->m_ShaderHandle;
			m_ShaderFiles.insert(m_ShaderFiles.end(), { filepath, shader });
			m_Shaders.insert(m_Shaders.end(), { shader, newShader });
			return newShader;
		}

		return shaderRef;
	}

	void OpenGLShaderCache::Flush()
	{
		m_ShaderFiles.clear();
		m_Shaders.clear();
	}
}