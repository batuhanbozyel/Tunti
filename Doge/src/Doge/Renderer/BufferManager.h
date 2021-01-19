#pragma once

namespace Doge
{
	struct Mesh;

	struct GraphicsBuffer
	{
		uint64_t* VertexBuffer = nullptr;
		uint32_t BaseVertex = 0;

		uint64_t* IndexBuffer = nullptr;
		uint32_t Count = 0;
	};
	
	class BufferManager
	{
	public:
		static GraphicsBuffer AllocateGraphicsBuffer(const Mesh& mesh, size_t key);
	};
}