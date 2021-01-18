#pragma once

namespace Doge
{
	enum class BufferType : uint16_t
	{
		Vertex,
		Index,
		Uniform,
		ShaderStorage
	};

	template<typename BufferType T>
	struct Buffer
	{
		uint32_t Index = 0;

		operator uint32_t() const { return Index; }

		constexpr BufferType Type() const { return T; }
	};
	
	class BufferManager
	{
	public:
		template<typename BufferType T, typename ... Args>
		static Buffer<T> AllocateBuffer(Args&& ... args);		
	};
}