#pragma once
#include "Material.h"

namespace Doge
{
#pragma pack(push, 1)
	struct Vertex
	{
		Vertex() {}

		Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& tex, uint32_t index = 0)
			: Position(pos, 1.0f), Normal(normal), TexCoord(tex), TexIndex(index) {}

		Vertex(const glm::vec4& pos, const glm::vec3& normal, const glm::vec2& tex, uint32_t index = 0)
			: Position(pos), Normal(normal), TexCoord(tex), TexIndex(index) {}

		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		uint32_t TexIndex;
	};
#pragma pack(pop)

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
			: m_Vertices(vertices), m_Indices(indices) {}

		void Translate(const glm::vec3& pos)
		{
			auto translation = glm::translate(glm::mat4(1.0f), pos);
			for (auto& vertex : m_Vertices)
			{
				vertex.Position = translation * vertex.Position;
			}
		}

		void Rotate(float angle, const glm::vec3& axis = glm::vec3(-1.0f, 0.0f, 0.0f))
		{
			auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
			for (auto& vertex : m_Vertices)
			{
				vertex.Position = rotation * vertex.Position;
				vertex.Normal = rotation * glm::vec4(vertex.Normal, 0.0f);
			}
		}

		inline const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		inline const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}