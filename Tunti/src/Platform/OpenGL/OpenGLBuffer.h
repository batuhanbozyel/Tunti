#pragma once

#include "Tunti/Renderer/Buffer.h"

namespace Tunti
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();
	private:
		uint32_t m_ID;
		friend VertexBuffer;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual inline uint32_t GetCount() const override { return m_Count; }
	private:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();
	private:
		uint32_t m_ID;
		uint32_t m_Count;
		friend IndexBuffer;
	};
}