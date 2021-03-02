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

	struct Mesh
	{
		std::string Name;
		std::vector<glm::vec3> Position;
		std::vector<glm::vec3> Normal;
		std::vector<glm::vec2> TexCoord;
		std::vector<uint32_t> TexIndex;
		std::vector<uint32_t> Indices;
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
