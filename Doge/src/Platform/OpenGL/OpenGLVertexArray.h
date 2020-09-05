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

		virtual void BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual void SetBufferLayout(const BufferLayout& layout, uint32_t binding) override;
	};
}