#pragma once
#include "ShaderDataType.h"

namespace Doge
{
	class VertexArray;

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
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

	class VertexBuffer
	{
	public:
		friend class VertexArray;

		VertexBuffer(uint32_t size);
		VertexBuffer(const float* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const float* vertices, uint32_t offset, uint32_t size);
		const BufferLayout& GetLayout() const { return m_Layout; }
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		friend class VertexArray;

		IndexBuffer(uint32_t count);
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const uint32_t* indices, uint32_t offset, uint32_t count);
		inline uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class ShaderStorageBuffer
	{
	public:
		ShaderStorageBuffer(uint32_t size, uint32_t location);
		~ShaderStorageBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t offset, uint32_t size);
	private:
		uint32_t m_RendererID;
		uint32_t m_Location;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t size, uint32_t location);
		~UniformBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size, uint32_t offset);
	private:
		uint32_t m_RendererID;
		uint32_t m_Location;
	};
}