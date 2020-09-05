#pragma once
#include "ShaderDataType.h"

namespace Doge
{
	class VertexArray;

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

		uint32_t GetComponentCount() const;
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
		static std::shared_ptr<VertexBuffer> Create(uint32_t size);
		static std::shared_ptr<VertexBuffer> Create(const float* vertices, uint32_t size);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const float* vertices, uint32_t offset, uint32_t size) = 0;

		inline const uint32_t GetID() { return m_RendererID; }
		inline const BufferLayout& GetLayout() const { return m_Layout; }
		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	protected:
		BufferLayout m_Layout;
		uint32_t m_RendererID;
	};

	class IndexBuffer
	{
	public:
		static std::shared_ptr<IndexBuffer> Create(uint32_t count);
		static std::shared_ptr<IndexBuffer> Create(const uint32_t* indices, uint32_t count);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const uint32_t* indices, uint32_t offset, uint32_t count) = 0;

		inline const uint32_t GetID() { return m_RendererID; }
		inline uint32_t GetCount() const { return m_Count; }
	protected:
		uint32_t m_Count;
		uint32_t m_RendererID;
	};

	class ShaderStorageBuffer
	{
	public:
		static std::shared_ptr<ShaderStorageBuffer> Create(uint32_t size, uint32_t location);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t offset, uint32_t size) = 0;
	protected:
		uint32_t m_RendererID;
		uint32_t m_Location;
	};

	class UniformBuffer
	{
	public:
		static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t location);

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
	protected:
		uint32_t m_RendererID;
		uint32_t m_Location;
	};
}