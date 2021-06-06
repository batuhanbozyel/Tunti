#pragma once
#include <glm/gtx/hash.hpp>

namespace Tunti
{
#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		bool operator==(const Vertex& other) const
		{
			return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
		}
	};
#pragma pack(pop)

	struct Submesh
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<uint32_t> MaterialIndices;
		std::vector<uint32_t> Indices;
	};

	struct Mesh
	{
		std::vector<Submesh> Submeshes;
	};

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
}

template<>
struct std::hash<Tunti::Vertex>
{
	size_t operator()(Tunti::Vertex const& vertex) const
	{
		return ((std::hash<glm::vec3>()(vertex.Position) ^ (std::hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^ (std::hash<glm::vec2>()(vertex.TexCoord) << 1);
	}
};

template<>
struct std::hash<Tunti::MeshBuffer>
{
	size_t operator()(Tunti::MeshBuffer const& buffer) const
	{
		return std::hash<uint32_t>()(buffer.Index);
	}
};
