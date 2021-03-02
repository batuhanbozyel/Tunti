#pragma once
#include "Mesh.h"

namespace Tunti
{
	struct MeshRenderer
	{
		uint32_t BaseVertex = 0;
		uint32_t BaseIndex = 0;
		uint32_t Count = 0;
	};
	
	class BufferManager
	{
	public:
		static MeshRenderer AllocateGraphicsBuffer(const Mesh& mesh, size_t key);
	};
}