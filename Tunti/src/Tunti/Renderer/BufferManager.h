#pragma once
#include "Mesh.h"

namespace Tunti
{
	class BufferManager
	{
	public:
		static MeshBuffer AllocateMeshBufferWithKey(size_t key);
		static SubmeshBuffer AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& data);
	};
}