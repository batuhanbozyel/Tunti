#include "pch.h"
#include "Shader.h"
#include "Renderer.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>

namespace Tunti
{
	std::string ShaderLibrary::ReadFile(const std::string& filePath)
	{
		std::string source;
		std::ifstream shaderFile(filePath, std::ios::in | std::ios::binary);
		if (shaderFile)
		{
			shaderFile.seekg(0, std::ios::end);
			size_t size = shaderFile.tellg();
			if (size != -1)
			{
				source.resize(size);
				shaderFile.seekg(0, std::ios::beg);
				shaderFile.read(&source[0], size);
				shaderFile.close();
			}
			else
			{
				LOG_ASSERT(false, "Could not read from shader file!");
			}
		}
		else
		{
			// Could not open file
			LOG_ASSERT(false, "Could not open shader file!");
		}
		return source;
	}

	Shader ShaderLibrary::LoadShader(const std::string& filePath)
	{
		const std::string& source = ReadFile(filePath);

		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return Shader();
			case RendererAPI::OpenGL: return OpenGLShaderCache::GetInstance()->LoadShaderProgram(filePath, source);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return Shader();
	}

	Shader ShaderLibrary::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return Shader();
			case RendererAPI::OpenGL: return OpenGLShaderCache::GetInstance()->LoadShaderProgram(name, vertexSrc, fragmentSrc);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return Shader();
	}

	Shader ShaderLibrary::LoadComputeShader(const std::string& filePath)
	{
		const std::string& source = ReadFile(filePath);

		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return Shader();
			case RendererAPI::OpenGL: return OpenGLShaderCache::GetInstance()->LoadComputeShader(source);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return Shader();
	}

	std::unordered_map<std::string, UniformProperty> ShaderLibrary::GetMaterialInfo(Shader shader)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				LOG_ASSERT(false, "RendererAPI is not specified!");
				return std::unordered_map<std::string, UniformProperty>();
			}
			case RendererAPI::OpenGL:
			{
				Ref<OpenGLShaderProgram> apiShader = (*OpenGLShaderCache::GetInstance())[shader];
				return apiShader->GetMaterialInfo();
			}
		}
		LOG_ASSERT(false, "RendererAPI is not specified!");
		return std::unordered_map<std::string, UniformProperty>();
	}
}