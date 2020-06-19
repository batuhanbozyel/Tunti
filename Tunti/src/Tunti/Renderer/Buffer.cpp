#include "tpch.h"
#include "Renderer.h"
#include "Buffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Tunti
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::CurrentAPI())
		{
			case RendererAPI::None:     T_CORE_ASSERT(false, "VertexBuffer: Renderer API is not specified!"); return nullptr;
			case RendererAPI::OpenGL:   return new OpenGLVertexBuffer(vertices, size);
		}
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::CurrentAPI())
		{
			case RendererAPI::None:     T_CORE_ASSERT(false, "IndexBuffer: Renderer API is not specified!"); return nullptr;
			case RendererAPI::OpenGL:   return new OpenGLIndexBuffer(indices, size);
		}
	}
}