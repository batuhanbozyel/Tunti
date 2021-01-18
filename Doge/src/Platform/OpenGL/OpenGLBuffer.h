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
	private:
		GLuint m_BufferHandle;

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

		uint32_t GetCount() const { return m_Count; }
	private:
		GLuint m_BufferHandle;
		uint32_t m_Count;

		friend class OpenGLVertexArray;
	};

	class OpenGLShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t size);
		~OpenGLShaderStorageBuffer();

		void Bind(uint32_t location) const;
		void Unbind(uint32_t location) const;

		void SetData(const void* data, uint32_t offset, uint32_t size) const;
	private:
		GLuint m_BufferHandle;
	};

	class OpenGLUniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size);
		~OpenGLUniformBuffer();

		void Bind(uint32_t location) const;
		void Unbind(uint32_t location) const;

		void SetData(const void* data, uint32_t size, uint32_t offset) const;
	private:
		GLuint m_BufferHandle;
	};

	class OpenGLBufferManager
	{
	public:
		~OpenGLBufferManager() = default;

		template<typename BufferType T, typename ... Args>
		Buffer<T> AllocateBuffer(Args&& ... args);

		void Flush();

		static OpenGLBufferManager* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLBufferManager;

			return s_Instance;
		}

		const Ref<OpenGLVertexBuffer>& operator[](const Buffer<BufferType::Vertex> buffer) const
		{
			return m_VertexBufferCache[buffer];
		}

		const Ref<OpenGLIndexBuffer>& operator[](const Buffer<BufferType::Index> buffer) const
		{
			return m_IndexBufferCache[buffer];
		}

		const Ref<OpenGLUniformBuffer>& operator[](const Buffer<BufferType::Uniform> buffer) const
		{
			return m_UniformBufferCache[buffer];
		}

		const Ref<OpenGLShaderStorageBuffer>& operator[](const Buffer<BufferType::ShaderStorage> buffer) const
		{
			return m_ShaderStorageBufferCache[buffer];
		}

		OpenGLBufferManager(const OpenGLBufferManager& other) = delete;
		OpenGLBufferManager& operator=(const OpenGLBufferManager& other) = delete;
	private:
		explicit OpenGLBufferManager() = default;
	private:
		std::vector<Ref<OpenGLVertexBuffer>> m_VertexBufferCache;
		std::vector<Ref<OpenGLIndexBuffer>> m_IndexBufferCache;
		std::vector<Ref<OpenGLUniformBuffer>> m_UniformBufferCache;
		std::vector<Ref<OpenGLShaderStorageBuffer>> m_ShaderStorageBufferCache;

		static OpenGLBufferManager* s_Instance;
	};
}