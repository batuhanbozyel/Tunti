#pragma once

namespace Doge
{
#pragma pack(push, 1)
	struct FramebufferQuadVertex
	{
		glm::vec2 a_Position;
		glm::vec2 a_TexCoord;

		FramebufferQuadVertex(const glm::vec2& position, const glm::vec2& texCoord)
			: a_Position(position), a_TexCoord(texCoord) {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct QuadVertex
	{
		glm::vec2 a_Position;
		glm::vec2 a_TexCoord;
		uint32_t TexIndex;

		QuadVertex(const glm::vec2& position, const glm::vec2& texCoord, uint32_t index = 0)
			: a_Position(position), a_TexCoord(texCoord), TexIndex(index) {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct Vertex
	{
		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		uint32_t TexIndex;

		Vertex() = default;

		Vertex& operator=(const Vertex& other)
		{
			Position = other.Position;
			Normal   = other.Normal;
			TexCoord = other.TexCoord;
			TexIndex = other.TexIndex;
		}

		Vertex(Vertex&& other) noexcept
			: Position(std::move(other.Position)), Normal(std::move(other.Normal)), TexCoord(std::move(other.TexCoord)), TexIndex(std::move(TexIndex)) {}

		Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& tex, uint32_t index = 0)
			: Position(pos, 1.0f), Normal(normal), TexCoord(tex), TexIndex(index) {}

		Vertex(const glm::vec4& pos, const glm::vec3& normal, const glm::vec2& tex, uint32_t index = 0)
			: Position(pos), Normal(normal), TexCoord(tex), TexIndex(index) {}
	};
#pragma pack(pop)

	struct Mesh
	{
		std::vector<glm::vec4> Position;
		std::vector<glm::vec3> Normal;
		std::vector<glm::vec2> TexCoord;
		std::vector<uint32_t> TexIndex;

		std::vector<uint32_t> Indices;

		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
			: Indices(indices)
		{
			for (const Vertex& vertex : vertices)
			{
				Position.push_back(vertex.Position);
				Normal.push_back(vertex.Normal);
				TexCoord.push_back(vertex.TexCoord);
				TexIndex.push_back(vertex.TexIndex);
			}
		}
	};
}