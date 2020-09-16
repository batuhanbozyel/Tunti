#pragma once
#include "Doge/Renderer/VertexArray.h"

namespace Doge
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindVertexBuffer(const VertexBuffer& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const IndexBuffer& indexBuffer) override;

		virtual void SetBufferLayout(const BufferLayout& layout, uint32_t binding, uint32_t divisor = 0) override;
	};
}