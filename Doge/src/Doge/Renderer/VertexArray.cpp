#include "pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Doge
{
	Scope<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLVertexArray>();
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
}