#include "pch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace Doge
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::BindVertexBuffer(const VertexBuffer& vertexBuffer, uint32_t binding)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		glVertexArrayVertexBuffer(m_RendererID, binding, vertexBuffer.GetID(), 0, vertexBuffer.GetLayout().GetStride());
	}

	void OpenGLVertexArray::BindIndexBuffer(const IndexBuffer& indexBuffer)
	{
		glVertexArrayElementBuffer(m_RendererID, indexBuffer.GetID());
	}

	void OpenGLVertexArray::SetBufferLayout(const BufferLayout& layout, uint32_t binding, uint32_t divisor)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		for (const auto& element : layout)
		{
			glEnableVertexArrayAttrib(m_RendererID, m_VertexAttribIndex);
			switch (element.Type)
			{
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::UInt:
			case ShaderDataType::UInt2:
			case ShaderDataType::UInt3:
			case ShaderDataType::UInt4:
				glVertexArrayAttribIFormat(m_RendererID,
					m_VertexAttribIndex,
					element.GetComponentCount(),
					ShaderDataType::ConvertToRendererAPIType(element.Type),
					element.Offset);
				break;
			default:
				glVertexArrayAttribFormat(m_RendererID,
					m_VertexAttribIndex,
					element.GetComponentCount(),
					ShaderDataType::ConvertToRendererAPIType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					element.Offset);
				break;
			}
			glVertexArrayAttribBinding(m_RendererID, m_VertexAttribIndex, binding);
			glVertexArrayBindingDivisor(m_RendererID, binding, divisor);
			m_VertexAttribIndex++;
		}
	}
}