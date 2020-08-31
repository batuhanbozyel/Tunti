#pragma once
#include "Buffer.h"

namespace Doge
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t binding);
		void BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		void SetBufferLayout(const BufferLayout& layout, uint32_t slot);
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexAttribIndex = 0;
	};
}