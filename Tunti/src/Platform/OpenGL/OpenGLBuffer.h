#pragma once

#include "Tunti/Renderer/Buffer.h"

namespace Tunti
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_ID;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_ID;
	};
}