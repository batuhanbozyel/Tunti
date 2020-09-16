#include "pch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Doge
{
	uint32_t BufferElement::GetComponentCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3 * 3;
		case ShaderDataType::Mat4:    return 4 * 4;
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::UInt:    return 1;
		case ShaderDataType::UInt2:   return 2;
		case ShaderDataType::UInt3:   return 3;
		case ShaderDataType::UInt4:   return 4;
		case ShaderDataType::Bool:    return 1;
		}

		LOG_ASSERT(false, "Unknown ShaderDataType specified!");
		return 0;
	}

	/////////// VertexBuffer ///////////

	Scope<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLVertexBuffer>(size);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Scope<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLVertexBuffer>(vertices, size);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	/////////// IndexBuffer ///////////

	Scope<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLIndexBuffer>(count);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Scope<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLIndexBuffer>(indices, count);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	/////////// ShaderStorageBuffer ///////////

	Scope<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t size, uint32_t location)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLShaderStorageBuffer>(size, location);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
	
	/////////// UniformBuffer ///////////

	Scope<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t location)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!");  return nullptr;
		case RendererAPI::OpenGL: return CreateScope<OpenGLUniformBuffer>(size, location);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
}