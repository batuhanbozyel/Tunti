#include "pch.h"
#include <glad/glad.h>
#include "OpenGLBuffer.h"

namespace Doge
{
	/////////// VertexBuffer ///////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, size, vertices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* vertices, uint32_t offset, uint32_t size) const
	{
		glNamedBufferSubData(m_BufferHandle, offset, size, vertices);
	}

	/////////// IndexBuffer ///////////


	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, count * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferHandle);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t offset, uint32_t count) const
	{
		glNamedBufferSubData(m_BufferHandle, offset, sizeof(uint32_t) * count, indices);
	}

	/////////// ShaderStorageBuffer ///////////

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t size, uint32_t location)
		: m_Location(location)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void OpenGLShaderStorageBuffer::Bind() const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Location, m_BufferHandle);
	}

	void OpenGLShaderStorageBuffer::Unbind() const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Location, 0);
	}

	void OpenGLShaderStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size) const
	{
		glNamedBufferSubData(m_BufferHandle, offset, size, data);
	}

	/////////// UniformBuffer ///////////

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t location)
		: m_Location(location)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glNamedBufferStorage(m_BufferHandle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Location, m_BufferHandle);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Location, 0);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) const
	{
		glNamedBufferSubData(m_BufferHandle, offset, size, data);
	}
}