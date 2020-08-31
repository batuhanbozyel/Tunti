#include "pch.h"
#include "VertexArray.h"

namespace Doge
{
	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t binding)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		glVertexArrayVertexBuffer(m_RendererID, binding, vertexBuffer->m_RendererID, 0, vertexBuffer->m_Layout.GetStride());
	}

	void VertexArray::BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glVertexArrayElementBuffer(m_RendererID, indexBuffer->m_RendererID);
	}

	void VertexArray::SetBufferLayout(const BufferLayout& layout, uint32_t binding)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		//uint32_t index = 0;
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
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Offset);
				break;
			default:
				glVertexArrayAttribFormat(m_RendererID,
					m_VertexAttribIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					element.Offset);
				break;
			}
			glVertexArrayAttribBinding(m_RendererID, m_VertexAttribIndex, binding);
			m_VertexAttribIndex++;
		}
	}
}