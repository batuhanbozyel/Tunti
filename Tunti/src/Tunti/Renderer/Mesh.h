#pragma once

namespace Tunti
{
#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};
#pragma pack(pop)

	struct Mesh
	{
		std::vector<glm::vec3> Position;
		std::vector<glm::vec3> Normal;
		std::vector<glm::vec2> TexCoord;
		std::vector<uint32_t> TexIndex;
		std::vector<uint32_t> Indices;
	};
}