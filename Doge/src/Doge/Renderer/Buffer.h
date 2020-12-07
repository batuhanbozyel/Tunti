#pragma once
#include "ShaderDataType.h"

#include "Doge/Core/Base.h"

namespace Doge
{
	struct BufferElement
	{
		std::string Name;
		ShaderDataType::Type Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType::Type type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataType::Size(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Mat3:    return 3 * 3;
				case ShaderDataType::Mat4:    return 4 * 4;
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::UInt:    return 1;
				case ShaderDataType::UInt2:   return 2;
				case ShaderDataType::UInt3:   return 3;
				case ShaderDataType::UInt4:   return 4;
				case ShaderDataType::Bool:    return 1;
			}

			LOG_ASSERT(false, "Unknown ShaderDataType specified!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout()
			: m_Stride(0) {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements), m_Stride(0)
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};
}