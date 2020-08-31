#include "pch.h"
#include "SceneObject3D.h"

#include "Doge/Renderer/Renderer.h"

namespace Doge
{
	// Cuboid Object Implementations

	Cuboid::Cuboid(glm::vec3 length, uint32_t texIndex)
	{
		m_Vertices.resize(24);
		length = length / 2.0f;

		// Front
		m_Vertices[0] = { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[1] = { glm::vec3(length.x, -length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[2] = { glm::vec3(length.x,  length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[3] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Back
		m_Vertices[4] = { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[5] = { glm::vec3(length.x, -length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[6] = { glm::vec3(length.x,  length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[7] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Left
		m_Vertices[8] = { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[9] = { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[10] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[11] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Right	
		m_Vertices[12] = { glm::vec3(length.x, -length.y, -length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[13] = { glm::vec3(length.x, -length.y,  length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[14] = { glm::vec3(length.x,  length.y,  length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[15] = { glm::vec3(length.x,  length.y, -length.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Top
		m_Vertices[16] = { glm::vec3(-length.x,  length.y, -length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[17] = { glm::vec3(length.x,  length.y, -length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[18] = { glm::vec3(length.x,  length.y,  length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[19] = { glm::vec3(-length.x,  length.y,  length.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
		// Bottom
		m_Vertices[20] = { glm::vec3(-length.x, -length.y, -length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), texIndex };
		m_Vertices[21] = { glm::vec3(length.x, -length.y, -length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), texIndex };
		m_Vertices[22] = { glm::vec3(length.x, -length.y,  length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), texIndex };
		m_Vertices[23] = { glm::vec3(-length.x, -length.y,  length.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), texIndex };
	}

	void Cuboid::Transform(const glm::mat4& transform)
	{
		for (auto& vertex : m_Vertices)
		{
			vertex.Position = transform * vertex.Position;
			vertex.Normal = transform * glm::vec4(vertex.Normal, 0.0f);
		}
	}

	const std::vector<uint32_t> Cuboid::CalculateIndices(uint32_t& offset)
	{
		std::vector<uint32_t> indices(36);
		for (uint32_t i = 0; i < 36; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		return indices;
	}

	// Sphere Object Implementations

	Sphere::Sphere(float radius, uint32_t sectorCount, uint32_t stackCount, uint32_t texIndex)
		: m_SectorCount(sectorCount), m_StackCount(stackCount)
	{
		radius = radius / 2.0f;
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;		// vertex normal
		float s, t;                                     // vertex texCoord

		float sectorCounttep = 2 * glm::pi<float>() / sectorCount;
		float stackStep = glm::pi<float>() / stackCount;
		float sectorAngle, stackAngle;

		for (uint32_t i = 0; i <= stackCount; i++)
		{
			stackAngle = i * stackStep - glm::pi<float>() / 2;		// starting from -pi/2 to pi/2
			xy = radius * glm::cos(stackAngle);
			z = radius * glm::sin(stackAngle);

			for (uint32_t j = 0; j <= sectorCount; j++)
			{
				sectorAngle = j * sectorCounttep;		// starting from 0 to 2pi

				x = xy * glm::cos(sectorAngle);
				y = xy * glm::sin(sectorAngle);
				glm::vec3 pos(x, y, z);

				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				glm::vec3 normal(nx, ny, nz);

				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				glm::vec2 texCoord(s, t);

				m_Vertices.push_back(Vertex(pos, normal, texCoord, texIndex));
			}
		}
		glm::mat4 transform(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
		for (auto& vertex : m_Vertices)
		{
			vertex.Position = transform * vertex.Position;
			vertex.Normal = transform * glm::vec4(vertex.Normal, 0.0f);
		}
	}

	void Sphere::Transform(const glm::mat4& transform)
	{
		for (auto& vertex : m_Vertices)
		{
			vertex.Position = transform * vertex.Position;
			vertex.Normal = transform * glm::vec4(vertex.Normal, 0.0f);
		}
	}

	const std::vector<uint32_t> Sphere::CalculateIndices(uint32_t& offset)
	{
		std::vector<uint32_t> indices;
		int k1, k2;
		for (uint32_t i = 0; i < m_StackCount; ++i)
		{
			k1 = i * (m_SectorCount + 1);     // beginning of current stack
			k2 = k1 + m_SectorCount + 1;      // beginning of next stack

			for (uint32_t j = 0; j < m_SectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(offset + k1);
					indices.push_back(offset + k2);
					indices.push_back(offset + k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (m_StackCount - 1))
				{
					indices.push_back(offset + k1 + 1);
					indices.push_back(offset + k2);
					indices.push_back(offset + k2 + 1);
				}
			}
		}
		offset += static_cast<uint32_t>(m_Vertices.size());
		m_IndexCount = static_cast<uint32_t>(indices.size());
		return indices;
	}
}