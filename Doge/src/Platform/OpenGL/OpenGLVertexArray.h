#pragma once

namespace Doge
{
	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;

	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;

		void BindVertexBuffer(const OpenGLVertexBuffer& vertexBuffer, uint32_t binding);
		void BindIndexBuffer(const OpenGLIndexBuffer& indexBuffer);

		void SetBufferLayout(const BufferLayout& layout, uint32_t binding, uint32_t divisor = 0);
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexAttribIndex = 0;
	};
}