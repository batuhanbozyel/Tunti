#include "tpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Tunti
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::CurrentAPI())
		{
			case RendererAPI::None:     T_CORE_ASSERT(false, "Shader: Renderer API is not specified!"); return nullptr;
			case RendererAPI::OpenGL:   return new OpenGLShader(vertexSrc, fragmentSrc);
		}
	}
}