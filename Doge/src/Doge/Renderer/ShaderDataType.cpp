#include "pch.h"
#include "ShaderDataType.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShaderDataType.h"

namespace Doge
{
	uint32_t ShaderDataType::ConvertToRendererAPIType(const ShaderDataType::Type type)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return 0;
		case RendererAPI::OpenGL: return ConvertToOpenGLBaseType(type);
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return 0;
	}

}