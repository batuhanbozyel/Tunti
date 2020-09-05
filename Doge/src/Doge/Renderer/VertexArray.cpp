#include "pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Doge
{
	std::shared_ptr<Doge::VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
}