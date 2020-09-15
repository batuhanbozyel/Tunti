#pragma once
#include "Buffer.h"

namespace Doge
{
	class VertexArray
	{
	public:
		static std::unique_ptr<VertexArray> Create();

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t binding) = 0;
		virtual void BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		virtual void SetBufferLayout(const BufferLayout& layout, uint32_t binding, uint32_t divisor = 0) = 0;
	protected:
		uint32_t m_RendererID;
		uint32_t m_VertexAttribIndex = 0;
	};
}