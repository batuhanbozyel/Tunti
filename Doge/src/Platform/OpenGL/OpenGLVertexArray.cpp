#include "pch.h"
#include <glad/glad.h>
#include "OpenGLVertexArray.h"
#include "OpenGLBuffer.h"

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

	void OpenGLVertexArray::BindVertexBuffer(const OpenGLVertexBuffer& vertexBuffer, uint32_t binding)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		glVertexArrayVertexBuffer(m_RendererID, binding, vertexBuffer.m_BufferHandle, 0, vertexBuffer.m_Layout.GetStride());
	}

	void OpenGLVertexArray::BindIndexBuffer(const OpenGLIndexBuffer& indexBuffer)
	{
		glVertexArrayElementBuffer(m_RendererID, indexBuffer.m_BufferHandle);
	}

	void OpenGLVertexArray::SetBufferLayout(const BufferLayout& layout, uint32_t binding, uint32_t divisor)
	{
		LOG_ASSERT(binding < 16, "VertexArrayBinding must be in range 0 to 16");

		auto ConvertToBaseType = [](const ShaderDataType::Type type)
		{
			switch (type)
			{
				case ShaderDataType::Float:    return GL_FLOAT;
				case ShaderDataType::Float2:   return GL_FLOAT;
				case ShaderDataType::Float3:   return GL_FLOAT;
				case ShaderDataType::Float4:   return GL_FLOAT;
				case ShaderDataType::Mat3:     return GL_FLOAT;
				case ShaderDataType::Mat4:     return GL_FLOAT;
				case ShaderDataType::Int:      return GL_INT;
				case ShaderDataType::Int2:     return GL_INT;
				case ShaderDataType::Int3:     return GL_INT;
				case ShaderDataType::Int4:     return GL_INT;
				case ShaderDataType::UInt:     return GL_UNSIGNED_INT;
				case ShaderDataType::UInt2:    return GL_UNSIGNED_INT;
				case ShaderDataType::UInt3:    return GL_UNSIGNED_INT;
				case ShaderDataType::UInt4:    return GL_UNSIGNED_INT;
				case ShaderDataType::Bool:     return GL_BOOL;
			}

			LOG_ASSERT(false, "OpenGL Base Type: Unknown Shader data type!");
			return 0;
		};

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
						ConvertToBaseType(element.Type),
						element.Offset);
					break;
				default:
					glVertexArrayAttribFormat(m_RendererID,
						m_VertexAttribIndex,
						element.GetComponentCount(),
						ConvertToBaseType(element.Type),
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