#include "pch.h"
#include "Buffer.h"

namespace Doge
{
	/////////// VertexBuffer ///////////

	VertexBuffer::VertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, size, vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetData(const float* vertices, uint32_t offset, uint32_t size)
	{
		glNamedBufferSubData(m_RendererID, offset, size, vertices);
	}

	/////////// IndexBuffer ///////////

	IndexBuffer::IndexBuffer(uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, count * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::SetData(const uint32_t* indices, uint32_t offset, uint32_t count)
	{
		glNamedBufferSubData(m_RendererID, offset, sizeof(uint32_t) * count, indices);
	}

	/////////// ShaderStorageBuffer ///////////

	ShaderStorageBuffer::ShaderStorageBuffer(uint32_t size, uint32_t location)
		: m_Location(location)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void ShaderStorageBuffer::Bind() const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Location, m_RendererID);
	}

	void ShaderStorageBuffer::Unbind() const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Location, 0);
	}

	void ShaderStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	/////////// UniformBuffer ///////////

	UniformBuffer::UniformBuffer(uint32_t size, uint32_t location)
		: m_Location(location)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferStorage(m_RendererID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);

	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::Bind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Location, m_RendererID);
	}

	void UniformBuffer::Unbind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Location, 0);
	}

	void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}