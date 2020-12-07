#pragma once
#include "Doge/Renderer/Buffer.h"

namespace Doge
{
	class OpenGLVertexArray;

	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* vertices, uint32_t offset, uint32_t size) const;

		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	private:
		GLuint m_BufferHandle;
		BufferLayout m_Layout;

		friend class OpenGLVertexArray;
	};

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const uint32_t* indices, uint32_t offset, uint32_t count) const;

		inline uint32_t GetCount() const { return m_Count; }
	private:
		GLuint m_BufferHandle;
		uint32_t m_Count;

		friend class OpenGLVertexArray;
	};

	class OpenGLShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t size, uint32_t location);
		~OpenGLShaderStorageBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t offset, uint32_t size) const;
	private:
		GLuint m_BufferHandle;
		uint32_t m_Location;
	};

	class OpenGLUniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t location);
		~OpenGLUniformBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size, uint32_t offset) const;
	private:
		GLuint m_BufferHandle;
		uint32_t m_Location;
	};
}