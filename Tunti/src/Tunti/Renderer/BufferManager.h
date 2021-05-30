#pragma once
#include "Mesh.h"

namespace Tunti
{
	struct MeshBuffer
	{
		uint32_t Key = 0;
		uint32_t Index = 0;
		bool IsValid = false;

		bool operator==(const Tunti::MeshBuffer& other) const
		{
			return this->Index == other.Index;
		}
	};

	struct SubmeshBuffer
	{
		uint32_t BaseVertex = 0;
		uint32_t BaseIndex = 0;
		uint32_t Count = 0;
	};

	class BufferManager
	{
	public:
		static MeshBuffer AllocateMeshBufferWithKey(size_t key);
		static SubmeshBuffer AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& data);
	};
}

template<>
struct std::hash<Tunti::MeshBuffer>
{
	size_t operator()(Tunti::MeshBuffer const& buffer) const
	{
		return std::hash<uint32_t>()(buffer.Index);
	}
};