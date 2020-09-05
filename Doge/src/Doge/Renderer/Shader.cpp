#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>

namespace Doge
{
	// Shader Program initialization methods

	std::shared_ptr<Shader> Shader::Create(const char* filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return std::make_shared<OpenGLShader>(filePath);
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	std::shared_ptr<Doge::Shader> Shader::Create(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	std::string Shader::ReadFile(const char* filePath)
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
				LOG_ASSERT(false, "Could not read from file!");
			}
		}
		else
		{
			// Could not open file
			LOG_ASSERT(false, "Could not open file!");
		}
		return source;
	}


	bool Shader::operator==(const Shader& shader) const
	{
		return m_RendererID == shader.m_RendererID;
	}

	bool Shader::operator!=(const Shader& shader) const
	{
		return m_RendererID != shader.m_RendererID;
	}

	// ShaderLibrary

	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_ShaderCache;

	std::shared_ptr<Shader> ShaderLibrary::CreateShader(const char* filePath)
	{
		auto& shaderIt = s_ShaderCache.find(filePath);
		if (shaderIt == s_ShaderCache.end())
		{
			std::shared_ptr<Shader> shader = Shader::Create(filePath);
			s_ShaderCache.emplace(std::make_pair(filePath, shader));
			return shader;
		}
		return shaderIt->second;
	}

	std::shared_ptr<Shader> ShaderLibrary::CreateShader(const char* name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		auto& shaderIt = s_ShaderCache.find(name);
		if (shaderIt == s_ShaderCache.end())
		{
			std::shared_ptr<Shader> shader = Shader::Create(name, vertexSrc, fragmentSrc);
			s_ShaderCache.emplace(std::make_pair(name, shader));
			return shader;
		}
		return shaderIt->second;
	}

	Shader& ShaderLibrary::GetShader(const std::string& shader)
	{
		return *s_ShaderCache.find(shader)->second;
	}
}